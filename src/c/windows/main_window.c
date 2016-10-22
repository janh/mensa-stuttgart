/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "main_window.h"
#include "../layers/progress_layer.h"
#include "../lib/colors.h"
#include "../lib/util.h"
#include "../lib/data.h"
#include "../lib/strings.h"
#include "../lib/size.h"

#include <pebble.h>
#include <inttypes.h>


static Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_text_layer;
static ProgressLayer *s_progress_layer;


static void progress_layer_hidden() {
  // exit if the window is already unloaded
  if (s_main_window == NULL) {
    return;
  }

  window_set_background_color(s_main_window, COLOR_WINDOW_BACKGROUND);

  Layer *menu_layer = menu_layer_get_layer(s_menu_layer);
  Layer *text_layer = text_layer_get_layer(s_text_layer);

  if (data_get_menu_category_count() != 0) {
    menu_layer_reload_data(s_menu_layer);
    layer_set_hidden(text_layer, true);
    layer_set_hidden(menu_layer, false);
    util_layer_animation_slide_up(menu_layer);
  } else {
    layer_set_hidden(menu_layer, true);
    layer_set_hidden(text_layer, false);
    util_layer_animation_slide_up(text_layer);
  }
}

static void menu_available() {
  progress_layer_hide(s_progress_layer, true, &progress_layer_hidden);
}


static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return data_get_menu_category_count();
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  if (data_get_menu_category_count() == 0) {
    return 0;
  }
  data_menu_category *category = data_get_menu_category(section_index);
  return category->meal_count;
}

static int16_t menu_get_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  data_menu_category *category = data_get_menu_category(cell_index->section);
  data_menu_meal *meal = data_get_menu_meal(category->meal_index + cell_index->row);

  int size_index = size_get_index();
  GRect bounds = layer_get_bounds(menu_layer_get_layer(menu_layer));

  GRect rect = GRect(0, 0,
                     bounds.size.w - SIZE_MENU_MARGIN_LEFT[size_index] - SIZE_MENU_MARGIN_RIGHT[size_index],
                     SIZE_MEAL_MAX_HEIGHT[size_index]);

  GSize size = graphics_text_layout_get_content_size(meal->title, fonts_get_system_font(SIZE_MEAL_FONT[size_index]),
                                                     rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);

  return size.h + SIZE_PRICE_HEIGHT[size_index];
}

static int16_t menu_get_header_height(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  int size_index = size_get_index();
  return SIZE_CATEGORY_HEIGHT[size_index];
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  data_menu_category *category = data_get_menu_category(cell_index->section);
  data_menu_meal *meal = data_get_menu_meal(category->meal_index + cell_index->row);

  int size_index = size_get_index();
  GRect bounds = layer_get_bounds(cell_layer);
  GRect rect;

  rect = GRect(SIZE_MENU_MARGIN_LEFT[size_index],
               0,
               bounds.size.w - SIZE_MENU_MARGIN_LEFT[size_index] - SIZE_MENU_MARGIN_RIGHT[size_index],
               bounds.size.h - SIZE_PRICE_HEIGHT[size_index]);

  graphics_draw_text(ctx, meal->title, fonts_get_system_font(SIZE_MEAL_FONT[size_index]),
                     rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

  char price[24];
  snprintf(price, sizeof(price), STRING_FORMAT_MEAL_PRICE, meal->price_student, meal->price_guest);

  rect = GRect(SIZE_MENU_MARGIN_LEFT[size_index],
               bounds.size.h - SIZE_PRICE_OFFSET[size_index],
               bounds.size.w - SIZE_MENU_MARGIN_LEFT[size_index] - SIZE_MENU_MARGIN_RIGHT[size_index],
               SIZE_PRICE_OFFSET[size_index]);

  graphics_draw_text(ctx, price, fonts_get_system_font(SIZE_PRICE_FONT[size_index]),
                     rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  data_menu_category *category = data_get_menu_category(section_index);

  int size_index = size_get_index();
  GRect bounds = layer_get_bounds(cell_layer);

  GRect rect = GRect(SIZE_MENU_MARGIN_LEFT[size_index],
                     SIZE_CATEGORY_OFFSET[size_index],
                     bounds.size.w - SIZE_MENU_MARGIN_LEFT[size_index] - SIZE_MENU_MARGIN_RIGHT[size_index],
                     SIZE_CATEGORY_HEIGHT[size_index] - SIZE_CATEGORY_OFFSET[size_index]);

  graphics_draw_text(ctx, category->title, fonts_get_system_font(SIZE_CATEGORY_FONT[size_index]),
                     rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}


static void window_load(Window *window) {
  window_set_background_color(window, COLOR_WINDOW_PROGRESS_BACKGROUND);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int size_index = size_get_index();

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_normal_colors(s_menu_layer, COLOR_MENU_NORMAL_BACKGROUND, COLOR_MENU_NORMAL_FOREGROUND);
  menu_layer_set_highlight_colors(s_menu_layer, COLOR_MENU_HIGHLIGHT_BACKGROUND, COLOR_MENU_HIGHLIGHT_FOREGROUND);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_cell_height = menu_get_cell_height,
    .get_header_height = menu_get_header_height,
    .draw_row = menu_draw_row_callback,
    .draw_header = menu_draw_header_callback,
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_set_hidden(menu_layer_get_layer(s_menu_layer), true);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  int16_t text_width = SIZE_UNAVAILABLE_WIDTH[size_index];
  GRect rect = GRect((bounds.size.w - text_width) / 2, 0, text_width, bounds.size.h);
  s_text_layer = text_layer_create(rect);
  text_layer_set_text_color(s_text_layer, COLOR_WINDOW_FOREGROUND);
  text_layer_set_font(s_text_layer, fonts_get_system_font(SIZE_UNAVAILABLE_FONT[size_index]));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_text(s_text_layer, STRING_MENU_NOT_AVAILABLE);
  layer_set_hidden(text_layer_get_layer(s_text_layer), true);
  GSize size = text_layer_get_content_size(s_text_layer);
  rect.origin.y = (bounds.size.h - size.h) / 2 - 5;
  rect.size.h = size.h + 10;
  layer_set_frame(text_layer_get_layer(s_text_layer), rect);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  s_progress_layer = progress_layer_create(bounds);
  layer_add_child(window_layer, s_progress_layer);
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_text_layer);
  progress_layer_destroy(s_progress_layer);
  window_destroy(s_main_window);
  s_main_window = NULL;
}

static void window_appear(Window *window) {
  progress_layer_show(s_progress_layer);
  data_set_menu_available_callback(menu_available);
}

static void window_disappear(Window *window) {
  progress_layer_hide(s_progress_layer, false, NULL);
}


void main_window_push() {
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .appear = window_appear,
      .disappear = window_disappear,
      .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
