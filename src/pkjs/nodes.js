/*
 * Copyright (c) 2016-2017 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


function Element(name, attributes) {
    this._name = name;
    this._attributes = attributes;
    this._classes = null;
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

    classes: function() {
        if (this._classes == null) {
            this._classes = [];
            var classString = this._attributes['class'];
            if (classString !== undefined) {
                classString = classString.replace(/^\s+|\s+$/g, '');
                if (classString.length > 0) {
                    this._classes = classString.split(/\s+/g);
                }
            }
        }
        return this._classes;
    },

    children: function() {
        return this._children;
    },

    findElementsByFilter: function(filter) {
        var elements = [];
        if (filter(this)) {
            elements.push(this);
        }
        for (var i = 0; i < this._children.length; i++)  {
            var child = this._children[i];
            if (child instanceof Element) {
                elements = elements.concat(child.findElementsByFilter(filter));
            }
        }
        return elements;
    },

    findElementsByName: function(name) {
        var filter = function(element) {
            return (element.name() == name);
        };
        return this.findElementsByFilter(filter);
    },

    findElementsByNameAndTag: function(name, tag) {
        var filter = function(element) {
            return (element.name() == name && element.classes().indexOf(tag) != -1);
        };
        return this.findElementsByFilter(filter);
    },

    content: function() {
        var content = "";
        for (var i = 0; i < this._children.length; i++)  {
            content += this._children[i].content();
        }
        return content;
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
