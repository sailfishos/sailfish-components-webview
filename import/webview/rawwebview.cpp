/****************************************************************************
**
** Copyright (c) 2016 - 2020 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rawwebview.h"

#include "webengine.h"
#include "webenginesettings.h"

#include <QtCore/QtGlobal>
#include <QtCore/QSet>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QStyleHints>
#include <QtGui/QMouseEvent>
#include <QtQuick/QQuickWindow>
#include <private/qquickwindow_p.h>

#define CONTENT_ORIENTATION_CHANGED QLatin1String("embed:contentOrientationChanged")

namespace SailfishOS {

namespace WebView {

namespace {

QSet<RawWebView *> &liveViews()
{
    static QSet<RawWebView *> views;
    return views;
}

}

RawWebView::RawWebView(QQuickItem *parent)
    : QuickMozView(parent)
    , m_vkbMargin(0.0)
    , m_footerMargin(0.0)
    , m_acceptTouchEvents(true)
{
    // Use Gecko's remote browser backend.  The initial tab must exist before
    // QuickMozView replays a URL assigned during QML component construction.
    setProperty("_qmozChromeHosted", true);
    setProperty("_qmozChromeInitialUrl", QStringLiteral("about:blank"));

    liveViews().insert(this);

    addMessageListener(CONTENT_ORIENTATION_CHANGED);

    connect(this, &QuickMozView::recvAsyncMessage, this, &RawWebView::onAsyncMessage);
}

RawWebView::~RawWebView()
{
    liveViews().remove(this);
}

bool RawWebView::hasLiveViews()
{
    return !liveViews().isEmpty();
}

void RawWebView::destroyLiveViews()
{
    const QSet<RawWebView *> views = liveViews();
    for (RawWebView *view : views) {
        if (view) {
            view->deleteLater();
        }
    }
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

        setDynamicToolbarHeight(m_vkbMargin != 0 ? m_footerMargin : 0);

        emit virtualKeyboardMarginChanged();
    }
}

qreal RawWebView::footerMargin() const
{
    return m_footerMargin;
}

void RawWebView::setFooterMargin(qreal margin)
{
    if (m_footerMargin != margin) {
        m_footerMargin = margin;

        setDynamicToolbarHeight(m_vkbMargin != 0 ? m_footerMargin : 0);

        emit footerMarginChanged();
    }
}

int RawWebView::safeAreaTop() const
{
    return m_safeAreaInsets.top();
}

void RawWebView::setSafeAreaTop(int top)
{
    applySafeAreaInsets(QMargins(m_safeAreaInsets.left(), qMax(0, top),
                                 m_safeAreaInsets.right(), m_safeAreaInsets.bottom()));
}

int RawWebView::safeAreaRight() const
{
    return m_safeAreaInsets.right();
}

void RawWebView::setSafeAreaRight(int right)
{
    applySafeAreaInsets(QMargins(m_safeAreaInsets.left(), m_safeAreaInsets.top(),
                                 qMax(0, right), m_safeAreaInsets.bottom()));
}

int RawWebView::safeAreaBottom() const
{
    return m_safeAreaInsets.bottom();
}

void RawWebView::setSafeAreaBottom(int bottom)
{
    applySafeAreaInsets(QMargins(m_safeAreaInsets.left(), m_safeAreaInsets.top(),
                                 m_safeAreaInsets.right(), qMax(0, bottom)));
}

int RawWebView::safeAreaLeft() const
{
    return m_safeAreaInsets.left();
}

void RawWebView::setSafeAreaLeft(int left)
{
    applySafeAreaInsets(QMargins(qMax(0, left), m_safeAreaInsets.top(),
                                 m_safeAreaInsets.right(), m_safeAreaInsets.bottom()));
}

void RawWebView::applySafeAreaInsets(const QMargins &insets)
{
    if (m_safeAreaInsets != insets) {
        m_safeAreaInsets = insets;
        setSafeAreaInsets(insets);
        emit safeAreaChanged();
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

                    switch (orientation()) {
                    case Qt::PortraitOrientation:
                    case Qt::InvertedPortraitOrientation:
                        // Item coordinates already match the presented orientation.
                        break;
                    case Qt::LandscapeOrientation:
                        delta = QPointF(delta.y(), -delta.x());
                        break;
                    case Qt::InvertedLandscapeOrientation:
                        delta = QPointF(-delta.y(), delta.x());
                        break;
                    default:
                        break;
                    }

                    const bool unavailableMetrics = scrollableSize().isEmpty();
                    const qreal pageGestureMargin = qMax<qreal>(dragThreshold * 2,
                                                                width() / 4.0);
                    const qreal startX = mapFromScene(m_startPos).x();
                    const bool pageGestureFromLeftEdge = startX <= pageGestureMargin;
                    const bool pageGestureFromRightEdge = startX >= width() - pageGestureMargin;

                    if ((delta.y() >= dragThreshold
                                    && (!atYBeginning()
                                        || (unavailableMetrics
                                            && scrollableOffset().y() > 0.0)))
                            || (delta.y() <= -dragThreshold
                                    && (!atYEnd() || unavailableMetrics))
                            || (delta.x() >= dragThreshold
                                    && !pageGestureFromLeftEdge
                                    && (!atXBeginning()
                                        || (unavailableMetrics
                                            && scrollableOffset().x() > 0.0)))
                            || (delta.x() <= -dragThreshold
                                    && !pageGestureFromRightEdge
                                    && (!atXEnd() || unavailableMetrics))) {
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
        const QString orientationName = data.toMap().value("orientation").toString();
        Qt::ScreenOrientation mappedOrientation;
        if (orientationName == QStringLiteral("portrait-primary")) {
            mappedOrientation = Qt::PortraitOrientation;
        } else if (orientationName == QStringLiteral("landscape-primary")) {
            mappedOrientation = Qt::LandscapeOrientation;
        } else if (orientationName == QStringLiteral("landscape-secondary")) {
            mappedOrientation = Qt::InvertedLandscapeOrientation;
        } else if (orientationName == QStringLiteral("portrait-secondary")) {
            mappedOrientation = Qt::InvertedPortraitOrientation;
        } else {
            qWarning() << "Ignoring unknown WebView content orientation:" << orientationName;
            return;
        }
        emit contentOrientationChanged(mappedOrientation);
        // Force a fresh scene-graph update so the reoriented WebRender frame
        // is presented without waiting for additional user interaction.
        update();
    }
}

} // namespace WebView

} // namespace SailfishOS
