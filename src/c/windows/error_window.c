/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "error_window.h"
#include "../lib/colors.h"
#include "../lib/util.h"
#include "../lib/size.h"

#include <pebble.h>


static Window *s_error_window;
static Layer *s_background_layer;
static TextLayer *s_text_layer_title;
static TextLayer *s_text_layer_desc;

char s_text_title[32];
char s_text_desc[48];


static void background_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, COLOR_WINDOW_ERROR_BACKGROUND);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, 0);
}

static void window_load(Window *window) {
  window_set_background_color(window, COLOR_WINDOW_PROGRESS_BACKGROUND);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int size_index = size_get_index();

  s_background_layer = layer_create(bounds);
  layer_set_update_proc(s_background_layer, background_update_proc);
  layer_add_child(window_layer, s_background_layer);

  s_text_layer_title = text_layer_create(GRect(0, 0, 0, 0));
  text_layer_set_background_color(s_text_layer_title, GColorClear);
  text_layer_set_text_color(s_text_layer_title, COLOR_WINDOW_ERROR_FOREGROUND);
  text_layer_set_font(s_text_layer_title, fonts_get_system_font(SIZE_ERROR_TITLE_FONT[size_index]));
  text_layer_set_text_alignment(s_text_layer_title, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
  layer_add_child(s_background_layer, text_layer_get_layer(s_text_layer_title));

  s_text_layer_desc = text_layer_create(GRect(0, 0, 0, 0));
  text_layer_set_background_color(s_text_layer_desc, GColorClear);
  text_layer_set_text_color(s_text_layer_desc, COLOR_WINDOW_ERROR_FOREGROUND);
  text_layer_set_font(s_text_layer_desc, fonts_get_system_font(SIZE_ERROR_DESC_FONT[size_index]));
  text_layer_set_text_alignment(s_text_layer_desc, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
  layer_add_child(s_background_layer, text_layer_get_layer(s_text_layer_desc));
}

static void window_unload(Window *window) {
  layer_destroy(s_background_layer);
  text_layer_destroy(s_text_layer_title);
  text_layer_destroy(s_text_layer_desc);
  window_destroy(s_error_window);
  s_error_window = NULL;
}

static void window_appear(Window *window) {
  GRect frame = layer_get_frame(s_background_layer);
  int size_index = size_get_index();

  int top = (frame.size.h - SIZE_ERROR_HEIGHT[size_index]) / 2;

  text_layer_set_text(s_text_layer_title, s_text_title);
  Layer *text_layer_title = text_layer_get_layer(s_text_layer_title);
  GRect title_rect = GRect(SIZE_ERROR_MARGIN_HORIZ[size_index],
                           top,
                           frame.size.w - 2 * SIZE_ERROR_MARGIN_HORIZ[size_index],
                           SIZE_ERROR_TITLE_MAX_HEIGHT[size_index]);
  layer_set_frame(text_layer_title, title_rect);
  text_layer_enable_screen_text_flow_and_paging(s_text_layer_title, SIZE_ERROR_MARGIN_HORIZ[size_index]);

  GSize title_size = text_layer_get_content_size(s_text_layer_title);
  title_rect.size.h = title_size.h + SIZE_ERROR_MARGIN_VERT[size_index];
  layer_set_frame(text_layer_title, title_rect);

  text_layer_set_text(s_text_layer_desc, s_text_desc);
  Layer *text_layer_desc = text_layer_get_layer(s_text_layer_desc);
  GRect desc_rect = GRect(SIZE_ERROR_MARGIN_HORIZ[size_index],
                          top + SIZE_ERROR_MARGIN_VERT[size_index] + title_size.h,
                          frame.size.w - 2 * SIZE_ERROR_MARGIN_HORIZ[size_index],
                          SIZE_ERROR_HEIGHT[size_index] - SIZE_ERROR_MARGIN_VERT[size_index] - title_size.h);
  layer_set_frame(text_layer_desc, desc_rect);
  text_layer_enable_screen_text_flow_and_paging(s_text_layer_desc, SIZE_ERROR_MARGIN_HORIZ[size_index]);

  GRect end = frame;
  GRect start = grect_inset(end, GEdgeInsets(frame.size.h, 0, 0, 0));
  layer_set_frame(s_background_layer, start);

  PropertyAnimation *property_anim = property_animation_create_layer_frame(s_background_layer, &start, &end);
  Animation *anim = property_animation_get_animation(property_anim);
  animation_schedule(anim);

  light_enable_interaction();
}


void error_window_push(const char *text_title, const char *text_desc) {
  if (!s_error_window) {
    s_error_window = window_create();
    window_set_window_handlers(s_error_window, (WindowHandlers) {
      .load = window_load,
      .appear = window_appear,
      .unload = window_unload,
    });
  }

  util_string_copy(s_text_title, text_title, sizeof(s_text_title));
  util_string_copy(s_text_desc, text_desc, sizeof(s_text_desc));

  window_stack_push(s_error_window, true);
}
