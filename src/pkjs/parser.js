/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var nodes = require('./nodes');


function parseString(text) {
    var index = 0;

    var tree = new nodes.Element('', {});
    var currentNode = tree;
    var nodeStack = [];

    while (true) {
        // find the next tag in the input
        var tagData = parseTag(text, index);
        if (tagData == null) {
            break;
        }
        index = tagData.end + 1;

        if (tagData.name == '?xml') {
            continue;
        }

        // opening tag: add new element and change node pointer
        if (tagData.opening) {
            var node = new nodes.Element(tagData.name, tagData.attributes);
            nodeStack.push(currentNode);
            currentNode.children().push(node);
            currentNode = node;
        }

        // closing tag: move node pointer to parent of closed tag
        if (tagData.closing) {
            while (currentNode.name() != tagData.name && nodeStack.length != 0) {
                currentNode = nodeStack.pop();
            }
            if (nodeStack.length != 0) {
                currentNode = nodeStack.pop();
            }
        }

        // find text before start of next tag
        var textData = parseText(text, index);
        index = textData.end + 1;

        // if text is not empty: add text node
        var content = textData.text.trim();
        if (content.length > 0) {
            var node = new nodes.Text(content);
            currentNode.children().push(node);
        }
    }

    return tree;
}

function parseText(text, start) {
    var end = text.indexOf('<', start);
    if (end == -1) {
        end = text.length + 1;
    }
    var content = text.substring(start, end);
    content = decodeEntities(content);
    return {
        'text': content,
        'end': end - 1,
    };
}

function parseTag(text, start) {
    var open = text.indexOf('<', start);
    var close = text.indexOf('>', open);

    if (open != -1 && close != -1) {
        var inner = text.substring(open + 1, close);

        var opening = true;
        var closing = false;

        if (inner.charAt(0) == '/') {
            opening = false;
            closing = true;
            inner = inner.substring(1);
        } else {
            var result = /\/\s*$/.exec(inner);
            if (result != null) {
                closing = true;
                inner = inner.substring(0, inner.length - result[0].length);
            }
        }

        var data = parseAttributes(inner);

        return {
            'name': data.name,
            'opening': opening,
            'closing': closing,
            'attributes': data.attributes,
            'start': open,
            'end': close,
        };
    } else {
        return null;
    }
}

function parseAttributes(text) {
    var result = /\s/.exec(text);
    if (result == null) {
        return { 'name': text, 'attributes': {} };
    }

    var index = result.index;

    var data = {
        'name': text.substring(0, index),
        'attributes': {},
    };

    var regexp = /([^\s=]+)(|=([^\s*?]|".*?"|'.*?'))(\s|$)/g;
    regexp.lastIndex = index;
    while (true) {
        var result = regexp.exec(text);
        if (result == null) {
            break;
        }
        var value = (result[3] != undefined) ? result[3] : '';
        if (value.charAt(0) == value.charAt(value.length - 1) && (value.charAt(0) == '"' || value.charAt(0) == "'")) {
            value = value.substring(1, value.length - 1);
        }
        data.attributes[result[1]] = value;
    }

    return data;
}

function decodeEntities(text) {
    var map = {
        'quot': '"',
        'amp': '&',
        'apos': "'",
        'lt': '<',
        'gt': '>',
    };

    text = text.replace(/&([a-z]+);/g, function(match, p1, offset, string) {
        if (p1 in map) {
            return map[p1];
        } else {
            return match;
        }
    });

    return text;
}


module.exports.parseString = parseString;
