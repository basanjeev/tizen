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

#include "main.h"

static Evas_Object *result;
static char *user_input;

Evas_Object *getResult(){
    return result;
}

void _list_refresh(void *d, Evas_Object *obj, void *event_info){
	elm_list_go(getResult());
}
void
_popup_close_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	user_input = (char *)elm_entry_entry_get(((appdata_s *)data)->en);
	evas_object_hide(((appdata_s *)data)->popup);
}

void get_user_input(Evas_Object *popup, char *title)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "User Input: %s ", title);
    elm_object_part_text_set(popup, "title,text", title);
    evas_object_show(popup);
}

void _item_cb(void *d, Evas_Object *obj, void *event_info)
{
	return;
}

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static Eina_Bool
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
	ui_app_exit();
	return EINA_FALSE;
}

static void
create_list_view(appdata_s *ad)
{
	Evas_Object *btn;
	Evas_Object *nf = ad->nf;
	Elm_Object_Item *nf_it;
    Evas_Object *panes, *li, *bxx, *bx;

	/* List */
    bx = elm_box_add(nf);
    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

    bxx = elm_box_add(nf);
    evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(bxx, EVAS_HINT_FILL, EVAS_HINT_FILL);

    evas_object_show(bx);
    evas_object_show(bxx);

    panes = elm_panes_add(nf);
    evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(nf, panes);
    elm_panes_horizontal_set(panes, EINA_TRUE);
    evas_object_show(panes);

    result = elm_entry_add(bx);
    elm_entry_scrollable_set(result, EINA_TRUE);
    elm_entry_editable_set(result, EINA_FALSE);
    elm_entry_line_wrap_set(result, ELM_WRAP_MIXED);
    evas_object_size_hint_weight_set(result, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(result, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(bxx, result);
    evas_object_show(result);
    elm_object_part_content_set(panes, "top", bx);
    elm_object_part_content_set(panes, "bottom", bxx);

    li = elm_list_add(bxx);
    elm_list_mode_set(li, ELM_LIST_LIMIT);
    evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(li, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_start(bx, li);
    int size = vinli_get_num_requests();

    for(int i = 0; i<size; i++)
    {
    	int *req = (int *)calloc(1, sizeof(int));
        elm_list_item_append(li, vinli_get_req_name(i), NULL, NULL, _item_cb, req);
    }
    elm_list_go(li);
    evas_object_show(li);

    //user input popup
    Evas_Object *btn1;
    ad->popup = elm_popup_add(bx);
    elm_object_part_text_set(ad->popup, "title,text", "Enter User Input");
    ad->en = elm_entry_add(nf);
    evas_object_size_hint_weight_set(ad->en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(ad->en, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(ad->popup, ad->en);
    btn1 = elm_button_add(ad->popup);
    elm_object_text_set(btn1, "OK");
    elm_object_part_content_set(ad->popup, "button1", btn1);
    evas_object_smart_callback_add(btn1, "clicked", _popup_close_cb, ad);

	/* This button is set for devices which doesn't have H/W back key. */
	btn = elm_button_add(nf);
	elm_object_style_set(btn, "naviframe/end_btn/default");
	nf_it = elm_naviframe_item_push(nf, "Vinli Tizen Mobile", btn, NULL, panes, NULL);
	evas_object_smart_callback_add(btn, "clicked", win_delete_request_cb, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, ad->win);
}

static void
create_base_gui(appdata_s *ad)
{
	/*
	 * Widget Tree
	 * Window
	 *  - conform
	 *   - layout main
	 *    - naviframe */

	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ad->win, EINA_TRUE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win,
			(const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request",
		win_delete_request_cb, NULL);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Indicator */
	/* elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW); */

	/* Base Layout */
	ad->layout = elm_layout_add(ad->conform);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->layout, "layout", "application", "default");
	evas_object_show(ad->layout);

	elm_object_content_set(ad->conform, ad->layout);

	/* Naviframe */
	ad->nf = elm_naviframe_add(ad->layout);
	create_list_view(ad);
	elm_object_part_content_set(ad->layout, "elm.swallow.content", ad->nf);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK,
		eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_MORE,
		eext_naviframe_more_cb, NULL);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
	 * Initialize UI resources and application's data
	 * If this function returns true, the main loop of application starts
	 * If this function returns false, the application is terminated
	 */
	appdata_s *ad = data;

	elm_app_base_scale_set(1.8);
	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
		&locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}


void app_cb(vinli_request_e type, void *value, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "App Callback Called");
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	vinli_init(app_cb, &ad);
	vinli_config_set(VINLI_CLIENT_ID, "689458ac-0600-4294-9b5b-7039a0d35dac");
	vinli_config_set(VINLI_REDIRECT_URL, "http://dummy_client_url");
	vinli_config_set(VINLI_ACCESS_TOKEN, VINLI_ACCESS_TOKEN);


	//ecore_event_handler_add(ECORE_CON_EVENT_URL_PROGRESS, _url_data_cb, NULL);
	//ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, _url_complete_cb, NULL);

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
		APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
		APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(
		&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
		APP_EVENT_DEVICE_ORIENTATION_CHANGED,
		ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
		APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
		APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() failed. err = %d",
			ret);
	}

	vinli_deinit();

	return ret;
}
