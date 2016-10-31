/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var parser = require('./parser');


function readMenu(callback) {
    var request = new XMLHttpRequest();

    request.onload = function() {
        parseFeed(this.responseText, callback);
    };

    request.onerror = function() {
        console.log('Error while loading data.');
        callback(false, null);
    }

    request.open('GET', 'https://www.studierendenwerk-stuttgart.de/speiseangebot_rss');
    request.send();
}

function parseFeed(text, callback) {
    var data = [];

    var feed = parser.parseString(text);

    var items = feed.findElementsByName('item');
    for (var i = 0; i < items.length; i++) {
        try {
            var item = items[i];
            var title = items[i].findElementsByName('title')[0].content();
            var description = items[i].findElementsByName('description')[0].content();
            var date = parseDate(title);
            var menu = parseMenu(description);
            data.push({ 'date': date, 'menu': menu });
        } catch (err) {
            console.log('Could not parse item: ' + err);
            callback(false, null);
            return;
        }
    }

    callback(true, data);
}

function parseDate(text) {
    var months = {
        'Januar': 0,
        'Februar': 1,
        'März': 2,
        'April': 3,
        'Mai': 4,
        'Juni': 5,
        'Juli': 6,
        'August': 7,
        'September': 8,
        'Oktober': 9,
        'November': 10,
        'Dezember': 11,
    };

    var result = /([0-9]{1,2})\.\s+([^\s]+)\s+([0-9]{4})/.exec(text);
    if (result == null) {
        throw 'Date not found.';
    }

    var day = parseInt(result[1]);
    var month = (result[2] in months) ? months[result[2]] : 0;
    var year = parseInt(result[3]);

    // set hour to -1 to compensate for timezone (standard time only, one hour off for daylight saving)
    var date = Date.UTC(year, month, day, -1) / 1000;
    return date;
}

function parseMenu(text) {
    var menu = [];
    var category = null;

    var table = parser.parseString(text);
    try {
        var tbody = table.findElementsByName('tbody')[0];
        var rows = tbody.findElementsByName('tr');

        for (var i = 0; i < rows.length; i++) {
            var columns = rows[i].findElementsByName('td');

            if (columns.length == 1) {
                // category
                var title = columns[0].content();
                category = { 'title': title, 'meals': [] };
                menu.push(category);
            } else if (columns.length == 4) {
                // meal
                var title = columns[0].content();
                var priceStudent = columns[1].content();
                var priceGuest = columns[2].content();
                category.meals.push({ 'title': title, 'priceStudent': priceStudent, 'priceGuest': priceGuest });
            } else {
                throw 'Unexpected table format: row with ' + columns.length + ' columns';
            }
        }
    } catch (err) {
        throw 'Error parsing meal table: ' + err;
    }

    return menu;
}


module.exports.readMenu = readMenu;
