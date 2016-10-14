/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Pebble Examples
 * Copyright (c) 2016 Jan Hoffmann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "progress_layer.h"
#include "../lib/colors.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

typedef struct {
  int16_t progress_percent;
  AppTimer *timer;
  ProgressLayerHidden callback;
} ProgressLayerData;

static int16_t scale_progress_bar_width_px(unsigned int progress_percent, int16_t rect_width_px) {
  return ((progress_percent * (rect_width_px)) / 10000);
}

static void progress_layer_update_proc(ProgressLayer* progress_layer, GContext* ctx) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  GRect bounds = layer_get_bounds(progress_layer);

  int16_t progress_bar_width_px = scale_progress_bar_width_px(data->progress_percent, bounds.size.w);
  GRect progress_bar = GRect(bounds.origin.x, bounds.origin.y, progress_bar_width_px, bounds.size.h);

  graphics_context_set_fill_color(ctx, COLOR_LAYER_PROGRESS_BACKGROUND);
  graphics_fill_rect(ctx, bounds, 2, GCornersAll);

  graphics_context_set_fill_color(ctx, COLOR_LAYER_PROGRESS_FOREGROUND);
  graphics_fill_rect(ctx, progress_bar, 2, GCornersAll);

#ifndef PBL_COLOR
  graphics_context_set_stroke_color(ctx, COLOR_LAYER_PROGRESS_FOREGROUND);
  graphics_draw_round_rect(ctx, bounds, 2);
#endif
}

ProgressLayer* progress_layer_create(GRect frame) {
  GRect actual_frame = GRect(frame.origin.x + (frame.size.w - 80) / 2, frame.origin.y + (frame.size.h - 6) / 2, 80, 6);
  ProgressLayer *progress_layer = layer_create_with_data(actual_frame, sizeof(ProgressLayerData));
  layer_set_update_proc(progress_layer, progress_layer_update_proc);
  layer_mark_dirty(progress_layer);

  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->progress_percent = 0;

  layer_set_hidden(progress_layer, true);

  return progress_layer;
}

void progress_layer_destroy(ProgressLayer* progress_layer) {
  if (progress_layer) {
    layer_destroy(progress_layer);
  }
}

static void progress_layer_callback(void *context);

static void progress_layer_next_timer(ProgressLayer* progress_layer) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->timer = app_timer_register(33, progress_layer_callback, progress_layer);
}

static void progress_layer_callback(void *context) {
  ProgressLayer *progress_layer = (ProgressLayer *)context;

  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->progress_percent = MIN(10000, data->progress_percent + (10000 - data->progress_percent) / 50 + 1);
  layer_mark_dirty(progress_layer);

  progress_layer_next_timer(progress_layer);
}

void progress_layer_show(ProgressLayer* progress_layer) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->progress_percent = 0;
  layer_set_hidden(progress_layer, false);
  layer_mark_dirty(progress_layer);

  progress_layer_next_timer(progress_layer);
}

static void progress_layer_anim_stopped_handler(Animation *animation, bool finished, void *context) {
  ProgressLayer *progress_layer = (ProgressLayer *)context;
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);

  layer_set_hidden(progress_layer, true);

  GRect frame = layer_get_frame(progress_layer);
  frame.size.w = 80;
  frame.origin.x -= 37;
  layer_set_frame(progress_layer, frame);

  if (data->callback != NULL) {
    data->callback();
  }
}

static void progress_layer_hide_animate(ProgressLayer* progress_layer, ProgressLayerHidden callback) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->callback = callback;

  GRect start = layer_get_frame(progress_layer);
  GRect end = start;
  end.size.w = 6;
  end.origin.x += 37;

  Animation *anim = (Animation *)property_animation_create_layer_frame(progress_layer, &start, &end);

  animation_set_delay(anim, 50);
  animation_set_duration(anim, 100);

  animation_set_handlers(anim, (AnimationHandlers) {
    .stopped = progress_layer_anim_stopped_handler
  }, progress_layer);

  animation_schedule(anim);
}

void progress_layer_hide(ProgressLayer* progress_layer, bool animate, ProgressLayerHidden callback) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->progress_percent = 10000;
  layer_mark_dirty(progress_layer);

  if (data->timer) {
    app_timer_cancel(data->timer);
    data->timer = NULL;
  }

  if (animate) {
    progress_layer_hide_animate(progress_layer, callback);
  } else {
    layer_set_hidden(progress_layer, true);
  }
}
