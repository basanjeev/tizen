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

static void _radio_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	int x = 0;
	printf("Selected Radio:%d\n", (int)(long)data);
	x = elm_radio_value_get(obj);
	//will print the value assigned to the currently selected radio
	printf("selected value for the group:%d\n", x);
}

void radio_cb(void *data, Evas_Object * obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	Evas_Object *scroller, *circle_scroller, *layout;
	Evas_Object *radio, *rdg = NULL;

	scroller = elm_scroller_add(nf);
	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);

	circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	layout = elm_layout_add(scroller);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "elmdemo-test/radio");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	//adding a radio instance
	radio = elm_radio_add(layout);
	elm_object_part_content_set(layout, "radio1", radio);
	//assigning a unique value(within the group) to the radio instance
	elm_radio_state_value_set(radio, 10);
	evas_object_smart_callback_add(radio, "changed", _radio_clicked_cb, (void *)1);
	//creating a radio group with first radio
	rdg = radio;

	radio = elm_radio_add(layout);
	//assigning a unique value(within the group) to the radio instance
	elm_radio_state_value_set(radio, 20);
	//adding this radio to the group containing the first radio
	elm_radio_group_add(radio, rdg);
	elm_object_part_content_set(layout, "radio2", radio);
	evas_object_smart_callback_add(radio, "changed", _radio_clicked_cb, (void *)2);

	radio = elm_radio_add(layout);
	//assigning a unique value(within the group) to the radio instance
	elm_radio_state_value_set(radio, 30);
	//adding this radio to the group containing the first radio
	elm_radio_group_add(radio, rdg);
	elm_object_part_content_set(layout, "radio3", radio);
	evas_object_smart_callback_add(radio, "changed", _radio_clicked_cb, (void *)3);

	radio = elm_radio_add(layout);
	//assigning a unique value(within the group) to the radio instance
	elm_radio_state_value_set(radio, 40);
	//adding this radio to the group containing the first radio
	elm_radio_group_add(radio, rdg);
	elm_object_part_content_set(layout, "radio4", radio);
	elm_object_disabled_set(radio, EINA_TRUE);
	evas_object_smart_callback_add(radio, "changed", _radio_clicked_cb, (void *)4);

	//selecting the second radio in the group with value set to 10. This will set the 2nd radio instance
	elm_radio_value_set(rdg, 10);

	elm_object_content_set(scroller, layout);

	elm_naviframe_item_push(nf, "Radio", NULL, NULL, scroller, "empty");
}
