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
check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	int check_num = (int)(data);

	Eina_Bool state = elm_check_state_get(obj);

	printf("Check %d:%d\n", check_num, state);
}

static Evas_Object*
create_scroller(appdata_s *ad)
{
	Evas_Object *scroller = elm_scroller_add(ad->nf);
	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);

	Evas_Object *circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	return scroller;
}

static Evas_Object*
create_checks(Evas_Object *parent)
{
	Evas_Object *box;
	Evas_Object *check;

	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_content_set(parent, box);
	evas_object_show(box);

	check = elm_check_add(box);
	elm_check_state_set(check, EINA_TRUE);
	evas_object_smart_callback_add(check, "changed", check_changed_cb, (void *)1);
	evas_object_show(check);
	elm_box_pack_end(box, check);

	check = elm_check_add(box);
	elm_object_style_set(check, "small");
	elm_check_state_set(check, EINA_TRUE);
	evas_object_smart_callback_add(check, "changed", check_changed_cb, (void *)2);
	evas_object_show(check);
	elm_box_pack_end(box, check);

	check = elm_check_add(box);
	elm_object_style_set(check, "on&off");
	elm_check_state_set(check, EINA_TRUE);
	evas_object_smart_callback_add(check, "changed", check_changed_cb, (void *)3);
	evas_object_show(check);
	elm_box_pack_end(box, check);

	return box;
}

void
check_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *scroller, *box;
	Evas_Object *nf = ad->nf;

	scroller = create_scroller(ad);
	box = create_checks(scroller);
	elm_object_content_set(scroller, box);

	elm_naviframe_item_push(nf, "Check", NULL, NULL, scroller, "empty");
}
