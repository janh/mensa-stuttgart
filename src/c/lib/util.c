/* 
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "util.h"


void util_layer_animation_slide_up(Layer *layer) {
  GRect end = layer_get_frame(layer);
  GRect start = grect_inset(end, GEdgeInsets(20,0,-20,0));
  layer_set_frame(layer, start);

  Animation *anim = (Animation *)property_animation_create_layer_frame(layer, &start, &end);
  animation_set_curve(anim, AnimationCurveEaseOut);
  animation_schedule(anim);
}

void util_string_copy(char *dest, const char *src, size_t length) {
  length--;
  strncpy(dest, src, length);
  dest[length] = '\0';
}
