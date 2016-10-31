/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var reader = require('./reader');


var DATA_FORMAT_VERSION = 2;


var cbError, cbMenu, cbFastSeller;

var menuReady = false;
var fastSellersReady = false;

var data = null;


function init(callbackError, callbackMenu, callbackFastSeller) {
    cbError = callbackError;
    cbMenu = callbackMenu;
    cbFastSeller = callbackFastSeller;

    loadData();
    if (data != null) {
        if (data.length > 0) {
            menuReady = true;
            cbMenu();
        }
        if (data.length >= 5) {
            fastSellersReady = true;
            cbFastSeller();
        }
    }
    if (!menuReady || !fastSellersReady) {
        reader.readMenu(readerCallback);
    }
}

function readerCallback(success, data) {
    if (success) {
        storeData(data);
        if (!menuReady) {
            menuReady = true;
            cbMenu();
        }
        if (!fastSellersReady) {
            fastSellersReady = true;
            cbFastSeller();
        }
    } else {
        cbError();
    }
}


function currentMenu() {
    if (!menuReady) {
        return null;
    }
    if (data.length > 0) {
        var now = Date.now() / 1000;
        if (now > data[0].date) {
            return { 'menu': data[0].menu, 'message': data[0].message };
        }
    }
    return { 'menu': [], 'message': '' };
}

function fastSellers() {
    if (!fastSellersReady) {
        return null;
    }
    var regexpFastSeller = /renner/i;
    var list = [];
    for (var i = 0; i < data.length; i++) {
        var day = data[i];
        var menu = day.menu;
        for (var j = 0; j < menu.length; j++) {
            var category = menu[j];
            if (category.title.search(regexpFastSeller) != -1) {
                if (category.meals.length > 0) {
                    list.push({ 'date': day.date, 'title': category.meals[0].title });
                }
                break;
            }
        }
    }
    return list;
}


function filterData() {
    var cutoff = Date.now() / 1000 - 86400;
    while (data.length > 0 && data[0].date < cutoff) {
        data.shift();
    }
}

function loadData() {
    var version = localStorage.getItem('version');
    if (parseInt(version) === DATA_FORMAT_VERSION) {
        var storedData = localStorage.getItem('data');
        data = JSON.parse(storedData);
        filterData();
    }
}

function storeData(newData) {
    data = newData;
    filterData();
    localStorage.setItem('version', DATA_FORMAT_VERSION);
    localStorage.setItem('data', JSON.stringify(data));
}


module.exports.init = init;
module.exports.currentMenu = currentMenu;
module.exports.fastSellers = fastSellers;
