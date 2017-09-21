/*
 * Copyright (c) 2017 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var storage = require('./storage');
var sender = require('./sender');
var html = require('html');


var appReady = false;
var menuSent = false;


Pebble.addEventListener("ready",
    function(e) {
        appReady = true;
        if (storage.currentMenu() != null) {
            sender.sendMenu();
        }
        if (storage.fastSellers() != null) {
            sender.sendFastSellers();
        }
    }
);


function errorHandler() {
    if (!menuSent) {
        sender.sendError();
    }
}

function menuHandler() {
    if (appReady) {
        sender.sendMenu();
    }
}

function fastSellersHandler() {
    if (appReady) {
        sender.sendFastSellers();
    }
}

function resetHandler() {
    if (appReady) {
        sender.sendReset();
    }
}


storage.init(errorHandler, menuHandler, fastSellersHandler, resetHandler);


Pebble.addEventListener("showConfiguration",
    function() {
        var content = html.config;
        content = content.replace('{{LOCATION}}', storage.getLocation());
        var url = 'data:text/html;charset=utf-8,' + encodeURIComponent(content);
        Pebble.openURL(url);
    }
);

Pebble.addEventListener("webviewclosed",
    function(e) {
        if (e.response != undefined) {
            var data = decodeURIComponent(e.response);
            var location = parseInt(data);
            storage.setLocation(location);
        }
    }
);
