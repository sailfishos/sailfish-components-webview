/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

#include <oauth2.h>
#include <redirectlistener.h>

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtGui/QGuiApplication>
#include <QtGui/QDesktopServices>

#include <QtDebug>

using namespace SailfishOS::OAuth;

int Q_DECL_EXPORT main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QNetworkAccessManager qnam;
    OAuth2 oauth2;
    RedirectListener listener;

    QObject::connect(&listener, &RedirectListener::failed,
                     &app, [&app, &listener]() {
        qWarning() << "Listener failed to listen!";
        app.quit();
    });

    QObject::connect(&oauth2, &OAuth2::errorChanged,
                     &app, [&app, &oauth2]() {
        qWarning() << "Error! " << oauth2.error().code() << ":" << oauth2.error().message();
        app.quit();
    });

    QObject::connect(&listener, &RedirectListener::uriChanged,
                     &app, [&app, &listener, &oauth2]() {
        qDebug() << "Listening for redirects on uri:" << listener.uri();
        oauth2.setRedirectUri(listener.uri());
        QDesktopServices::openUrl(oauth2.generateAuthorizationUrl());
    });

    QObject::connect(&listener, &RedirectListener::receivedRedirect,
                     &app, [&app, &listener, &oauth2](const QString &redirectUri) {
        const QVariantMap data = oauth2.parseRedirectUri(redirectUri);
        if (!data.value("code").toString().isEmpty()) {
            qDebug() << "Received auth code, about to request access token";
            oauth2.setCustomParameters(QVariantMap());
            oauth2.requestAccessToken(data.value("code").toString(),
                                      data.value("state").toString());
        } else {
            qWarning() << "Unable to parse authorization code from redirect: " << redirectUri;
            app.quit();
        }
    });

    QObject::connect(&oauth2, &OAuth2::receivedAccessToken,
                     &app, [&app, &listener, &oauth2, &qnam](const QVariantMap &token) {
        qDebug() << "Received access token: " << token.value("access_token").toString();
        listener.stopListening();

        QUrl url(QStringLiteral("https://www.googleapis.com/oauth2/v2/userinfo"));
        QNetworkRequest request(url);
        request.setRawHeader(QString(QLatin1String("Authorization")).toUtf8(),
                             QString(QLatin1String("Bearer ") + token.value("access_token").toString()).toUtf8());
        QNetworkReply *reply = qnam.get(request);
        if (!reply) {
            qWarning() << "Failed to perform authenticated request";
            app.quit();
            return;
        }

        qDebug() << "Performing authenticated request";
        QObject::connect(reply, &QNetworkReply::finished,
                         &app, [&app, &listener, &oauth2, reply]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError) {
                qWarning() << "Authenticated request error: " << reply->errorString()
                           << " : " << reply->readAll();
            } else {
                qDebug() << reply->readAll();
            }
            app.quit();
        });
    });

    oauth2.setClientId(QString());      // enter your app's clientId here
    oauth2.setClientSecret(QString());  // enter your app's clientSecret here
    oauth2.setTokenEndpoint("https://accounts.google.com/o/oauth2/token");
    oauth2.setAuthorizationEndpoint("https://accounts.google.com/o/oauth2/auth");
    oauth2.setScope("https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/userinfo.profile");
    oauth2.setState(oauth2.generateState());
    oauth2.setCodeVerifier(oauth2.generateCodeVerifier());

    listener.startListening();
    return app.exec();
}
