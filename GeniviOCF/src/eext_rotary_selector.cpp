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

#include <geniviocf.h>

static void
_item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Eext_Object_Item *item;
	const char *main_text;
	const char *sub_text;
	/* Get current seleted item object */
	item = eext_rotary_selector_selected_item_get(obj);
	/* Get set text for the item */
	main_text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	sub_text = eext_rotary_selector_item_part_text_get(item, "selector,sub_text");
	dlog_print(DLOG_INFO, LOG_TAG,"Item Selected! Currently Selected %s, %s\n", main_text, sub_text);
	ad->master.res = get_res_by_name(main_text);
}

static void
_item_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Eext_Object_Item *item;
	const char *main_text;
	const char *sub_text;

	/* Get current seleted item object */
	item = eext_rotary_selector_selected_item_get(obj);

	/* Get set text for the item */
	main_text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	sub_text = eext_rotary_selector_item_part_text_get(item, "selector,sub_text");

	dlog_print(DLOG_INFO, LOG_TAG,"Item Clicked!, Currently Selected %s, %s\n", main_text, sub_text);
	ad->master.res = get_res_by_name(sub_text);
	if(!ad->master.res)return;
	dlog_print(DLOG_INFO, LOG_TAG,"Currently Selected Resource = %s, %d\n", ad->master.res->name, ad->master.res->restype);

	double start = 0;
	double end = 0;
	double incr = 0;


	switch(ad->master.res->restype)
	{
		case TVPOWER:
		case RVCPOWER:
		case ACPOWER:
			dlog_print(DLOG_INFO, LOG_TAG, "%s Resource ", ad->master.res->name);
			show_power_screen(ad);
			break;
		case VOLMUTE:
			show_spinner(ad, "units", 0, 100, 1, ad->master.res->name);
			break;
		case FHCCT:
		case FHCDT:
			show_spinner(ad, "deg. F", 0, 100, 1, ad->master.res->name);
			break;
		case FHFCT:
		case FHFDT:
			show_spinner(ad, "deg. F", 0, 100, 1, ad->master.res->name);
			break;
		case FHCVCT:
		case FHCVDT:
			show_spinner(ad, "deg. F", 0, 100, 1, ad->master.res->name);
			break;

		case PICMODE:
		case SOUNDMODE:
		case SOURCE:
			//show_text(ad, "", 0, 100, ad->master.res->name);
			break;

		case FHDCV:
		case FHDF:
		case FHDC:

		case FHFREEZER:
		case ACMODE:
		case RVCMODE:
		case FHMODE:
		case AIRFLOW:
		case ACTEMP:
		case ACSENSORS:

		default:
			break;
	}
}

void
_item_create(Evas_Object *rotary_selector, void *data)
{
	Evas_Object *image;
	Eext_Object_Item * item;
	char buf[255] = {0};
	int i, j;
	int startindex;
	appdata_s *ad = (appdata_s *)data;

	dlog_print(DLOG_INFO, LOG_TAG, "_item_create ad %p",ad );


	int ROTARY_SELECTOR_PAGE_ITEM_COUNT = 1;
	switch(ad->master.res->devtype)
	{
		case TV:
			ROTARY_SELECTOR_PAGE_ITEM_COUNT = ad->master.data.tv.count;
			startindex = TVPOWER;
			break;
		case FAMILYHUB:
			ROTARY_SELECTOR_PAGE_ITEM_COUNT = ad->master.data.fh.count;
			startindex = FHDCV;
			break;
		case AC:
			ROTARY_SELECTOR_PAGE_ITEM_COUNT = ad->master.data.ac.count;
			startindex = ACPOWER;
			break;
		case RVC:
			ROTARY_SELECTOR_PAGE_ITEM_COUNT = ad->master.data.rvc.count;
			startindex = RVCPOWER;
			break;
		case SURFACE:
			ROTARY_SELECTOR_PAGE_ITEM_COUNT = ad->master.data.surface.count;
			startindex = MSP4;
			break;
		default:
			return;
	}
	char *devname = get_device_name(ad->master.res->devtype);

	for (i = 0; i < 1; i++) //only one page of resources to fix for CES
	{
		for (j = 0; j < ROTARY_SELECTOR_PAGE_ITEM_COUNT; j++)
		{
			item = eext_rotary_selector_item_append(rotary_selector);
			image = elm_image_add(rotary_selector);


			lr_resources_s *lrres = get_res_by_restype((ocfrestype_e)(startindex + j));
			elm_image_file_set(image, lrres->icon, NULL);
			eext_rotary_selector_item_part_content_set(item,
																	 "item,icon",
																	 EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL,
																	 image);

			snprintf(buf, sizeof(buf), "%s", devname);
			eext_rotary_selector_item_part_text_set(item, "selector,main_text", buf);

			snprintf(buf, sizeof(buf), "%s", lrres->name);
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
	_item_create(rotary_selector, ad);


	/* Add smart callback */
	evas_object_smart_callback_add(rotary_selector, "item,selected", _item_selected_cb, ad);
	evas_object_smart_callback_add(rotary_selector, "item,clicked", _item_clicked_cb, ad);

	nf_it = elm_naviframe_item_push(nf, _("Rotary Selector"), NULL, NULL, rotary_selector, "empty");

	findDeviceResource(&ad->master);
	char buf[100];
	snprintf(buf, 100, "Discovering %s", get_device_name(ad->master.res->devtype));
	show_graphic_popup(ad, buf, 5);
}
