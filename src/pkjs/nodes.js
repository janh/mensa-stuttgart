/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


function Element(name, attributes) {
    this._name = name;
    this._attributes = attributes;
    this._children = [];
}

Element.prototype = {

    constructor: Element,

    name: function() {
        return this._name;
    },

    attributes: function() {
        return this._attributes;
    },

    children: function() {
        return this._children;
    },

}


function Text(content) {
    this._content = content;
}

Text.prototype = {

    constructor: Text,

    content: function() {
        return this._content;
    },

}


module.exports.Element = Element;
module.exports.Text = Text;
