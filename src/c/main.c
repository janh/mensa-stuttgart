/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "windows/main_window.h"

#include <pebble.h>


static void init(void) {
  main_window_push();
}

static void deinit(void) {
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
