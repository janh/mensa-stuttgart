/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "size.h"

#include <pebble.h>


#ifdef PBL_PLATFORM_EMERY
  #define IF_LARGE_ELSE(a,b) a
#else
  #define IF_LARGE_ELSE(a,b) b
#endif


const int16_t SIZE_MENU_MARGIN_LEFT[2] = {PBL_IF_RECT_ELSE(6, 24),
                                          IF_LARGE_ELSE(PBL_IF_RECT_ELSE(8, 26), PBL_IF_RECT_ELSE(4, 22))};
const int16_t SIZE_MENU_MARGIN_RIGHT[2] = {PBL_IF_RECT_ELSE(3, 21),
                                           IF_LARGE_ELSE(PBL_IF_RECT_ELSE(4, 22), PBL_IF_RECT_ELSE(2, 20))};

const char *SIZE_CATEGORY_FONT[2] = {FONT_KEY_GOTHIC_14_BOLD, FONT_KEY_GOTHIC_18_BOLD};
const int16_t SIZE_CATEGORY_OFFSET[2] = {2, 2};
const int16_t SIZE_CATEGORY_HEIGHT[2] = {20, 26};

const char *SIZE_MEAL_FONT[2] = {FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24};
const int16_t SIZE_MEAL_MAX_HEIGHT[2] = {PBL_IF_RECT_ELSE(72, 54), PBL_IF_RECT_ELSE(72, 48)};

const char *SIZE_PRICE_FONT[2] = {FONT_KEY_GOTHIC_14, FONT_KEY_GOTHIC_18};
const int16_t SIZE_PRICE_HEIGHT[2] = {25, 32};
const int16_t SIZE_PRICE_OFFSET[2] = {21, 27};

const char *SIZE_UNAVAILABLE_FONT[2] = {FONT_KEY_GOTHIC_24_BOLD, FONT_KEY_GOTHIC_28_BOLD};
const int16_t SIZE_UNAVAILABLE_WIDTH[2] = {100, 120};

const char *SIZE_ERROR_TITLE_FONT[2] = {FONT_KEY_GOTHIC_24_BOLD, FONT_KEY_GOTHIC_28_BOLD};
const char *SIZE_ERROR_DESC_FONT[2] = {FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24};
const int16_t SIZE_ERROR_HEIGHT[2] = {120, 144};
const int16_t SIZE_ERROR_TITLE_MAX_HEIGHT[2] = {60, 72};
const int16_t SIZE_ERROR_MARGIN_HORIZ[2] = {15, IF_LARGE_ELSE(18, 10)};
const int16_t SIZE_ERROR_MARGIN_VERT[2] = {10, 12};

const int16_t SIZE_PROGRESS_WIDTH[2] = {80, IF_LARGE_ELSE(96, 80)};
const int16_t SIZE_PROGRESS_HEIGHT[2] = {6, IF_LARGE_ELSE(8, 6)};
const int16_t SIZE_PROGRESS_RADIUS[2] = {2, IF_LARGE_ELSE(3, 2)};

const int16_t SIZE_UTIL_ANIMATION_OFFSET[2] = {20, IF_LARGE_ELSE(24, 20)};


int size_get_index() {
  PreferredContentSize preferred_size = preferred_content_size();
  if (preferred_size <= PreferredContentSizeMedium) {
    return 0;
  } else {
    return 1;
  }
}
