/*
 * Copyright (c) 2017 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "message_layer.h"
#include "../lib/colors.h"
#include "../lib/util.h"
#include "../lib/size.h"


typedef struct {
  char text_title[32];
  char text_desc[128];
} MessageLayerData;


static void message_layer_update_proc(MessageLayer* message_layer, GContext* ctx) {
  MessageLayerData *data = (MessageLayerData *)layer_get_data(message_layer);
  GRect bounds = layer_get_bounds(message_layer);
  int size_index = size_get_index();

  GFont font_title = fonts_get_system_font(SIZE_MESSAGE_TITLE_FONT[size_index]);
  GFont font_desc = fonts_get_system_font(SIZE_MESSAGE_DESC_FONT[size_index]);

  int16_t text_width = SIZE_MESSAGE_WIDTH[size_index];
  int16_t text_height = SIZE_MESSAGE_HEIGHT[size_index];
  GRect rect = GRect((bounds.size.w - text_width) / 2, 0, text_width, text_height);

  GSize size_title = graphics_text_layout_get_content_size(data->text_title, font_title,
                                                           rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);

  rect.size.h = rect.size.h - size_title.h - SIZE_MESSAGE_MARGIN[size_index];
  GSize size_desc = graphics_text_layout_get_content_size(data->text_desc, font_desc,
                                                          rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);

  int16_t height = size_title.h + size_desc.h + SIZE_MESSAGE_MARGIN[size_index];
  if (size_desc.h > 0) {
    height += SIZE_MESSAGE_MARGIN[size_index];
  }

  graphics_context_set_text_color(ctx, COLOR_WINDOW_FOREGROUND);

  rect.origin.y = (bounds.size.h - height) / 2;
  rect.size.h = size_title.h;
  graphics_draw_text(ctx, data->text_title, font_title,
                     rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

  rect.origin.y = rect.origin.y + size_title.h + SIZE_MESSAGE_MARGIN[size_index];
  rect.size.h = size_desc.h;
  graphics_draw_text(ctx, data->text_desc, font_desc,
                     rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void message_layer_set_content(MessageLayer* message_layer, char *text_title, char *text_desc) {
  MessageLayerData *data = (MessageLayerData *)layer_get_data(message_layer);

  util_string_copy(data->text_title, text_title, sizeof(data->text_title));
  util_string_copy(data->text_desc, text_desc, sizeof(data->text_desc));

  layer_mark_dirty(message_layer);
}

MessageLayer* message_layer_create(GRect frame) {
  MessageLayer *message_layer = layer_create_with_data(frame, sizeof(MessageLayerData));
  layer_set_update_proc(message_layer, message_layer_update_proc);
  layer_mark_dirty(message_layer);

  MessageLayerData *data = (MessageLayerData *)layer_get_data(message_layer);
  data->text_title[0] = '\0';
  data->text_desc[0] = '\0';

  return message_layer;
}

void message_layer_destroy(MessageLayer* message_layer) {
  if (message_layer) {
    layer_destroy(message_layer);
  }
}
