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

#define ROTARY_SELECTOR_PAGE_COUNT 3
#define ROTARY_SELECTOR_PAGE_ITEM_COUNT 11

static char *icon_path_list[] = {
	ICON_DIR"/music_controller_btn_mobile.png",
	ICON_DIR"/music_controller_btn_play.png",
	ICON_DIR"/music_controller_btn_repeat_all.png",
	ICON_DIR"/music_controller_btn_shuffle_on.png",
	NULL
};

static void
_item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Eext_Object_Item *item;
	const char *main_text;
	const char *sub_text;
	/* Get current seleted item object */
	item = eext_rotary_selector_selected_item_get(obj);

	/* Get set text for the item */
	main_text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	sub_text = eext_rotary_selector_item_part_text_get(item, "selector,sub_text");

	printf("Item Selected! Currently Selected %s, %s\n", main_text, sub_text);
}

static void
_item_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	Eext_Object_Item *item;
	const char *main_text;
	const char *sub_text;

	/* Get current seleted item object */
	item = eext_rotary_selector_selected_item_get(obj);

	/* Get set text for the item */
	main_text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	sub_text = eext_rotary_selector_item_part_text_get(item, "selector,sub_text");

	printf("Item Clicked!, Currently Selected %s, %s\n", main_text, sub_text);
}

void
_item_create(Evas_Object *rotary_selector)
{
	Evas_Object *image;
	Eext_Object_Item * item;
	char buf[255] = {0};
	int i, j;

	for (i = 0; i < ROTARY_SELECTOR_PAGE_COUNT; i++)
	{
		for (j = 0; j < ROTARY_SELECTOR_PAGE_ITEM_COUNT; j++)
		{
			/* Add new eext_rotary_selector_item
				Instead of eext_rotary_selector_item_append() can be used API's
				- eext_rotary_selector_item_prepend()
				- eext_rotary_selector_item_insert_after()
				- eext_rotary_selector_item_insert_before() */
			item = eext_rotary_selector_item_append(rotary_selector);

			image = elm_image_add(rotary_selector);
			elm_image_file_set(image, icon_path_list[j % 4], NULL);

			/* Set the icon of the selector item. */
			eext_rotary_selector_item_part_content_set(item,
																	 "item,icon",
																	 EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL,
																	 image);

			/* Set the main/sub text visible when the item is selected. */
			snprintf(buf, sizeof(buf), "Page : %d", i + 1);
			eext_rotary_selector_item_part_text_set(item, "selector,main_text", buf);

			snprintf(buf, sizeof(buf), "Item : %d", j + 1);
			eext_rotary_selector_item_part_text_set(item, "selector,sub_text", buf);
		}
	}
}

void
eext_rotary_selector_cb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	Evas_Object *rotary_selector;
	Elm_Object_Item *nf_it = NULL;

	/* Add a new Rotary Selector
		Because the return value is the elm_layout handle,
		the elm_layout APIs can be applicable to rotary selector handle. */
	rotary_selector = eext_rotary_selector_add(nf);

	/* Rotary event activated set */
	eext_rotary_object_event_activated_set(rotary_selector, EINA_TRUE);

	_item_create(rotary_selector);

	/* Add smart callback */
	evas_object_smart_callback_add(rotary_selector, "item,selected", _item_selected_cb, NULL);
	evas_object_smart_callback_add(rotary_selector, "item,clicked", _item_clicked_cb, NULL);

	nf_it = elm_naviframe_item_push(nf, _("Rotary Selector"), NULL, NULL, rotary_selector, "empty");
}
