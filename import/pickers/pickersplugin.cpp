/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Chris Adams <chris.adams@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pickersplugin.h"

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

namespace SailfishOS {

namespace WebView {

namespace Pickers {

void SailfishOSWebViewPickersPlugin::registerTypes(const char *)
{
}

void SailfishOSWebViewPickersPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView.Pickers"));

    AppTranslator *engineeringEnglish = new AppTranslator(engine);
    AppTranslator *translator = new AppTranslator(engine);
    engineeringEnglish->load("sailfish_components_webview_pickers_qt5_eng_en", "/usr/share/translations");
    translator->load(QLocale(), "sailfish_components_webview_pickers_qt5", "-", "/usr/share/translations");
}

} // namespace Pickers

} // namespace WebView

} // namespace SailfishOS

