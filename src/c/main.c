/*
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "lib/data.h"
#include "windows/main_window.h"

#include <pebble.h>


static void init(void) {
  if (data_init_receiver()) {
    main_window_push();
  }
}

# ifndef PBL_PLATFORM_APLITE
static void app_glance_reload_callback(AppGlanceReloadSession *session, size_t limit, void *context) {
  size_t count = data_get_fast_seller_count();
  if (limit < count) {
    count = limit;
  }

  data_fast_seller *fast_seller;

  for (unsigned int i = 0; i < count; i++) {
    fast_seller = data_get_fast_seller(i);

    AppGlanceSlice slice = (AppGlanceSlice) {
      .layout = {
        .icon = APP_GLANCE_SLICE_DEFAULT_ICON,
        .subtitle_template_string = fast_seller->title
      },
      .expiration_time = fast_seller->date
    };

    AppGlanceResult result = app_glance_add_slice(session, slice);
    if (result != APP_GLANCE_RESULT_SUCCESS) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to add app glance slice: %d", result);
    }
  }

  data_deinit();
}
#endif

static void deinit(void) {
  # ifndef PBL_PLATFORM_APLITE
  if (data_get_fast_seller_count() > 0) {
    app_glance_reload(app_glance_reload_callback, NULL);
  } else {
    data_deinit();
  }
  # else
  data_deinit();
  # endif
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
