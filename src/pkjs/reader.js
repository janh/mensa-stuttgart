/*
 * Copyright (c) 2016-2017 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var parser = require('./parser');
var time = require('./time');


var request = null;


function updateMenuData(data, location, callback) {
    if (data == null) {
        data = [];
    }

    var day = new time.Day();
    var dayCount = 0;

    var menu = getMenu(data, day);
    if (day.isWeekday() && getAge(menu) > 300) {
        loadDay(data, location, day, dayCount, callback);
    } else {
        loadNextDay(data, location, day, dayCount, callback);
    }
}

function getMenu(data, day) {
    var timestamp = day.getUTCTimestamp();

    for (var i = 0; i < data.length; i++) {
        if (data[i].date == timestamp) {
            var menu = data[i];
            return menu;
        }
    }

    return null;
}

function getAge(menu) {
    if (menu != null) {
        return time.now() - menu.loadtime;
    }
    return 365 * 86400;
}

function loadDay(data, location, day, dayCount, callback) {
    if (request != null) {
        request.abort();
    }

    request = new XMLHttpRequest();

    request.onload = function() {
        if (request.status != 200) {
            this.onerror();
            return;
        }
        request = null;
        parseMenu(data, location, day, dayCount, this.responseText, callback);
    };

    request.onerror = function() {
        request = null;
        console.log('Error while loading data.');
        callback(day, false, data);
    }

    request.ontimeout = function() {
        request = null;
        console.log('Timeout while loading data.');
        callback(day, false, data);
    }

    var dateString = day.toString();
    var params = 'func=make_spl&locId=' + encodeURIComponent(location) + '&date=' + encodeURIComponent(dateString) + '&lang=de';

    request.timeout = 30000;
    request.open('POST', 'https://sws2.maxmanager.xyz/inc/ajax-php_konnektor.inc.php');
    request.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    request.send(params);
}

function loadNextDay(data, location, day, dayCount, callback) {
    day = day.nextDay();
    dayCount++;
    while (!day.isWeekday()) {
        day = day.nextDay();
        dayCount++;
    }

    if (dayCount < 7) {
        var menu = getMenu(data, day);
        if (menu == null || (menu.menu.length == 0 && getAge(menu) > 86400)) {
            loadDay(data, location, day, dayCount, callback);
        } else {
            loadNextDay(data, location, day, dayCount, callback);
        }
    }
}

function parseMenu(data, location, day, dayCount, text, callback) {
    try {
        var content = parser.parseString(text);
        var rows = content.findElementsByNameAndTag('div', 'row');

        var menu = [];
        var message = '';

        var category = null;

        for (var i = 0; i < rows.length; i++) {
            var row = rows[i];
            var classes = row.classes();

            if (classes.indexOf('gruppenkopf') != -1) {
                // category
                category = parseCategory(row);
                menu.push(category);
            } else if (classes.indexOf('splMeal') != -1) {
                // meal
                var meal = parseMeal(row);
                if (category != null) {
                    category.meals.push(meal);
                } else {
                    throw 'Unexpected data format: found meal without category';
                }
            } else if (classes.indexOf('nodata') != -1) {
                // message
                message = parseMessage(row);
            }
        }

        var timestamp = day.getUTCTimestamp();
        var menu = { 'date': timestamp, 'loadtime': time.now(), 'menu': menu, 'message': message };

        var replaced = false;
        for (var i = 0; i < data.length; i++) {
            if (data[i].date == timestamp) {
                data[i] = menu;
                replaced = true;
                break;
            }
        }

        if (!replaced) {
            data.push(menu);
        }
    } catch (err) {
        console.log('Could not parse item: ' + err);
        callback(day, false, data);
        return;
    }

    callback(day, true, data);

    loadNextDay(data, location, day, dayCount, callback);
}

function parseCategory(row) {
    try {
        var title = row.findElementsByNameAndTag('div', 'gruppenname')[0].content();
        var category = { 'title': title, 'meals': [] };
        return category;
    } catch (err) {
        throw 'Error parsing category: ' + err;
    }
}

function parseMeal(row) {
    try {
        var title = row.findElementsByNameAndTag('div', 'visible-xs-block')[0].content();

        var priceColumn = row.findElementsByNameAndTag('div', 'preise-xs')[0];

        var prices = priceColumn.findElementsByName('div')[0].content();
        prices = prices.replace(/[€\s]/g, '');
        prices = prices.split('/');

        var priceStudent = prices[0];
        var priceGuest = prices[1];

        var fastSellerIcon = priceColumn.findElementsByFilter(function(element) {
            if (element.name() == 'img') {
                var src = element.attributes()['src'];
                if (src !== undefined && src.substr(-5) == 'P.png') {
                    return true;
                }
            }
            return false;
        });
        var fastSeller = (fastSellerIcon.length > 0);

        return { 'title': title, 'priceStudent': priceStudent, 'priceGuest': priceGuest, 'fastSeller': fastSeller };
    } catch (err) {
        throw 'Error parsing meal: ' + err;
    }
}

function parseMessage(row) {
    try {
        var message = row.findElementsByNameAndTag('div', 'col-xs-12')[0].content();
        return message;
    } catch (err) {
        throw 'Error parsing message: ' + err;
    }
}


module.exports.updateMenuData = updateMenuData;
