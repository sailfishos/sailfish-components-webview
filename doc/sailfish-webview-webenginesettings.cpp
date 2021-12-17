/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sailfish-webview-webenginesettings.h"

// This file is used only for documentation purposes

// These items are all part of QMozEngineSettings, but inherited by
// WebEngineSettings, so we take some notational liberty and present them
// as part of the the subclass.

namespace SailfishOS {

/*!
    \property SailfishOS::WebEngineSettings::initialized
    \brief Whether the engine settings have been initialized.

    This property returns true when the engine settings have been initialized
    and false otherwise

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}
    \sa {SailfishOS::QMozContext::initialize}{QMozContext::initialize}
*/

/*!
    \property SailfishOS::WebEngineSettings::autoLoadImages
    \brief Whether to automatically load images.

    When set to \c true images will be automatically downloaded and rendered.
    When set to \c false images will not be downloaded.

    The default value is \c true.

    This corresponds to the "permissions.default.image" gecko preference.

    Setting this property to \c false can help avoid tracking by external
    servers when rendering local content, but may prevent a page from rendering
    correctly.
*/

/*!
    \property SailfishOS::WebEngineSettings::javascriptEnabled
    \brief Whether JavaScript on the page will be automatically executed.

    When set to \c true JavaScript on the page will be executed. When set to
    \c false JavaScript will not be executed.

    The default value is \c true.

    This corresponds to the "javascript.enabled" gecko preference.

    Disabling JavaScript may be beneficial for security, privacy or efficiency
    reasons, but may also prevent pages from rendering correctly.
*/

/*!
    \property SailfishOS::WebEngineSettings::popupEnabled
    \brief Whether to allow popups to open during page loading.

    When set to \c true any popups the page requests to open during loading will
    be allowed to open. When set to \c false requests for popups will be
    blocked from being actioned.

    The default value is \c false.

    This corresponds to the "dom.disable_open_during_load" gecko preference.
*/

/*!
    \enum SailfishOS::WebEngineSettings::CookieBehavior

    This enum type specifies the cookie behaviour for the engine to use.

    \value AcceptAll
           Accept all cookies independent of their origin.
    \value BlockThirdParty
           Accept cookies only if they are from the requested domain.
    \value BlockAll
           Do not accept any cookies.
    \omitvalue Deprecated
*/

/*!
    \enum SailfishOS::WebEngineSettings::PreferenceType

    This enum type specifies possible preference types that can used.

    \value UnknownPref
           Unknown preference type.
           WebEngineSettings interprets value on best effort basis.
    \value StringPref
           Preference interpreted as string type, commonly used with float as well.
    \value IntPref
           Preference interpreted as int type.
    \value BoolPref
           Preference interpreted as bool type (true or false).

*/

/*!
    \property SailfishOS::WebEngineSettings::cookieBehavior
    \brief Sets the cookie behaviour.

    The cookie behaviour can be one of:

    \value WebEngineSettings.AcceptAll
           Accept all cookies independent of their origin, the default
    \value WebEngineSettings.BlockThirdParty
           Accept cookies only if they are from the requested domain.
    \value WebEngineSettings.BlockAll
           Do not accept any cookies.

    This corresponds to the "network.cookie.cookieBehavior" gecko preference.

    \sa WebEngineSettings::CookieBehavior
*/

/*!
    \property SailfishOS::WebEngineSettings::useDownloadDir
    \brief Sets whether or not to use the default download location.

    When set to \c true downloaded files will be saved to the default download
    location, as specified by the \l downloadDir property. When set to \c false
    the user will be asked to specify a location for each file downloaded.

    This corresponds to the "browser.download.useDownloadDir" gecko preference.

    \sa downloadDir
*/

/*!
    \property SailfishOS::WebEngineSettings::downloadDir
    \brief Sets the location to save downloaded files to.

    Specifies an absolute path location to save downloaded files to. This
    property applies only if the \l useDownloadDir property is set to \c true.

    This corresponds to the "browser.download.downloadDir" gecko preference.

    \sa useDownloadDir
*/

/*!
    \property SailfishOS::WebEngineSettings::pixelRatio
    \brief Specifies the device to logical pixel ratio.

    This property represents the number of physical device pixels used to
    represent each logical pixel of the web rendering.

    Hence, a \c pixelRatio of 2 will cause items to be rendered on screen with
    twice the dimensions (covering four times the area) as a \c pixelRatio of 1,
    for example.

    This corresponds to the "layout.css.devPixelsPerPx" gecko preference.

    \note Unlike desktop Firefox, changing this value will only change the
    rendering of web content; it does not affect the UI scaling.

    \sa WebView::resolution
*/

/*!
    \brief Returns the instance of the singleton WebEngineSettings class.

    The returned instance may not be initialized.

    In most situations the singleton
    {SailfishOS::WebEngineSettings::instance}{WebEngineSettings::instance},
    which inherits\c WebEngineSettings, should be used instead.

    \sa initialized,
    {SailfishOS::WebEngineSettings::instance}{WebEngineSettings::instance}
    {SailfishOS::WebEngineSettings::initialize}{WebEngineSettings::initialize}
*/
WebEngineSettings *SailfishOS::WebEngineSettings::instance();


/*!
    \brief Sets the tile \a size used for rendering pages.

    Setting the tile \a size is equivalent to setting the two gecko properties
    "layers.tile-width" and "layers.tile-height".
*/
void SailfishOS::WebEngineSettings::setTileSize(const QSize &size);

/*!
    \brief Sets whether or not to use progressive rendering.

    When \a enabled the rendering engine will use progressive rendering for
    the page.

    This corresponds to the "layers.progressive-paint" gecko preference.
*/
void SailfishOS::WebEngineSettings::enableProgressivePainting(bool enabled);

/*!
    \brief Sets whether to enable low precision buffers.

    When \a enabled the rendering engine will display low resolution
    tiles to the screen while the full resolution tiles are being rendered.

    This can be used to make scrolling more responsive, but initially only
    showing a lower resolution version of the page.

    This corresponds to the "layers.low-precision-buffer" gecko preference.
*/
void SailfishOS::WebEngineSettings::enableLowPrecisionBuffers(bool enabled);

/*!
    \brief Directly set gecko engine preferences.

    Sets the \a key preference to \a value.

    This is a low-level API to set engine preferences. Higher-level calls, such
    as those provided by \l WebEngineSettings,
    \l {SailfishOS::WebEngineSettings}{WebEngineSettings},
    \l {SailfishOS::WebEngine}{WebEngine} and \l {SailfishOS::WebView}{WebView}
    should be used in preference whenever possible.

    See the \l {https://developer.mozilla.org/en-US/docs/Mozilla/Preferences?retiredLocale=ar}{MDN docs}
    for more info about the available preferences.
*/
void SailfishOS::WebEngineSettings::setPreference(const QString &key, const QVariant &value);

/*!
    \brief Directly set gecko engine preferences.

    This function overloads setPreference().

    Sets the \a key preference to \a value. The \a type is the type of the prererence value.

    \sa setPreference
*/
void SailfishOS::WebEngineSettings::setPreference(const QString &key, const QVariant &value, PreferenceType preferenceType);

} // namespace SailfishOS
