/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Martin Jones <martin.jones@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBVIEW_H
#define SAILFISHOS_WEBVIEW_H

#include <QtQuick/QQuickItem>

//mozembedlite-qt5
#include <quickmozview.h>

namespace SailfishOS {

namespace WebView {

class RawWebView : public QuickMozView
{
    Q_OBJECT
    Q_PROPERTY(qreal virtualKeyboardMargin READ virtualKeyboardMargin WRITE setVirtualKeyboardMargin NOTIFY virtualKeyboardMarginChanged)
    Q_PROPERTY(QQuickItem *flickable READ flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)

public:
    RawWebView(QQuickItem *parent = 0);
    ~RawWebView();

    qreal virtualKeyboardMargin() const;
    void setVirtualKeyboardMargin(qreal vkbMargin);

    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *flickable);

    qreal topMargin() const;
    void setTopMargin(qreal margin);

    qreal bottomMargin() const;
    void setBottomMargin(qreal margin);

protected:
    void handleTouchEvent(QTouchEvent *event);

    void touchEvent(QTouchEvent *event);
    bool childMouseEventFilter(QQuickItem *i, QEvent *e);

signals:
    void virtualKeyboardMarginChanged();
    void contentOrientationChanged(Qt::ScreenOrientation orientation);
    void flickableChanged();
    void topMarginChanged();
    void bottomMarginChanged();

private slots:
    void onAsyncMessage(const QString &message, const QVariant &data);

private:
    void updateMargins();
    int findTouch(int id) const;
    void clearTouch();

    qreal m_vkbMargin;
    QQuickItem *m_flickable;
    QPointF m_startPos;
    qreal m_topMargin;
    qreal m_bottomMargin;
    QList<QTouchEvent::TouchPoint> m_touchPoints;
};

} // namespace WebView

} // namespace SailfishOS

#endif // SAILFISHOS_WEBVIEW_H
