/****************************************************************************
**
** Copyright (c) 2016 - 2020 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBVIEW_H
#define SAILFISHOS_WEBVIEW_H

#include <QtCore/QMargins>
#include <QtQuick/QQuickItem>

//mozembedlite-qt5
#include <quickmozview.h>
#include <memory>

namespace SailfishOS {

namespace WebView {

class ViewCreator;

class RawWebView : public QuickMozView
{
    Q_OBJECT
    Q_PROPERTY(qreal virtualKeyboardMargin READ virtualKeyboardMargin WRITE setVirtualKeyboardMargin NOTIFY virtualKeyboardMarginChanged)
    Q_PROPERTY(qreal footerMargin READ footerMargin WRITE setFooterMargin NOTIFY footerMarginChanged)
    Q_PROPERTY(int safeAreaTop READ safeAreaTop WRITE setSafeAreaTop NOTIFY safeAreaChanged)
    Q_PROPERTY(int safeAreaRight READ safeAreaRight WRITE setSafeAreaRight NOTIFY safeAreaChanged)
    Q_PROPERTY(int safeAreaBottom READ safeAreaBottom WRITE setSafeAreaBottom NOTIFY safeAreaChanged)
    Q_PROPERTY(int safeAreaLeft READ safeAreaLeft WRITE setSafeAreaLeft NOTIFY safeAreaChanged)
    Q_PROPERTY(bool _acceptTouchEvents READ acceptTouchEvents WRITE setAcceptTouchEvents NOTIFY acceptTouchEventsChanged)

public:
    RawWebView(QQuickItem *parent = 0);
    ~RawWebView();

    qreal virtualKeyboardMargin() const;
    void setVirtualKeyboardMargin(qreal vkbMargin);

    qreal footerMargin() const;
    void setFooterMargin(qreal margin);

    int safeAreaTop() const;
    void setSafeAreaTop(int top);

    int safeAreaRight() const;
    void setSafeAreaRight(int right);

    int safeAreaBottom() const;
    void setSafeAreaBottom(int bottom);

    int safeAreaLeft() const;
    void setSafeAreaLeft(int left);

    bool acceptTouchEvents() const;
    void setAcceptTouchEvents(bool accept);

protected:
    void touchEvent(QTouchEvent *event);

signals:
    void virtualKeyboardMarginChanged();
    void footerMarginChanged();
    void safeAreaChanged();
    void contentOrientationChanged(Qt::ScreenOrientation orientation);
    void acceptTouchEventsChanged();
    void openUrlInNewWindow();

private:
    void applySafeAreaInsets(const QMargins &insets);
    void onAsyncMessage(const QString &message, const QVariant &data);

    std::shared_ptr<ViewCreator> m_viewCreator;
    qreal m_vkbMargin;
    qreal m_footerMargin;
    QMargins m_safeAreaInsets;
    QPointF m_startPos;
    bool m_acceptTouchEvents;
};

} // namespace WebView

} // namespace SailfishOS

#endif // SAILFISHOS_WEBVIEW_H
