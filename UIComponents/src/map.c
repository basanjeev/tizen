/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "main.h"

void
map_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *nf = (Evas_Object *)data;
	Evas_Object *map;
	Elm_Map_Overlay *overlay;

	map = elm_map_add(nf);
	evas_object_size_hint_weight_set(map, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(map);

	elm_map_zoom_set(map, 10);
	elm_map_region_bring_in(map, 2.352, 48.857);

	overlay = elm_map_overlay_add(map, 2.352, 48.857);
	elm_map_overlay_color_set(overlay, 0x00, 0xfa, 0x9a, 0xff);
	elm_map_overlay_displayed_zoom_min_set(overlay, 5);

	elm_naviframe_item_push(nf, "Map", NULL, NULL, map, "empty");
}

