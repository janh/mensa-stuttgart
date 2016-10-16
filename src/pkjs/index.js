/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var storage = require('./storage');
var sender = require('./sender');


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


storage.init(errorHandler, menuHandler, fastSellersHandler);
