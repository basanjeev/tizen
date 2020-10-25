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

static void
_value_changed(void *data, Evas_Object *obj, void *event_info)
{
	char buf[PATH_MAX];
	Evas_Object *layout = (Evas_Object *)data;

	snprintf(buf, sizeof(buf), "%.1lf", eext_circle_object_value_get(obj));
	printf("Slider value = %s\n", buf);
	elm_object_part_text_set(layout, "elm.text.slider", buf);
}

void
eext_slider_cb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	Evas_Object *layout = NULL;
	Evas_Object *slider = NULL;
	Elm_Object_Item *nf_it = NULL;

	layout = elm_layout_add(nf);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "slider_layout");
	elm_object_part_text_set(layout, "elm.text.slider", "3.0");
	evas_object_show(layout);

	/* Add new eext_circle_object_slider with Eext_Circle_Surface object to render.
	Value is set as 3 which is within range from 0 to 15. */
	slider = eext_circle_object_slider_add(layout, ad->circle_surface);
	eext_circle_object_value_min_max_set(slider, 0.0, 15.0);
	eext_circle_object_value_set(slider, 3.0);

	/* Activate Circle slider's rotary object event.
	Its value increases/decreases its value by 0.5 to clockwise/counter clockwise
	rotary event. */
	eext_rotary_object_event_activated_set(slider, EINA_TRUE);
	eext_circle_object_slider_step_set(slider, 0.5);
	evas_object_smart_callback_add(slider, "value,changed", _value_changed, layout);

	nf_it = elm_naviframe_item_push(nf, _("Slider"), NULL, NULL, layout, "empty");
}
