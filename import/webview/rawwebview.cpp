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
//    , m_flickable(0)
    , m_topMargin(0)
    , m_bottomMargin(0)
{
    addMessageListener(CONTENT_ORIENTATION_CHANGED);

    connect(qGuiApp->inputMethod(), &QInputMethod::visibleChanged, this, [=]() {
        if (qGuiApp->inputMethod()->isVisible()) {
            setFollowItemGeometry(false);
        }
    });

    connect(this, &QuickMozView::recvAsyncMessage, this, &RawWebView::onAsyncMessage);

//    setFiltersChildMouseEvents(true);
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
        updateMargins();
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

//QQuickItem *RawWebView::flickable() const
//{
//    return m_flickable;
//}

//void RawWebView::setFlickable(QQuickItem *flickable)
//{
//    // TODO: currently unneeded. We could replace some QML bindings with C++ handlers.
//    if (m_flickable != flickable) {
//        m_flickable = flickable;
//        if (m_flickable) {
//        }
//        emit flickableChanged();
//    }
//}

qreal RawWebView::topMargin() const
{
    return m_topMargin;
}

void RawWebView::setTopMargin(qreal margin)
{
    if (margin != m_topMargin) {
        m_topMargin = margin;
        updateMargins();
        emit topMarginChanged();
    }
}

qreal RawWebView::bottomMargin() const
{
    return m_bottomMargin;
}

void RawWebView::setBottomMargin(qreal margin)
{
    if (margin != m_bottomMargin) {
        m_bottomMargin = margin;
        updateMargins();
        emit bottomMarginChanged();
    }
}

void RawWebView::updateMargins()
{
    setMargins(QMargins(0, m_topMargin, 0, m_bottomMargin+m_vkbMargin));
}

//int RawWebView::findTouch(int id) const
//{
//    auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(), [id](const QTouchEvent::TouchPoint& tp) { return tp.id() == id; });
//    return it != m_touchPoints.end() ? it - m_touchPoints.begin() : -1;
//}

//void RawWebView::clearTouch()
//{
//    setKeepMouseGrab(false);
//    setKeepTouchGrab(false);
//    m_touchPoints.clear();
//}

//void RawWebView::touchEvent(QTouchEvent *event)
//{
//    handleTouchEvent(event);
//    event->setAccepted(true);
//}

//void RawWebView::handleTouchEvent(QTouchEvent *event)
//{
//    if (event->type() == QEvent::TouchCancel) {
//        QuickMozView::touchEvent(event);
//        clearTouch();
//        return;
//    }

//    QQuickWindow *win = window();
//    QQuickItem *grabber = win ? win->mouseGrabberItem() : 0;

//    if (grabber && grabber != this && grabber->keepMouseGrab()) {
//        if (!m_touchPoints.isEmpty()) {
//            QTouchEvent localEvent(QEvent::TouchCancel);
//            localEvent.setTouchPoints(m_touchPoints);
//            QuickMozView::touchEvent(&localEvent);
//            clearTouch();
//        }
//        return;
//    }

//    Qt::TouchPointStates touchStates = 0;
//    QList<int> removedTouches;

//    const QList<QTouchEvent::TouchPoint> &touchPoints = event->touchPoints();
//    foreach (QTouchEvent::TouchPoint touchPoint, touchPoints) {

//        int touchIdx = findTouch(touchPoint.id());
//        if (touchIdx >= 0)
//            m_touchPoints[touchIdx] = touchPoint;

//        switch (touchPoint.state()) {
//        case Qt::TouchPointPressed:
//            if (touchIdx >= 0)
//                continue;
//            touchStates |= Qt::TouchPointPressed;
//            m_touchPoints.append(touchPoint);
//            if (m_touchPoints.count() > 1) {
//                setKeepMouseGrab(true);
//                setKeepTouchGrab(true);
//                grabMouse();
//                grabTouchPoints(QVector<int>() << touchPoint.id());
//            } else {
//                m_startPos = touchPoint.scenePos();
//                setKeepMouseGrab(false);
//                setKeepTouchGrab(false);
//            }
//            break;
//        // fall through
//        case Qt::TouchPointMoved: {
//            if (touchIdx < 0)
//                continue;
//            if (QQuickWindowPrivate::get(win)->touchMouseId == touchPoint.id()) {
//                const int dragThreshold = QGuiApplication::styleHints()->startDragDistance();
//                QPointF delta = touchPoint.scenePos() - m_startPos;
//                if (!keepMouseGrab()) {
//                    if ((delta.y() >= dragThreshold && !atYBeginning()) || (delta.y() <= -dragThreshold && !atYEnd())
//                            || (delta.x() >= dragThreshold && !atXBeginning()) || (delta.x() <= -dragThreshold && !atXEnd())) {
//                        setKeepMouseGrab(true);
//                        setKeepTouchGrab(true);
//                        grabMouse();
//                        grabTouchPoints(QVector<int>() << touchPoint.id());
//                    }
//                    // Do not pass this event through
//                    return;
//                } else if (grabber && grabber != this && grabber->keepMouseGrab()) {
//                    m_touchPoints.removeAt(touchIdx);
//                    event->ignore();
//                    return;
//                }
//            }
//            touchStates |= Qt::TouchPointMoved;
//            break;
//        }
//        case Qt::TouchPointReleased: {
//            if (touchIdx < 0)
//                continue;
//            touchStates |= Qt::TouchPointReleased;
//            removedTouches << touchPoint.id();
//            break;
//        }
//        default:
//            break;
//        }
//    }

//    if (m_touchPoints.isEmpty())
//        return;

//    QTouchEvent localEvent(*event);
//    localEvent.setTouchPoints(m_touchPoints);
//    localEvent.setTouchPointStates(touchStates);
//    QuickMozView::touchEvent(&localEvent);
//    event->setAccepted(localEvent.isAccepted());

//    foreach (int id, removedTouches) {
//        int touchIdx = findTouch(id);
//        if (touchIdx >= 0) {
//            m_touchPoints.removeAt(touchIdx);
//        }
//    }

//    if (m_touchPoints.isEmpty())
//        clearTouch();
//}

//bool RawWebView::childMouseEventFilter(QQuickItem *i, QEvent *e)
//{
//    if (!isVisible())
//        return QQuickItem::childMouseEventFilter(i, e);
//    switch (e->type()) {
//    case QEvent::TouchBegin:
//    case QEvent::TouchUpdate:
//        handleTouchEvent(static_cast<QTouchEvent*>(e));
//        e->setAccepted(keepMouseGrab());
//        return keepMouseGrab();
//    case QEvent::TouchEnd:
//        handleTouchEvent(static_cast<QTouchEvent*>(e));
//        break;
//    default:
//        break;
//    }

//    return QQuickItem::childMouseEventFilter(i, e);
//}

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

