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

void scroller_cb(void *data, Evas_Object * obj, void *event_info)
{
	Evas_Object *nf = (Evas_Object *)data;
	Evas_Object *scroller, *layout, *label;

	/* Create Layout */
	layout = elm_layout_add(nf);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "elmdemo-test/scroller");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* Create Scroller */
	scroller = elm_scroller_add(layout);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	elm_object_style_set(scroller, "effect");
	evas_object_show(scroller);

	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);

	/* Create Content */
	label = elm_label_add(scroller);
	elm_object_text_set(label, "<br/><br/><align=center>  Tizen is an open and flexible operating system "
										"built from the ground up to address the needs of all stakeholders of "
										"the mobile and connected device ecosystem, including device manufacturers, "
										"mobile operators, application developers and independent software vendors "
										"(ISVs). Tizen is developed by a community of developers, under open source "
										"governance, and is open to all members who wish to participate.<br/><br/>");

	elm_label_line_wrap_set(label, ELM_WRAP_WORD);
	elm_label_wrap_width_set(label, 360);
	evas_object_size_hint_weight_set(label, 0.5, 0.5);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(label);

	elm_object_part_content_set(layout, "scroller", scroller);
	elm_object_content_set(scroller, label);

	Elm_Object_Item *it = elm_naviframe_item_push(nf, "Scroller", NULL, NULL, layout, "empty");
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);

	/* Content Set*/
	elm_object_part_content_set(layout, "scroller", scroller);
}
