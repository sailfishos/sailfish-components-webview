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

#include <qmozviewcreator.h>

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QStyleHints>
#include <QtGui/QMouseEvent>
#include <QtQuick/QQuickWindow>
#include <private/qquickwindow_p.h>

#include <algorithm>

#define CONTENT_ORIENTATION_CHANGED QLatin1String("embed:contentOrientationChanged")

namespace SailfishOS {

namespace WebView {

class ViewCreator : public QMozViewCreator
{
public:
    ViewCreator();
    ~ViewCreator();

    quint32 createView(const quint32 &parentId, const uintptr_t &parentBrowsingContext, const bool hidden = false) override;

    static std::shared_ptr<ViewCreator> instance();

    std::vector<RawWebView *> views;
};

ViewCreator::ViewCreator()
{
    SailfishOS::WebEngine::instance()->setViewCreator(this);
}

ViewCreator::~ViewCreator()
{
    SailfishOS::WebEngine::instance()->setViewCreator(nullptr);
}

quint32 ViewCreator::createView(const quint32 &parentId, const uintptr_t &parentBrowsingContext, const bool hidden)
{
    Q_UNUSED(parentBrowsingContext)
    Q_UNUSED(hidden)

    for (RawWebView *view : views) {
        if (view->uniqueId() == parentId) {
            view->openUrlInNewWindow();
            break;
        }
    }

    return 0;
}

std::shared_ptr<ViewCreator> ViewCreator::instance()
{
    static std::weak_ptr<ViewCreator> instance;

    std::shared_ptr<ViewCreator> creator = instance.lock();
    if (!creator) {
        creator = std::make_shared<ViewCreator>();
        instance = creator;
    }

    return creator;
}


RawWebView::RawWebView(QQuickItem *parent)
    : QuickMozView(parent)
    , m_viewCreator(ViewCreator::instance())
    , m_vkbMargin(0.0)
    , m_footerMargin(0.0)
    , m_acceptTouchEvents(true)
{
    m_viewCreator->views.push_back(this);

    addMessageListener(CONTENT_ORIENTATION_CHANGED);

    connect(this, &QuickMozView::recvAsyncMessage, this, &RawWebView::onAsyncMessage);
}

RawWebView::~RawWebView()
{
    m_viewCreator->views.erase(std::find(m_viewCreator->views.begin(), m_viewCreator->views.end(), this));
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

