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

#include <pebble.h>
#include <inttypes.h>


static Window *s_main_window;
static MenuLayer *s_menu_layer;
static ProgressLayer *s_progress_layer;


static void progress_layer_hidden() {
  // exit if the window is already unloaded
  if (s_main_window == NULL) {
    return;
  }

  window_set_background_color(s_main_window, COLOR_WINDOW_BACKGROUND);

  Layer *menu_layer = menu_layer_get_layer(s_menu_layer);
  menu_layer_reload_data(s_menu_layer);
  layer_set_hidden(menu_layer, false);
  util_layer_animation_slide_up(menu_layer);
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
  return 26;
}

static int16_t menu_get_header_height(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 20;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  data_menu_category *category = data_get_menu_category(cell_index->section);
  data_menu_meal *meal = data_get_menu_meal(category->meal_index + cell_index->row);

  GRect bounds = layer_get_bounds(cell_layer);
  graphics_draw_text(ctx, meal->title, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                     GRect(PBL_IF_RECT_ELSE(6, 24), 0, bounds.size.w - PBL_IF_RECT_ELSE(6, 42), 26),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  data_menu_category *category = data_get_menu_category(section_index);

  GRect bounds = layer_get_bounds(cell_layer);
  graphics_draw_text(ctx, category->title, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
                     GRect(PBL_IF_RECT_ELSE(6, 24), 2, bounds.size.w - PBL_IF_RECT_ELSE(6, 42), 16),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  uint16_t index = cell_index->row;
}


static void window_load(Window *window) {
  window_set_background_color(window, COLOR_WINDOW_PROGRESS_BACKGROUND);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

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
    .select_click = menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_set_hidden(menu_layer_get_layer(s_menu_layer), true);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  s_progress_layer = progress_layer_create(bounds);
  layer_add_child(window_layer, s_progress_layer);
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
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
