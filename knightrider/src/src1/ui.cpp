/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *		  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "geniviocf.h"

#define NUM_ITEMS             3
#define NUM_INDEX             3
#define NUM_ITEMS_CIRCLE_EVEN 6
#define NUM_INDEX_CIRCLE_EVEN 6


const char *main_menu_names[] = {
	"Select User", "Scenes", "Gestures",
	NULL
};

typedef struct _item_data
{
	int index;
	Elm_Object_Item *item;
} item_data;

typedef struct _page_data page_data;
struct _page_data
{
	Evas_Object *main_layout;
	Evas_Object *scroller;
	Evas_Object *box;
	Evas_Object *mapbuf[NUM_ITEMS_CIRCLE_EVEN];
	Evas_Object *index;
	Evas_Object *page_layout[NUM_ITEMS_CIRCLE_EVEN];
	Evas_Object *left_right_rect;
	int cur_page;
	int prev_page;
	Elm_Object_Item *it[NUM_ITEMS_CIRCLE_EVEN];

	Elm_Object_Item *last_it;
	Elm_Object_Item *new_it;
	int min_page, max_page;
};

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	dlog_print(DLOG_INFO, LOG_TAG,  "Top Item %p - %p\n", ad->nf_it, elm_naviframe_top_item_get(ad->nf));
	if(ad->nf_it == elm_naviframe_top_item_get(ad->nf)){
		dlog_print(DLOG_INFO, LOG_TAG,  "Exiting The App");
		ui_app_exit();
	}else if(ad->gestit == elm_naviframe_top_item_get(ad->nf)){
		elm_naviframe_item_pop(ad->nf);
		ad->gestit = NULL;
		// unregister gesture events
		ad->master.gesture = TAP1;
		gesture_callbacks_set(ad, false);
		ad->master.gesture = TAP2;
		gesture_callbacks_set(ad, false);
		ad->master.gesture = TAP3;
		gesture_callbacks_set(ad, false);
		ad->master.gesture = LINELEFT;
		gesture_callbacks_set(ad, false);
	}
	else{
		elm_naviframe_item_pop_to(ad->nf_it);
		ad->nf_it = elm_naviframe_top_item_get(ad->nf);
		// unregister gesture events
		ad->master.gesture = TAP1;
		gesture_callbacks_set(ad, false);
		ad->master.gesture = TAP2;
		gesture_callbacks_set(ad, false);
		ad->master.gesture = TAP3;
		gesture_callbacks_set(ad, false);
		ad->master.gesture = LINELEFT;
		gesture_callbacks_set(ad, false);
	}
}

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

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
	snprintf(buf, 1023, "%s", "OCFAuto@CES'17");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, 1023, "%s", main_menu_names[index]);
		return strdup(buf);
	}
	return NULL;
}

static Evas_Object*
_gl_icon_get(void *data, Evas_Object *obj, const char *part)
{
	item_data *id = (item_data *)data;
	Evas_Object *content = NULL;

	if (strcmp(part, "elm.icon")) return NULL;

	content = elm_image_add(obj);
	int index = id->index;
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s/u%d.png",ICON_DIR, index);
	elm_image_file_set(content, buf, NULL);
	evas_object_show(content);

	return content;
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
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
	ui_app_exit();
	return EINA_FALSE;
}

static void destroy_popup(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	if(!ad->popup) return;
	elm_popup_dismiss(ad->popup);
	if(ad->popup)evas_object_del(ad->popup);
	if(ad->image)evas_object_del(ad->image);
	ad->popup = NULL;
	ad->image = NULL;
	dlog_print(DLOG_INFO, LOG_TAG, "Step 1 Completed.");
}


void show_graphic_popup(appdata_s *ad, char *text, char *img, int timeout){

	ad->popup = elm_popup_add(ad->win);
	elm_object_style_set(ad->popup, "toast/circle");
	elm_popup_orient_set(ad->popup, ELM_POPUP_ORIENT_CENTER);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	ad->image = elm_image_add(ad->popup);
	elm_object_part_text_set(ad->popup,"elm.text", text);
	elm_image_file_set(ad->image, img, NULL);
	elm_object_part_content_set(ad->popup, "toast,icon", ad->image);
	evas_object_show(ad->image);

	if(timeout>0){
		elm_popup_timeout_set(ad->popup, timeout);
		evas_object_smart_callback_add(ad->popup, "timeout", destroy_popup, ad);
	}else{
		evas_object_smart_callback_del(ad->popup, "timeout", destroy_popup);
	}
	evas_object_show(ad->popup);


#if 0
	Evas_Object *layout, *progressbar;
	ad->popup = elm_popup_add(ad->win);
	elm_object_style_set(ad->popup, "circle");
	layout = elm_layout_add(ad->popup);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_progressbar");
	elm_object_content_set(ad->popup, layout);
	elm_object_part_text_set(layout,"elm.text", text);
	evas_object_show(layout);

	progressbar = elm_progressbar_add(layout);
	elm_object_style_set(progressbar, "process/popup/small");
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_part_content_set(layout, "elm.swallow.content", progressbar);
	evas_object_show(progressbar);

	evas_object_show(ad->popup);

	if(timeout>0){
		elm_popup_timeout_set(ad->popup, timeout);
		evas_object_smart_callback_add(ad->popup, "timeout", destroy_popup, ad);
	}else{
		evas_object_smart_callback_del(ad->popup, "timeout", destroy_popup);
	}
#endif
}

static void
create_main_view(appdata_s *ad)
{
	Evas_Object *genlist;
	Evas_Object *circle_genlist;
	Evas_Object *btn;
	Evas_Object *nf = ad->nf;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	int index = 0;

	genlist = elm_genlist_add(nf);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "selected", gl_selected_cb, NULL);
	itc->func.content_get = _gl_icon_get;
	itc->func.del = _gl_menu_del;

	circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);

	ttc->item_style = "title";
	ttc->func.text_get = _gl_menu_title_text_get;
	ttc->func.del = _gl_menu_del;

	/* Genlist Item style */
	itc->item_style = "default";
	itc->func.text_get = _gl_menu_text_get;
	itc->func.del = _gl_menu_del;

	ptc->item_style = "padding";
	ptc->func.del = _gl_menu_del;

	elm_genlist_item_append(genlist, ttc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, index_cb, ad);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, index_cb, ad);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, index_cb, ad);

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	btn = elm_button_add(nf);
	elm_object_style_set(btn, "naviframe/end_btn/default");
	ad->nf_it = elm_naviframe_item_push(nf, NULL, btn, NULL, genlist, "empty");
	elm_naviframe_item_pop_cb_set(ad->nf_it, naviframe_pop_cb, ad->win);
}

void
create_base_gui(appdata_s *ad)
{
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ad->win, EINA_TRUE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->circle_surface = eext_circle_surface_conformant_add(ad->conform);

	/* Indicator */
	/* elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW); */

	ad->layout = elm_layout_add(ad->conform);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->layout, "layout", "application", "default");
	evas_object_show(ad->layout);

	elm_object_content_set(ad->conform, ad->layout);

	/* Naviframe */
	ad->nf = elm_naviframe_add(ad->layout);
	create_main_view(ad);
	elm_object_part_content_set(ad->layout, "elm.swallow.content", ad->nf);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}
