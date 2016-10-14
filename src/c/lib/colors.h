/* 
 * Copyright (c) 2016 Jan Hoffmann
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once


#define COLOR_WINDOW_BACKGROUND GColorWhite
#define COLOR_WINDOW_FOREGROUND GColorBlack

#define COLOR_WINDOW_PROGRESS_BACKGROUND PBL_IF_COLOR_ELSE(GColorLightGray,GColorWhite)
#define COLOR_LAYER_PROGRESS_BACKGROUND PBL_IF_COLOR_ELSE(GColorBlack,GColorWhite)
#define COLOR_LAYER_PROGRESS_FOREGROUND PBL_IF_COLOR_ELSE(GColorWhite,GColorBlack)

#define COLOR_MENU_NORMAL_BACKGROUND GColorWhite
#define COLOR_MENU_NORMAL_FOREGROUND GColorBlack

#define COLOR_MENU_HIGHLIGHT_BACKGROUND PBL_IF_COLOR_ELSE(GColorDarkGray,GColorBlack)
#define COLOR_MENU_HIGHLIGHT_FOREGROUND PBL_IF_COLOR_ELSE(GColorWhite,GColorWhite)
