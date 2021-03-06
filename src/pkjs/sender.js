/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

var storage = require('./storage');
var keys = require('message_keys');


var MESSAGE_TYPE_ERROR = 0;
var MESSAGE_TYPE_MENU = 1;
var MESSAGE_TYPE_FAST_SELLERS = 2;
var MESSAGE_TYPE_RESET = 3;


var messageQueue = [];


function sendError() {
    var dict = {};
    dict[keys.MessageType] = MESSAGE_TYPE_ERROR;

    queueMessage(dict);
}

function sendMenu() {
    var dict = {};
    dict[keys.MessageType] = MESSAGE_TYPE_MENU;

    var data = storage.currentMenu();
    var menu = data.menu;
    var message = data.message;

    dict[keys.MenuMessage] = message;

    dict[keys.MenuCategoriesCount] = menu.length;

    var mealCount = 0;
    for (var i = 0; i < menu.length; i++) {
        var category = menu[i];
        var meals = category.meals;
        dict[keys.MenuCategoriesTitleStart+i] = category.title;
        dict[keys.MenuCategoriesMealCountStart+i] = meals.length;
        dict[keys.MenuCategoriesMealIndexStart+i] = mealCount;

        for (var j = 0; j < meals.length; j++) {
            var meal = meals[j];
            dict[keys.MenuMealsTitleStart+mealCount] = meal.title;
            dict[keys.MenuMealsPriceStudentStart+mealCount] = meal.priceStudent;
            dict[keys.MenuMealsPriceGuestStart+mealCount] = meal.priceGuest;
            mealCount++;
        }
    }
    dict[keys.MenuMealsCount] = mealCount;

    queueMessage(dict);
}

function sendFastSellers() {
    var dict = {};
    dict[keys.MessageType] = MESSAGE_TYPE_FAST_SELLERS;

    var fastSellers = storage.fastSellers();

    var currentIndex = -1;
    var fastSeller = null;
    var lastFastSeller = null;

    for (var i = 0; i < fastSellers.length; i++) {
        lastFastSeller = fastSeller;
        fastSeller = fastSellers[i];

        if (lastFastSeller == null || fastSeller.title != lastFastSeller.title) {
            currentIndex++;
            dict[keys.FastSellersTitleStart+currentIndex] = fastSeller.title;
        }
        dict[keys.FastSellersDateStart+currentIndex] = fastSeller.date + 86400;
    }

    dict[keys.FastSellersCount] = currentIndex + 1;

    queueMessage(dict);
}

function sendReset() {
    var dict = {};
    dict[keys.MessageType] = MESSAGE_TYPE_RESET;

    queueMessage(dict);
}


function queueMessage(dict) {
    messageQueue.push(dict);
    if (messageQueue.length == 1) {
        handleMessageQueue();
    }
}

function handleMessageQueue() {
    if (messageQueue.length > 0) {
        var dict = messageQueue[0];
        sendMessage(dict, 0);
    }
}

function sendMessage(dict, count) {
    Pebble.sendAppMessage(dict, function() {
        messageQueue.shift();
        handleMessageQueue();
    }, function(e) {
        if (count < 1) {
            sendMessage(dict, count++);
        } else {
            console.log('Sending message failed: ' + JSON.stringify(e));
            messageQueue.shift();
            handleMessageQueue();
        }
    });
}


module.exports.sendError = sendError;
module.exports.sendMenu = sendMenu;
module.exports.sendFastSellers = sendFastSellers;
module.exports.sendReset = sendReset;
