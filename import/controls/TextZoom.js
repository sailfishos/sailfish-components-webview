/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

(function() {
    "use strict";

    function applyTextZoom(message) {
        var data = message.data || message.json || {};
        var zoom = Number(data.zoom);

        if (!isFinite(zoom) || zoom <= 0) {
            zoom = 1.0;
        }

        try {
            docShell.browsingContext.textZoom = zoom;
        } catch (e) {
        }
    }

    addMessageListener("embedui:textZoom", applyTextZoom);
})();
