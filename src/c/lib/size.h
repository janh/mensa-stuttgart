/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once


#include <inttypes.h>


extern const int16_t SIZE_MENU_MARGIN_LEFT[3];
extern const int16_t SIZE_MENU_MARGIN_RIGHT[3];

extern const char *SIZE_CATEGORY_FONT[3];
extern const int16_t SIZE_CATEGORY_OFFSET[3];
extern const int16_t SIZE_CATEGORY_HEIGHT[3];

extern const char *SIZE_MEAL_FONT[3];
extern const int16_t SIZE_MEAL_MAX_HEIGHT[3];

extern const char *SIZE_PRICE_FONT[3];
extern const int16_t SIZE_PRICE_HEIGHT[3];
extern const int16_t SIZE_PRICE_OFFSET[3];

extern const char *SIZE_UNAVAILABLE_FONT[3];
extern const int16_t SIZE_UNAVAILABLE_WIDTH[3];

extern const char *SIZE_ERROR_TITLE_FONT[3];
extern const char *SIZE_ERROR_DESC_FONT[3];
extern const int16_t SIZE_ERROR_HEIGHT[3];
extern const int16_t SIZE_ERROR_TITLE_MAX_HEIGHT[3];
extern const int16_t SIZE_ERROR_MARGIN_HORIZ[3];
extern const int16_t SIZE_ERROR_MARGIN_VERT[3];

extern const int16_t SIZE_PROGRESS_WIDTH[3];
extern const int16_t SIZE_PROGRESS_HEIGHT[3];
extern const int16_t SIZE_PROGRESS_RADIUS[3];

extern const int16_t SIZE_UTIL_ANIMATION_OFFSET[3];


int size_get_index();
