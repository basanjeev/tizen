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

char *popup_menu_names[] = {
	"text", "text scrollable", "title text", "text 1button",
	"title text 1button", "title text 2button", "title text check button",
	"Toast", "Graphic Toast", "Process Small",
	NULL
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

	snprintf(buf, 1023, "%s", "Popup");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, 1023, "%s", popup_menu_names[index]);
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

Evas_Object *top = NULL;

typedef struct _Item_Data
{
	int index;
	Evas_Object *genlist;
	Elm_Object_Item *item;
} Item_Data;

typedef struct _Testitem
{
	Elm_Object_Item *item;
	const char *text;
	const char *path;
} Testitem;

typedef struct _spin_date spin_date;
struct _spin_date
{
	Evas_Object *scroller;
	Evas_Object *spinner1;
	Evas_Object *spinner2;
	Evas_Object *layout;

	double posx1, posx2, prev1, prev2;
};

static void _timeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(!obj) return;
	elm_popup_dismiss(obj);
}

static void _response_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(!data) return;
	elm_popup_dismiss(data);
}

static void _block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(!obj) return;
	elm_popup_dismiss(obj);
}

static void
_popup_hide_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(!obj) return;
	elm_popup_dismiss(obj);
}

static void
_popup_hide_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(!obj) return;
	evas_object_del(obj);
}

static void _popup_text_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	Evas_Object *layout;
	appdata_s *ad;

	ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle");

	elm_object_part_text_set(layout, "elm.text", "This has only texts. This is set by object");
	elm_object_content_set(popup, layout);

	evas_object_show(popup);
}

static void _popup_scrollable_text_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	Evas_Object *layout;
	appdata_s *ad;

	ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle");

	elm_object_part_text_set(layout, "elm.text", "This is scrollable popup text."
					"This part is made by adding long text in popup. Popup internally added "
					"scroller to this layout when size of text is greater than total popup "
					"height. This popup does not have buttons.");
	elm_object_content_set(popup, layout);

	evas_object_show(popup);
}

static void _popup_title_text_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	Evas_Object *layout;
	appdata_s *ad;
	ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle");
	elm_object_part_text_set(layout, "elm.text.title", "Title");

	elm_object_part_text_set(layout, "elm.text", "This Popup has title area");
	elm_object_content_set(popup, layout);

	evas_object_show(popup);
}

static void _popup_text_1button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	Evas_Object *btn;
	Evas_Object *layout;
	appdata_s *ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle");

	elm_object_part_text_set(layout, "elm.text", "This is scrollable popup text."
		"This part is made by adding long text in popup. Popup internally added "
		"scroller to this layout when size of text is greater than total popup "
		"height. This has one button in action area and does not have title area");
	elm_object_content_set(popup, layout);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "OK");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	evas_object_show(popup);
}

static void _popup_title_text_1button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	Evas_Object *btn;
	Evas_Object *layout;
	appdata_s *ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons1");
	elm_object_part_text_set(layout, "elm.text.title", "Popup title");

	elm_object_part_text_set(layout, "elm.text", "This is scrollable popup text."
		"This part is made by adding long text in popup. Popup internally added "
		"scroller to this layout when size of text is greater than total popup "
		"height. This has one button in action area and title text in title area");
	elm_object_content_set(popup, layout);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "OK");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	evas_object_show(popup);
}

static void _popup_title_text_2button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	Evas_Object *btn;
	Evas_Object *icon;
	Evas_Object *layout;
	appdata_s *ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");
	elm_object_part_text_set(layout, "elm.text.title", "Popup title");

	elm_object_part_text_set(layout, "elm.text", "This is scrollable popup text."
		"This part is made by adding long text in popup. Popup internally added "
		"scroller to this layout when size of text is greater than total popup "
		"height. This has two buttons in action area and title text in title area");
	elm_object_content_set(popup, layout);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "popup/circle/left");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	icon = elm_image_add(btn);
	elm_image_file_set(icon, ICON_DIR"/tw_ic_popup_btn_delete.png", NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn, "elm.swallow.content", icon);
	evas_object_show(icon);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "popup/circle/right");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(popup, "button2", btn);
	evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	icon = elm_image_add(btn);
	elm_image_file_set(icon, ICON_DIR"/tw_ic_popup_btn_check.png", NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn, "elm.swallow.content", icon);
	evas_object_show(icon);

	evas_object_show(popup);
}

static void _popup_title_text_check_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup, *label, *layout, *layout_inner, *check;
	Evas_Object *btn1,*btn2, *icon;
	appdata_s *ad = (appdata_s *) data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");
	elm_object_part_text_set(layout, "elm.text.title", "Popup title");
	elm_object_content_set(popup, layout);

	layout_inner = elm_layout_add(layout);
	elm_layout_file_set(layout_inner, ELM_DEMO_EDJ, "popup_checkview_internal");
	evas_object_size_hint_weight_set(layout_inner, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "elm.swallow.content", layout_inner);

	label = elm_label_add(layout_inner);
	elm_object_style_set(label, "popup/default");
	elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
	elm_object_text_set(label, "Will be saved, and sound, only on the Gear.");
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout_inner, "label", label);

	check = elm_check_add(popup);
	elm_object_style_set(check, "popup");
	elm_object_text_set(check, "Do not repeat");
	elm_object_part_content_set(layout_inner, "elm.swallow.end", check);
	evas_object_show(check);

	btn1 = elm_button_add(popup);
	elm_object_style_set(btn1, "popup/circle/left");
	elm_object_part_content_set(popup, "button1", btn1);
	evas_object_smart_callback_add(btn1, "clicked", _response_cb, popup);

	icon = elm_image_add(btn1);
	elm_image_file_set(icon, ICON_DIR"/b_option_list_icon_share.png", NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn1, "elm.swallow.content", icon);
	evas_object_show(icon);

	btn2 = elm_button_add(popup);
	elm_object_style_set(btn2, "popup/circle/right");
	elm_object_part_content_set(popup, "button2", btn2);
	evas_object_smart_callback_add(btn2, "clicked", _response_cb, popup);

	icon = elm_image_add(btn2);
	elm_image_file_set(icon, ICON_DIR"/b_option_list_icon_delete.png", NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn2, "elm.swallow.content", icon);
	evas_object_show(icon);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	evas_object_show(popup);
}

static void _popup_toast_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;
	appdata_s *ad = (appdata_s *)data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "toast/circle");
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_BOTTOM);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);
	elm_object_part_text_set(popup,"elm.text", "Saving Contacts to sim on sams, 1 2 3 4 5 6 7 8 9 10.");

	evas_object_smart_callback_add(popup, "block,clicked", _block_clicked_cb, NULL);

	elm_popup_timeout_set(popup, 2.0);
	evas_object_smart_callback_add(popup, "timeout", _timeout_cb, NULL);

	evas_object_show(popup);
}

static void _popup_graphic_toast_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup, *image;
	appdata_s *ad = (appdata_s *)data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "toast/circle");
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);
	elm_object_part_text_set(popup,"elm.text", "Check my device and phone number");

	image = elm_image_add(popup);
	elm_image_file_set(image, ICON_DIR"/b_option_list_icon_action.png", NULL);
	elm_object_part_content_set(popup, "toast,icon", image);

	evas_object_smart_callback_add(popup, "block,clicked", _block_clicked_cb, NULL);

	elm_popup_timeout_set(popup, 2.0);
	evas_object_smart_callback_add(popup, "timeout", _timeout_cb, NULL);

	evas_object_show(popup);
}

static void
_popup_small_process_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup, *layout, *progressbar;
	appdata_s *ad = (appdata_s *)data;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_progressbar");
	elm_object_content_set(popup, layout);
	elm_object_part_text_set(layout,"elm.text", "Description about progress");
	evas_object_show(layout);

	progressbar = elm_progressbar_add(layout);
	elm_object_style_set(progressbar, "process/popup/small");
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_part_content_set(layout, "elm.swallow.content", progressbar);
	evas_object_show(progressbar);

	evas_object_show(popup);
}

void popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	if (ad == NULL) return;

	Evas_Object *genlist;
	Evas_Object *circle_genlist;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	int index = 0;

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

	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_text_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_scrollable_text_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_title_text_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_text_1button_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_title_text_1button_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_title_text_2button_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_title_text_check_button_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_toast_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_graphic_toast_cb, ad);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _popup_small_process_cb, ad);

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	elm_naviframe_item_push(ad->nf, "Popup", NULL, NULL, genlist, "empty");
}
