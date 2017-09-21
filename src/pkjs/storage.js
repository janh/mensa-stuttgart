/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var reader = require('./reader');
var time = require('./time');


var DATA_FORMAT_VERSION = 6;


var cbError, cbMenu, cbFastSeller;

var menuReady = false;
var fastSellersReady = false;

var data = null;

var location = 2;


function init(callbackError, callbackMenu, callbackFastSeller) {
    cbError = callbackError;
    cbMenu = callbackMenu;
    cbFastSeller = callbackFastSeller;

    load();

    if (data != null && data.length > 0) {
        var day = new time.Day();
        if (day.getUTCTimestamp() == data[0].date || !day.isWeekday()) {
            menuReady = true;
            cbMenu();
        }

        fastSellersReady = true;
        cbFastSeller();
    }

    reader.updateMenuData(data, location, readerCallback);
}

function readerCallback(day, success, data) {
    if (success) {
        setData(data);

        var today = new time.Day();
        if (today.getUTCTimestamp() == day.getUTCTimestamp()) {
            menuReady = true;
            cbMenu();
        }

        fastSellersReady = true;
        cbFastSeller();
    } else {
        if (!menuReady) {
            cbError();
        }
    }
}

function currentMenu() {
    if (!menuReady) {
        return null;
    }
    if (data.length > 0) {
        var day = new time.Day();
        var timestamp = day.getUTCTimestamp();
        if (timestamp == data[0].date) {
            return { 'menu': data[0].menu, 'message': data[0].message };
        }
    }
    return { 'menu': [], 'message': '' };
}

function fastSellers() {
    if (!fastSellersReady) {
        return null;
    }
    var list = [];
    for (var i = 0; i < data.length; i++) {
        var day = data[i];
        var menu = day.menu;
        loop_menu:
        for (var j = 0; j < menu.length; j++) {
            var category = menu[j];
            var meals = category.meals;
            for (var k = 0; k < meals.length; k++) {
                var meal = meals[k];
                if (meal.fastSeller == true) {
                    list.push({ 'date': day.date, 'title': meal.title });
                    break loop_menu;
                }
            }
        }
    }
    return list;
}


function setLocation(newLocation) {
    if (newLocation != location) {
        location = newLocation

        data = [];
        menuReady = false;
        fastSellersReady = false;

        store();

        reader.updateMenuData(data, location, readerCallback);
    }
}

function setData(newData) {
    data = newData;
    filterData();
    store();
}


function filterData() {
    var day = new time.Day();
    var cutoff = day.getUTCTimestamp();
    while (data.length > 0 && data[0].date < cutoff) {
        data.shift();
    }
}

function load() {
    var version = localStorage.getItem('version');
    version = parseInt(version);

    if (version >= 6 && version <= DATA_FORMAT_VERSION) {
        try {
            var storedLocation = localStorage.getItem('location');
            location = parseInt(storedLocation);
        } catch (err) {
            console.log('Failed to load location: ' + err);
        }
    }

    if (version === DATA_FORMAT_VERSION) {
        try {
            var timestamp = localStorage.getItem('timestamp');
            if (parseFloat(timestamp) <= time.now()) {
                var storedData = localStorage.getItem('data');
                data = JSON.parse(storedData);
                filterData();
            }
        } catch (err) {
            console.log('Failed to load data: ' + err);
        }
    }
}

function store() {
    localStorage.setItem('version', DATA_FORMAT_VERSION);
    localStorage.setItem('location', location);
    localStorage.setItem('timestamp', time.now());
    localStorage.setItem('data', JSON.stringify(data));
}


module.exports.init = init;
module.exports.currentMenu = currentMenu;
module.exports.fastSellers = fastSellers;
module.exports.setLocation = setLocation;
