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

#include "main.h"

#define NUM_ITEMS 20
#define ICON_WIDTH 80
#define ICON_HEIGHT 80

static void _vertical_scroll_cb(void *data, Evas_Object *obj, void *event_info);
static void _horizontal_scroll_cb(void *data, Evas_Object *obj, void *event_info);
static void _vertical_pagecontrol_cb(void *data, Evas_Object *obj, void *event_info);
static void _horizontal_pagecontrol_cb(void *data, Evas_Object *obj, void *event_info);

static struct _menu_item _menu_its[] = {
	{"Vertical Scroll", _vertical_scroll_cb},
	{"Horizontal Scroll", _horizontal_scroll_cb},
	{"Vertical Pagecontrol", _vertical_pagecontrol_cb},
	{"Horizontal Pagecontrol", _horizontal_pagecontrol_cb},
	/* do not delete below */
	{NULL, NULL}
};

static char *icon_its[] = {
	ICON_DIR "/tw_btn_delete_holo_dark.png",
	ICON_DIR "/tw_number_controller_icon_ringtone_mute.png",
	ICON_DIR "/tw_ic_popup_btn_check.png",
	ICON_DIR "/tw_number_controller_icon_alert.png",
	ICON_DIR "/tw_number_controller_icon_bell.png",
	ICON_DIR "/tw_number_controller_icon_ringtone_sound.png"
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

	snprintf(buf, 1023, "%s", "Eext Scroller");
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

static Eina_Bool
_nf_pop_cb(void *data, Elm_Object_Item *it)
{
	if (!data) return EINA_FALSE;
	Evas_Object *scroller = (Evas_Object *)data;
	evas_object_hide(scroller);

	return EINA_TRUE;
}


static void
_vertical_scroll_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = NULL;
	Evas_Object *scroller = NULL;
	Evas_Object *box = NULL;
	Evas_Object *rect = NULL;
	Evas_Object *circle_scroller = NULL;
	Evas_Object *ic = NULL;
	Evas_Coord w, h;
	int i;

	if (!data) return;
	ad = (appdata_s *)data;

	scroller = elm_scroller_add(ad->nf);
	elm_scroller_loop_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_object_scroll_lock_x_set(scroller, EINA_TRUE);
	evas_object_show(scroller);

	circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	box = elm_box_add(scroller);
	elm_object_content_set(scroller, box);
	evas_object_show(box);

	rect = evas_object_rectangle_add(evas_object_evas_get(box));
	evas_object_geometry_get(ad->nf, NULL, NULL, &w, &h);
	evas_object_size_hint_min_set(rect, w, (h - ICON_HEIGHT) / 2);
	elm_box_pack_end(box, rect);

	for (i = 0; i < NUM_ITEMS; ++i) {
		ic = elm_image_add(box);
		elm_image_file_set(ic, icon_its[i % (sizeof(icon_its) / sizeof(char *))], NULL);
		evas_object_size_hint_min_set(ic, w, ICON_HEIGHT);
		evas_object_show(ic);
		elm_box_pack_end(box, ic);
	}

	elm_box_pack_end(box, rect);

	Elm_Object_Item *it = elm_naviframe_item_push(ad->nf, _("Vertical Scroll"), NULL, NULL, scroller, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
	elm_naviframe_item_pop_cb_set(it, _nf_pop_cb, scroller);
}


static void
_horizontal_scroll_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad;
	Evas_Object *scroller = NULL;
	Evas_Object *box = NULL;
	Evas_Object *rect = NULL;
	Evas_Object *circle_scroller = NULL;
	Evas_Object *ic = NULL;
	Evas_Coord w, h;
	int i;

	if (!data) return;
	ad = (appdata_s *)data;

	scroller = elm_scroller_add(ad->nf);
	elm_scroller_loop_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_object_scroll_lock_y_set(scroller, EINA_TRUE);
	evas_object_show(scroller);

	circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	box = elm_box_add(scroller);
	elm_box_horizontal_set(box, EINA_TRUE);
	elm_object_content_set(scroller, box);
	evas_object_show(box);

	rect = evas_object_rectangle_add(evas_object_evas_get(box));
	evas_object_geometry_get(ad->nf, NULL, NULL, &w, &h);
	evas_object_size_hint_min_set(rect, (w - ICON_WIDTH) / 2, h);
	elm_box_pack_end(box, rect);

	for (i = 0; i < NUM_ITEMS; ++i) {
		ic = elm_image_add(box);
		elm_image_file_set(ic, icon_its[i % (sizeof(icon_its) / sizeof(char *))], NULL);
		evas_object_size_hint_min_set(ic, ICON_WIDTH, h);
		evas_object_show(ic);
		elm_box_pack_end(box, ic);
	}

	elm_box_pack_end(box, rect);

	Elm_Object_Item *it = elm_naviframe_item_push(ad->nf, _("Horizontal Scroll"), NULL, NULL, scroller, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
	elm_naviframe_item_pop_cb_set(it, _nf_pop_cb, scroller);
}

static void
_vertical_pagecontrol_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad;
	Evas_Object *scroller = NULL;
	Evas_Object *box = NULL;
	Evas_Object *rect = NULL;
	Evas_Object *circle_scroller = NULL;
	Evas_Object *ic = NULL;
	Evas_Coord w, h;
	int i;

	if (!data) return;
	ad = (appdata_s *)data;

	scroller = elm_scroller_add(ad->nf);
	elm_scroller_loop_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_scroller_page_size_set(scroller, 0, ICON_HEIGHT);
	elm_scroller_page_scroll_limit_set(scroller, 0, 1);
	elm_object_scroll_lock_x_set(scroller, EINA_TRUE);
	evas_object_show(scroller);

	circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	box = elm_box_add(scroller);
	elm_object_content_set(scroller, box);
	evas_object_show(box);

	rect = evas_object_rectangle_add(evas_object_evas_get(box));
	evas_object_geometry_get(ad->nf, NULL, NULL, &w, &h);
	evas_object_size_hint_min_set(rect, w, (h - ICON_HEIGHT) / 2);
	elm_box_pack_end(box, rect);

	for (i = 0; i < NUM_ITEMS; ++i) {
		ic = elm_image_add(box);
		elm_image_file_set(ic, icon_its[i % (sizeof(icon_its) / sizeof(char *))], NULL);
		evas_object_size_hint_min_set(ic, w, ICON_HEIGHT);
		evas_object_show(ic);
		elm_box_pack_end(box, ic);
	}

	elm_box_pack_end(box, rect);

	Elm_Object_Item *it = elm_naviframe_item_push(ad->nf, _("Vertical Page"), NULL, NULL, scroller, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
	elm_naviframe_item_pop_cb_set(it, _nf_pop_cb, scroller);
}


static void
_horizontal_pagecontrol_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad;
	Evas_Object *scroller = NULL;
	Evas_Object *box = NULL;
	Evas_Object *rect = NULL;
	Evas_Object *circle_scroller = NULL;
	Evas_Object *ic = NULL;
	Evas_Coord w, h;
	int i;

	if (!data) return;
	ad = (appdata_s *)data;

	scroller = elm_scroller_add(ad->nf);
	elm_scroller_loop_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_scroller_page_size_set(scroller, ICON_WIDTH, 0);
	elm_scroller_page_scroll_limit_set(scroller, 1, 0);
	elm_object_scroll_lock_y_set(scroller, EINA_TRUE);
	evas_object_show(scroller);

	circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	box = elm_box_add(scroller);
	elm_box_horizontal_set(box, EINA_TRUE);
	elm_object_content_set(scroller, box);
	evas_object_show(box);

	rect = evas_object_rectangle_add(evas_object_evas_get(box));
	evas_object_geometry_get(ad->nf, NULL, NULL, &w, &h);
	evas_object_size_hint_min_set(rect, (w - ICON_WIDTH) / 2, h);
	elm_box_pack_end(box, rect);

	for (i = 0; i < NUM_ITEMS; ++i) {
		ic = elm_image_add(box);
		elm_image_file_set(ic, icon_its[i % (sizeof(icon_its) / sizeof(char *))], NULL);
		evas_object_size_hint_min_set(ic, ICON_WIDTH, h);
		evas_object_show(ic);
		elm_box_pack_end(box, ic);
	}

	elm_box_pack_end(box, rect);

	Elm_Object_Item *it = elm_naviframe_item_push(ad->nf, _("Horizontal Page"), NULL, NULL, scroller, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
	elm_naviframe_item_pop_cb_set(it, _nf_pop_cb, scroller);
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

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	return genlist;
}

void
eext_scroller_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = NULL;
	Evas_Object *genlist = NULL;

	ad = (appdata_s *)data;
	if (!data) return;

	genlist = _create_list_winset(ad);
	elm_naviframe_item_push(ad->nf, _("(Eext) Scroller"), NULL, NULL, genlist, "empty");
}
