/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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


#include <langinfo.h>
#include "main.h"

#define FORMAT "%d/%b/%Y%I:%M%p"

static void _datetime_date_cb(void *data, Evas_Object *obj, void *event_info);
static void _datetime_time_cb(void *data, Evas_Object *obj, void *event_info);

static struct _menu_item _menu_its[] = {
	{"Set date", _datetime_date_cb},
	{"Set time", _datetime_time_cb},
	{NULL, NULL}
};

typedef struct _item_data
{
	int index;
	Elm_Object_Item *item;
} item_data;

static void
gl_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(it, EINA_FALSE);
}

static char *
_gl_menu_title_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];

	snprintf(buf, 1023, "%s", "Eext Datetime");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, 1023, "%s", _menu_its[index].name);
		return strdup(buf);
	}
	return NULL;
}

static void
_gl_menu_del(void *data, Evas_Object *obj)
{
	// FIXME: Unrealized callback can be called after this.
	// Accessing Item_Data can be dangerous on unrealized callback.
	item_data *id = (item_data *)data;
	if (id) free(id);
}

static void
_set_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (!data) return;
	Evas_Object *naviframe = (Evas_Object *)data;
	elm_naviframe_item_pop(naviframe);
}

static void
_datetime_date_cb(void *data, Evas_Object * obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *layout = NULL;
	Evas_Object *datetime = NULL;
	Evas_Object *circle_datetime = NULL;
	Evas_Object *btn = NULL;
	Elm_Object_Item *nf_it = NULL;

	layout = elm_layout_add(ad->nf);
	elm_layout_theme_set(layout, "layout", "circle", "datetime");

	datetime = elm_datetime_add(layout);

	circle_datetime = eext_circle_object_datetime_add(datetime, ad->circle_surface);

	eext_rotary_object_event_activated_set(datetime, EINA_TRUE);

	elm_datetime_format_set(datetime, FORMAT);

	elm_object_style_set(datetime, "datepicker/circle");
	elm_object_part_content_set(layout, "elm.swallow.content", datetime);

	btn = elm_button_add(layout);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "OK");
	elm_object_part_content_set(layout, "elm.swallow.btn", btn);
	evas_object_smart_callback_add(btn, "clicked", _set_clicked_cb, ad->nf);

	elm_object_part_text_set(layout, "elm.text", "Set date");

	nf_it = elm_naviframe_item_push(ad->nf, "Date picker", NULL, NULL, layout, NULL);
	elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
}

static void
_datetime_time_cb(void *data, Evas_Object * obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *layout = NULL;
	Evas_Object *datetime = NULL;
	Evas_Object *circle_datetime = NULL;
	Evas_Object *btn = NULL;
	Elm_Object_Item *nf_it = NULL;

	layout = elm_layout_add(ad->nf);
	elm_layout_theme_set(layout, "layout", "circle", "datetime");

	btn = elm_button_add(layout);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "OK");

	elm_object_part_content_set(layout, "elm.swallow.btn", btn);
	evas_object_smart_callback_add(btn, "clicked", _set_clicked_cb, ad->nf);

	elm_object_part_text_set(layout, "elm.text", "Set time");

	datetime = elm_datetime_add(layout);

	circle_datetime = eext_circle_object_datetime_add(datetime, ad->circle_surface);

	eext_rotary_object_event_activated_set(datetime, EINA_TRUE);

	elm_datetime_format_set(datetime, FORMAT);

	elm_object_style_set(datetime, "timepicker/circle");
	elm_object_part_content_set(layout, "elm.swallow.content", datetime);

	nf_it = elm_naviframe_item_push(ad->nf, "Time picker", NULL, NULL, layout, NULL);
	elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
}

static Evas_Object*
_create_list_winset(appdata_s *ad)
{
	Evas_Object *genlist = NULL;
	Evas_Object *circle_genlist;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	struct _menu_item *menu_its = NULL;
	int idx = 0;

	if (!ad) return NULL;

	genlist = elm_genlist_add(ad->nf);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "selected", gl_selected_cb, NULL);

	circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);

	ttc->item_style = "title";
	ttc->func.text_get = _gl_menu_title_text_get;
	ttc->func.del = _gl_menu_del;

	itc->item_style = "default";
	itc->func.text_get = _gl_menu_text_get;
	itc->func.del = _gl_menu_del;

	ptc->item_style = "padding";
	ptc->func.del = _gl_menu_del;

	elm_genlist_item_append(genlist, ttc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	menu_its = _menu_its;
	while (menu_its[idx].name != NULL) {
		id = calloc(sizeof(item_data), 1);
		id->index = idx;
		id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, menu_its[idx].func, ad);
		++idx;
	}

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ptc);

	return genlist;
}

void
eext_datetime_cb(void *data, Evas_Object * obj, void *event_info)
{
	Evas_Object *genlist = NULL;

	appdata_s *ad = (appdata_s *)data;
	if (!ad) return;

	genlist = _create_list_winset(ad);
	elm_naviframe_item_push(ad->nf, "Datetime", NULL, NULL, genlist, "empty");
}
