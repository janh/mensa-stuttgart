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


const int16_t SIZE_MENU_MARGIN_LEFT[3] = {PBL_IF_RECT_ELSE(6, 21),
                                          IF_LARGE_ELSE(PBL_IF_RECT_ELSE(8, 22), PBL_IF_RECT_ELSE(4, 20)),
                                          8};
const int16_t SIZE_MENU_MARGIN_RIGHT[3] = {PBL_IF_RECT_ELSE(3, 21),
                                           IF_LARGE_ELSE(PBL_IF_RECT_ELSE(4, 22), PBL_IF_RECT_ELSE(2, 20)),
                                           4};

const char *SIZE_CATEGORY_FONT[3] = {FONT_KEY_GOTHIC_14_BOLD, FONT_KEY_GOTHIC_18_BOLD, FONT_KEY_GOTHIC_24_BOLD};
const int16_t SIZE_CATEGORY_OFFSET[3] = {PBL_IF_RECT_ELSE(2, 7), PBL_IF_RECT_ELSE(2, 7), 2};
const int16_t SIZE_CATEGORY_HEIGHT[3] = {PBL_IF_RECT_ELSE(20, 25), PBL_IF_RECT_ELSE(26, 31), 32};

const char *SIZE_MEAL_FONT[3] = {FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24, FONT_KEY_GOTHIC_28};
const int16_t SIZE_MEAL_MAX_HEIGHT[3] = {72, 72, 84};

const char *SIZE_PRICE_FONT[3] = {FONT_KEY_GOTHIC_14, FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24};
const int16_t SIZE_PRICE_HEIGHT[3] = {25, 32, 38};
const int16_t SIZE_PRICE_OFFSET[3] = {21, 27, 34};

const char *SIZE_MESSAGE_TITLE_FONT[3] = {FONT_KEY_GOTHIC_24_BOLD, FONT_KEY_GOTHIC_28_BOLD, FONT_KEY_GOTHIC_28_BOLD};
const char *SIZE_MESSAGE_DESC_FONT[3] = {FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24, FONT_KEY_GOTHIC_24};
const int16_t SIZE_MESSAGE_WIDTH[3] = {100, 120, 120};
const int16_t SIZE_MESSAGE_HEIGHT[3] = {140, IF_LARGE_ELSE(168, 140), 168};
const int16_t SIZE_MESSAGE_MARGIN[3] = {10, 12, 12};

const char *SIZE_ERROR_TITLE_FONT[3] = {FONT_KEY_GOTHIC_24_BOLD, FONT_KEY_GOTHIC_28_BOLD, FONT_KEY_GOTHIC_28_BOLD};
const char *SIZE_ERROR_DESC_FONT[3] = {FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24, FONT_KEY_GOTHIC_24};
const int16_t SIZE_ERROR_HEIGHT[3] = {120, 144, 144};
const int16_t SIZE_ERROR_TITLE_MAX_HEIGHT[3] = {60, 72, 72};
const int16_t SIZE_ERROR_MARGIN_HORIZ[3] = {15, IF_LARGE_ELSE(18, 10), 18};
const int16_t SIZE_ERROR_MARGIN_VERT[3] = {10, 12, 12};

const int16_t SIZE_PROGRESS_WIDTH[3] = {80, IF_LARGE_ELSE(96, 80), 96};
const int16_t SIZE_PROGRESS_HEIGHT[3] = {6, IF_LARGE_ELSE(8, 6), 8};
const int16_t SIZE_PROGRESS_RADIUS[3] = {2, IF_LARGE_ELSE(3, 2), 3};

const int16_t SIZE_UTIL_ANIMATION_OFFSET[3] = {20, IF_LARGE_ELSE(24, 20), 28};


int size_get_index() {
  PreferredContentSize preferred_size = preferred_content_size();
  if (preferred_size <= PreferredContentSizeMedium) {
    return 0;
  } else if (preferred_size >= PreferredContentSizeExtraLarge) {
    return 2;
  } else {
    return 1;
  }
}
