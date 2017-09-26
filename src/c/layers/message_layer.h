/*
 * Copyright (c) 2017 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <pebble.h>

typedef Layer MessageLayer;

MessageLayer* message_layer_create(GRect frame);
void message_layer_destroy(MessageLayer* message_layer);
void message_layer_set_content(MessageLayer* message_layer, char *text_title, char *text_desc);
