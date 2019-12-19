/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Martin Jones <martin.jones@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rawwebview.h"

#include "webengine.h"
#include "webenginesettings.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QStyleHints>
#include <QtGui/QMouseEvent>
#include <QtQuick/QQuickWindow>
#include <private/qquickwindow_p.h>

#define CONTENT_ORIENTATION_CHANGED QLatin1String("embed:contentOrientationChanged")

namespace SailfishOS {

namespace WebView {

RawWebView::RawWebView(QQuickItem *parent)
    : QuickMozView(parent)
    , m_vkbMargin(0.0)
    , m_acceptTouchEvents(true)
{
    addMessageListener(CONTENT_ORIENTATION_CHANGED);

    connect(qGuiApp->inputMethod(), &QInputMethod::visibleChanged, this, [=]() {
        if (qGuiApp->inputMethod()->isVisible()) {
            setFollowItemGeometry(false);
        }
    });

    connect(this, &QuickMozView::recvAsyncMessage, this, &RawWebView::onAsyncMessage);
}

RawWebView::~RawWebView()
{
}

qreal RawWebView::virtualKeyboardMargin() const
{
    return m_vkbMargin;
}

void RawWebView::setVirtualKeyboardMargin(qreal vkbMargin)
{
    if (m_vkbMargin != vkbMargin) {
        m_vkbMargin = vkbMargin;
        setMargins(QMargins(0, 0, 0, m_vkbMargin));
        emit virtualKeyboardMarginChanged();

        QVariantMap map;
        map.insert("imOpen", m_vkbMargin > 0);
        map.insert("pixelRatio", SailfishOS::WebEngineSettings::instance()->pixelRatio());
        map.insert("bottomMargin", m_vkbMargin);
        // These map to max css composition size. Item's geometry might update right after this
        // meaning that height() + m_vkbMargin doesn't yet equal to available max space.
        // Nevertheless, it is not a big deal if we loose a pixel or two from
        // max composition size.
        map.insert("screenWidth", width());
        map.insert("screenHeight", (height() + m_vkbMargin));
        QVariant data(map);
        sendAsyncMessage("embedui:vkbOpenCompositionMetrics", data);

        if (m_vkbMargin == 0) {
            setFollowItemGeometry(true);
        }
    }
}

bool RawWebView::acceptTouchEvents() const
{
    return m_acceptTouchEvents;
}

void RawWebView::setAcceptTouchEvents(bool accept)
{
    if (m_acceptTouchEvents != accept) {
        m_acceptTouchEvents = accept;

        if (!m_acceptTouchEvents) {
            ungrabMouse();
            ungrabTouchPoints();
            setKeepMouseGrab(false);
            setKeepTouchGrab(false);
        }

        emit acceptTouchEventsChanged();
    }
}

void RawWebView::touchEvent(QTouchEvent *event)
{
    if (m_acceptTouchEvents || event->type() != QEvent::TouchBegin) {
        event->setAccepted(true);

        const QList<QTouchEvent::TouchPoint> &touchPoints = event->touchPoints();
        for (QTouchEvent::TouchPoint touchPoint : touchPoints) {
            switch (touchPoint.state()) {
            case Qt::TouchPointPressed:
                if (touchPoints.count() == 1) {
                    m_startPos = touchPoint.scenePos();
                    grabMouse();
                    setKeepMouseGrab(false);
                    setKeepTouchGrab(false);
                } else {
                    setKeepMouseGrab(true);
                    setKeepTouchGrab(true);
                }
                break;
            // fall through
            case Qt::TouchPointMoved: {
                if (touchPoints.count() == 1 && !keepMouseGrab()) {
                    const int dragThreshold = QGuiApplication::styleHints()->startDragDistance();
                    QPointF delta = touchPoint.scenePos() - m_startPos;

                    switch (window()->contentOrientation()) {
                    case Qt::LandscapeOrientation:
                        delta = QPointF(delta.y(), -delta.x());
                        break;
                    case Qt::InvertedLandscapeOrientation:
                        delta = QPointF(-delta.y(), delta.x());
                        break;
                    case Qt::InvertedPortraitOrientation:
                        delta = QPointF(-delta.x(), -delta.y());
                        break;
                    default:
                        break;
                    }

                    if ((delta.y() >= dragThreshold && !atYBeginning())
                            || (delta.y() <= -dragThreshold && !atYEnd())
                            || (delta.x() >= dragThreshold && !atXBeginning())
                            || (delta.x() <= -dragThreshold && !atXEnd())) {
                        setKeepMouseGrab(true);
                        setKeepTouchGrab(true);
                    }
                }
                break;
            }
            case Qt::TouchPointReleased: {
                break;
            }
            default:
                break;
            }
        }

        QuickMozView::touchEvent(event);

        if (event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel) {
            ungrabMouse();
            setKeepMouseGrab(false);
            setKeepTouchGrab(false);
        }
    } else {
        event->setAccepted(false);
    }
}

void RawWebView::onAsyncMessage(const QString &message, const QVariant &data)
{
    if (message == CONTENT_ORIENTATION_CHANGED) {
        QString orientation = data.toMap().value("orientation").toString();
        Qt::ScreenOrientation mappedOrientation = Qt::PortraitOrientation;
        if (orientation == QStringLiteral("landscape-primary")) {
            mappedOrientation = Qt::LandscapeOrientation;
        } else if (orientation == QStringLiteral("landscape-secondary")) {
            mappedOrientation = Qt::InvertedLandscapeOrientation;
        } else if (orientation == QStringLiteral("portrait-secondary")) {
            mappedOrientation = Qt::InvertedPortraitOrientation;
        }
        emit contentOrientationChanged(mappedOrientation);
    }
}

} // namespace WebView

} // namespace SailfishOS

