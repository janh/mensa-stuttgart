/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <stdbool.h>
#include <inttypes.h>


#define MESSAGE_TYPE_ERROR 0
#define MESSAGE_TYPE_MENU 1
#define MESSAGE_TYPE_FAST_SELLERS 2


typedef struct data_menu_category {
  char title[32];
  int32_t meal_count;
  int32_t meal_index;
} data_menu_category;

typedef struct data_menu_meal {
  char title[64];
  char price_student[5];
  char price_guest[5];
} data_menu_meal;

typedef struct data_fast_seller {
  char title[64];
  int32_t date;
} data_fast_seller;


typedef void (*MenuAvailable)();


bool data_init_receiver();
void data_deinit();

void data_set_menu_available_callback(MenuAvailable callback);

int32_t data_get_menu_category_count();
data_menu_category* data_get_menu_category(int32_t index);

int32_t data_get_menu_meal_count();
data_menu_meal* data_get_menu_meal(int32_t index);

int32_t data_get_fast_seller_count();
data_fast_seller* data_get_fast_seller(int32_t index);
