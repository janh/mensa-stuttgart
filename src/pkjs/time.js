/*
 * Copyright (c) 2017 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


// As Javascript has no proper timezone support, all calculations are performed using the UTC methods. However, instead
// of standard Unix timestamps, we internally use the number of seconds since 1970-01-01 00:00:00 MEZ (thus, the result
// is correct for MEZ local time, but off by 1 hour for MESZ).


function Day(timestamp) {
    if (timestamp == undefined) {
        timestamp = now();
    }

    var date = new Date((timestamp + 3600) * 1000);
    this._date = new Date(Date.UTC(date.getUTCFullYear(), date.getUTCMonth(), date.getUTCDate()));
}

Day.prototype = {

    constructor: Day,

    getMEZTimestamp: function() {
        return this._date.getTime() / 1000;
    },

    getUTCTimestamp: function() {
        return this._date.getTime() / 1000 - 3600;
    },

    toString: function() {
        var year = this._date.getUTCFullYear();
        var month = this._date.getUTCMonth() + 1;
        var day = this._date.getUTCDate();
        return year + '-' + month + '-' + day;
    },

    isWeekday: function() {
        var day = this._date.getUTCDay();
        return (day > 0 && day < 6);
    },

    nextDay: function() {
        var timestamp = this.getUTCTimestamp();
        return new Day(timestamp + 86400);
    },

}


function now() {
    return Date.now() / 1000;
}


module.exports.Day = Day;
module.exports.now = now;
