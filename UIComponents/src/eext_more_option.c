/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

static const int ITEM_COUNT_MAX = 16;

static void
_creat_page(Evas_Object *parent, int index)
{
	Evas_Object *img = NULL;

	/* Create the new item */
	Eext_Object_Item *item  = eext_more_option_item_append(parent);
	printf("item is created!!! [%p]\n", item);

	/* Set the content in item */
	if (index % 4 == 0) {
		/* Set the text in item text part */
		eext_more_option_item_part_text_set(item, "selector,main_text", "test1");
		eext_more_option_item_part_text_set(item, "selector,sub_text", "test1");

		img = elm_image_add(parent);
		/* Set the content in item content part */
		eext_more_option_item_part_content_set(item, "item,icon", img);
		elm_image_file_set(img, ICON_DIR"/music_controller_btn_play.png", NULL);
	} else if (index % 4 == 1) {
		eext_more_option_item_part_text_set(item, "selector,main_text", "test2");

		img = elm_image_add(parent);
		eext_more_option_item_part_content_set(item, "item,icon", img);
		elm_image_file_set(img, ICON_DIR"/music_controller_btn_shuffle_on.png", NULL);
	} else if (index % 4 == 2) {
		eext_more_option_item_part_text_set(item, "selector,main_text", "test3");

		img = elm_image_add(parent);
		eext_more_option_item_part_content_set(item, "item,icon", img);
		elm_image_file_set(img, ICON_DIR"/music_controller_btn_repeat_all.png", NULL);
	} else if (index % 4 == 3) {
		eext_more_option_item_part_text_set(item, "selector,main_text", "test4");

		img = elm_image_add(parent);
		eext_more_option_item_part_content_set(item, "item,icon", img);
		elm_image_file_set(img, ICON_DIR"/music_controller_btn_mobile.png", NULL);
	}
	return;
}

static void
_more_option_opened(void *data, Evas_Object *obj, void *event_info)
{
	printf("more option is opened!!! \n");
}

static void
_more_option_closed(void *data, Evas_Object *obj, void *event_info)
{
	printf("more option is closed!!! \n");
}

static void
_item_selected(void *data, Evas_Object *obj, void *event_info)
{
	Eext_Object_Item *item = (Eext_Object_Item *)event_info;
	const char *main_text;
	const char *sub_text;

	/* Get the text in item text part */
	main_text = eext_more_option_item_part_text_get(item, "selector,main_text");
	sub_text = eext_more_option_item_part_text_get(item, "selector,sub_text");
	printf("item[%p]:main text[%s](sub text[%s]) is selected!\n", item, main_text, sub_text);
}

static void
_item_clicked(void *data, Evas_Object *obj, void *event_info)
{
	Eext_Object_Item *item = (Eext_Object_Item *)event_info;
	const char *main_text;
	const char *sub_text;

	main_text = eext_more_option_item_part_text_get(item, "selector,main_text");
	sub_text = eext_more_option_item_part_text_get(item, "selector,sub_text");
	printf("item[%p]:main text[%s](sub text[%s]) is selected!\n", item, main_text, sub_text);
}

void
eext_more_option_cb(void *data, Evas_Object *obj, void *event_info)
{
	int i = 0;
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	/* Create the more option object.
	 * Beacus the return value is the elm_layout handle,
	 * the elm_layout APIs can be applicable to rotary selector handle.
	 */
	Evas_Object *more_option = eext_more_option_add(nf);

	for (i = 0; i < ITEM_COUNT_MAX; ++i) {
		if (i == 0)
			_creat_page(more_option, i);
		else
			_creat_page(more_option, i);
	}

	/* Add smart callback */
	evas_object_smart_callback_add(more_option, "more,option,opened", _more_option_opened, NULL);
	evas_object_smart_callback_add(more_option, "more,option,closed", _more_option_closed, NULL);
	evas_object_smart_callback_add(more_option, "item,selected", _item_selected, NULL);
	evas_object_smart_callback_add(more_option, "item,clicked", _item_clicked, NULL);

	/* Push the More option as Naviframe item */
	elm_naviframe_item_push(nf, "More Option Demo", NULL, NULL, more_option, "empty");
}
