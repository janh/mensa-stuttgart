/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "data.h"
#include "util.h"
#include "strings.h"
#include "../windows/error_window.h"
#include "../windows/main_window.h"

#include <pebble.h>


static bool s_error_received;

static bool s_menu_received;
static MenuAvailable s_menu_available_callback;

static int32_t s_menu_categories_count;
static data_menu_category *s_menu_categories;

static int32_t s_menu_meals_count;
static data_menu_meal *s_menu_meals;

static char s_menu_message[32];

static int32_t s_fast_sellers_count;
static data_fast_seller *s_fast_sellers;

static AppTimer *s_timer;


static void timeout_handler(void *data) {
  s_timer = NULL;
  window_stack_pop(false);
  error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_TIMEOUT);
}

static void cancel_timer() {
  if (s_timer != NULL) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
}

static void start_timer() {
  cancel_timer();
  s_timer = app_timer_register(30000, timeout_handler, NULL);
}


static void handle_error() {
  if (!s_error_received) {
    cancel_timer();
    s_error_received = true;
    window_stack_pop(false);
    error_window_push(STRING_ERROR_TITLE_DATA, STRING_ERROR_DESC_DATA);
  }
}


void data_set_menu_available_callback(MenuAvailable callback) {
  s_menu_available_callback = callback;
  if (s_menu_received && s_menu_available_callback != NULL) {
    s_menu_available_callback();
  }
}

static void clear_menu_categories() {
  if (s_menu_categories_count != 0) {
    free(s_menu_categories);
    s_menu_categories = NULL;
    s_menu_categories_count = 0;
  }
}

static void clear_menu_meals() {
  if (s_menu_meals_count != 0) {
    free(s_menu_meals);
    s_menu_meals = NULL;
    s_menu_meals_count = 0;
  }
}

int32_t data_get_menu_category_count() {
  return s_menu_categories_count;
}

data_menu_category* data_get_menu_category(int32_t index) {
  if (index < s_menu_categories_count) {
    return &s_menu_categories[index];
  }
  return NULL;
}

int32_t data_get_menu_meal_count() {
  return s_menu_meals_count;
}

data_menu_meal* data_get_menu_meal(int32_t index) {
  if (index < s_menu_meals_count) {
    return &s_menu_meals[index];
  }
  return NULL;
}

char* data_get_menu_message() {
  return s_menu_message;
}

static void handle_menu_categories(DictionaryIterator *iterator) {
  clear_menu_categories();

  Tuple *count_tuple = dict_find(iterator, MESSAGE_KEY_MenuCategoriesCount);
  if (!count_tuple) {
    window_stack_pop(false);
    error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
    return;
  }
  s_menu_categories_count = count_tuple->value->uint32;
  s_menu_categories = malloc(s_menu_categories_count * sizeof(s_menu_categories[0]));

  for (int i = 0; i < s_menu_categories_count; i++) {
    Tuple *title_tuple = dict_find(iterator, MESSAGE_KEY_MenuCategoriesTitleStart + i);
    Tuple *meal_count_tuple = dict_find(iterator, MESSAGE_KEY_MenuCategoriesMealCountStart + i);
    Tuple *meal_index_tuple = dict_find(iterator, MESSAGE_KEY_MenuCategoriesMealIndexStart + i);

    if (!title_tuple || !meal_count_tuple || !meal_index_tuple) {
      window_stack_pop(false);
      error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
      return;
    }

    util_string_copy(s_menu_categories[i].title, title_tuple->value->cstring, sizeof(s_menu_categories[0].title));
    s_menu_categories[i].meal_count = meal_count_tuple->value->uint32;
    s_menu_categories[i].meal_index = meal_index_tuple->value->uint32;
  }
}

static void handle_menu_meals(DictionaryIterator *iterator) {
  clear_menu_meals();

  Tuple *count_tuple = dict_find(iterator, MESSAGE_KEY_MenuMealsCount);
  if (!count_tuple) {
    window_stack_pop(false);
    error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
    return;
  }
  s_menu_meals_count = count_tuple->value->uint32;
  s_menu_meals = malloc(s_menu_meals_count * sizeof(s_menu_meals[0]));

  for (int i = 0; i < s_menu_meals_count; i++) {
    Tuple *title_tuple = dict_find(iterator, MESSAGE_KEY_MenuMealsTitleStart + i);
    Tuple *price_student_tuple = dict_find(iterator, MESSAGE_KEY_MenuMealsPriceStudentStart + i);
    Tuple *price_guest_tuple = dict_find(iterator, MESSAGE_KEY_MenuMealsPriceGuestStart + i);

    if (!title_tuple || !price_student_tuple || !price_guest_tuple) {
      window_stack_pop(false);
      error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
      return;
    }

    util_string_copy(s_menu_meals[i].title, title_tuple->value->cstring, sizeof(s_menu_meals[0].title));
    util_string_copy(s_menu_meals[i].price_student, price_student_tuple->value->cstring, sizeof(s_menu_meals[0].price_student));
    util_string_copy(s_menu_meals[i].price_guest, price_guest_tuple->value->cstring, sizeof(s_menu_meals[0].price_guest));
  }
}

static void handle_menu_message(DictionaryIterator *iterator) {
  Tuple *message_tuple = dict_find(iterator, MESSAGE_KEY_MenuMessage);

  if (!message_tuple) {
    window_stack_pop(false);
    error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
    return;
  }

  util_string_copy(s_menu_message, message_tuple->value->cstring, sizeof(s_menu_message));
}

static void handle_menu(DictionaryIterator *iterator) {
  cancel_timer();
  handle_menu_categories(iterator);
  handle_menu_meals(iterator);
  handle_menu_message(iterator);
  s_menu_received = true;
  if (s_menu_available_callback != NULL) {
    s_menu_available_callback();
  }
}


static void clear_fast_sellers() {
  if (s_fast_sellers_count != 0) {
    free(s_fast_sellers);
    s_fast_sellers = NULL;
    s_fast_sellers_count = 0;
  }
}

int32_t data_get_fast_seller_count() {
  return s_fast_sellers_count;
}

data_fast_seller* data_get_fast_seller(int32_t index) {
  if (index < s_fast_sellers_count) {
    return &s_fast_sellers[index];
  }
  return NULL;
}

static void handle_fast_sellers(DictionaryIterator *iterator) {
  clear_fast_sellers();

  Tuple *count_tuple = dict_find(iterator, MESSAGE_KEY_FastSellersCount);
  if (!count_tuple) {
    window_stack_pop(false);
    error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
    return;
  }
  s_fast_sellers_count = count_tuple->value->uint32;
  s_fast_sellers = malloc(s_fast_sellers_count * sizeof(s_fast_sellers[0]));

  for (int i = 0; i < s_fast_sellers_count; i++) {
    Tuple *title_tuple = dict_find(iterator, MESSAGE_KEY_FastSellersTitleStart + i);
    Tuple *date_tuple = dict_find(iterator, MESSAGE_KEY_FastSellersDateStart + i);

    if (!title_tuple || !date_tuple) {
      window_stack_pop(false);
      error_window_push(STRING_ERROR_TITLE_TRANSFER, STRING_ERROR_DESC_TRANSFER_INVALID_MESSAGE);
      return;
    }

    util_string_copy(s_fast_sellers[i].title, title_tuple->value->cstring, sizeof(s_fast_sellers[0].title));
    s_fast_sellers[i].date = date_tuple->value->uint32;
  }
}


static void handle_reset() {
  start_timer();

  clear_menu_categories();
  clear_menu_meals();
  clear_fast_sellers();

  s_menu_received = false;
  s_error_received = false;

  window_stack_pop_all(false);
  main_window_push();
}


static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
  if (s_error_received) {
    return;
  }

  Tuple *message_type_tuple = dict_find(iterator, MESSAGE_KEY_MessageType);

  if (message_type_tuple) {
    uint32_t message_type = message_type_tuple->value->int32;

    switch (message_type) {
    case MESSAGE_TYPE_ERROR:
      handle_error(iterator);
      break;
    case MESSAGE_TYPE_MENU:
      handle_menu(iterator);
      break;
    case MESSAGE_TYPE_FAST_SELLERS:
      handle_fast_sellers(iterator);
      break;
    case MESSAGE_TYPE_RESET:
      handle_reset(iterator);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_INFO, "Unhandled message: %" PRId32, message_type);
    }
  }
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  char desc[48];
  snprintf(desc, sizeof(desc), STRING_ERROR_DESC_TRANSFER_MESSAGE_DROPPED, reason);
  window_stack_pop(false);
  error_window_push(STRING_ERROR_TITLE_TRANSFER, desc);
}


bool data_init_receiver() {
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);

  uint32_t max_inbox_size = app_message_inbox_size_maximum();
  if (max_inbox_size < 2048) {
    char desc[48];
    snprintf(desc, sizeof(desc), STRING_ERROR_DESC_TRANSFER_INBOX_SIZE, max_inbox_size);
    window_stack_pop_all(false);
    error_window_push(STRING_ERROR_TITLE_TRANSFER, desc);
    return false;
  }

  start_timer();

  app_message_open(2048, 32);

  return true;
}

void data_deinit() {
  clear_menu_categories();
  clear_menu_meals();
  clear_fast_sellers();
}
