/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

#include <tizen.h>
#include <dlog.h>
#include <app.h>
#include <efl_extension.h>
#include <Elementary.h>
#include "w3cocf.h"
#include "view.h"

static void _win_delete_request_cb(void *data, Evas_Object *obj, void *event_info);
static void _rectangle_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _rectangle_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _rectangle_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _dialer_text_resize(Evas_Object *entry);

static int toggle = 0;


static const char *main_menu_names[] = {
	"Vehicle Dash", "Wheels",
	NULL
};

static const char *images[] = {

	ICON_DIR"/lb%d.png",
	ICON_DIR"/battery%d.png",
	ICON_DIR"/speed%d.png",
	ICON_DIR"/fuel%d.png",
	ICON_DIR"/psi%d.png",
	ICON_DIR"/gauge%d.png",
	ICON_DIR"/temp%d.png",
	ICON_DIR"/psi%d.png",
	ICON_DIR"/gauge%d.png",
	ICON_DIR"/temp%d.png",
	ICON_DIR"/psi%d.png",
	ICON_DIR"/gauge%d.png",
	ICON_DIR"/temp%d.png",
	ICON_DIR"/psi%d.png",
	ICON_DIR"/gauge%d.png",
	ICON_DIR"/temp%d.png",
	ICON_DIR"/fuel%d.png",
	ICON_DIR"/temp%d.png",
	ICON_DIR"/park%d.png",
	ICON_DIR"/pause%d.png",
	ICON_DIR"/dialer_button_bg.png",
	ICON_DIR"/w3c.png",
	ICON_DIR"/car%d.png",
	NULL
};

typedef struct _item_data
{
	int index;
	Elm_Object_Item *item;
} item_data;


static void win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "naviframe_pop_cb is called");

	appdata_s *ad = (appdata_s *)data;
	dlog_print(DLOG_INFO, LOG_TAG,  "Top Item %p - %p\n", ad->nf_it, elm_naviframe_top_item_get(ad->nf));
	if(ad->nf_it == elm_naviframe_top_item_get(ad->nf)){
		dlog_print(DLOG_INFO, LOG_TAG,  "Exiting The App");
		ui_app_exit();
	}
	else{
		elm_naviframe_item_pop_to(ad->nf_it);
		//elm_naviframe_item_promote(ad->nf_it);
		elm_naviframe_item_pop_to(ad->nf_it);
		evas_object_del(ad->layout);
		evas_object_show(ad->mainlayout);
		evas_object_show(ad->genlist);
		ad->choice = -1;
	}
}


void dash_cb(void *data, Evas_Object *obj, void *event_info){

	appdata_s *ad = (appdata_s *)data;

	char file_path[PATH_MAX];

	Elm_Object_Item *it = (Elm_Object_Item*) event_info;
	const char *choice = elm_object_item_part_text_get(it, "elm.text");
	dlog_print(DLOG_INFO, LOG_TAG, "Printing default part text %s", choice);
	dlog_print(DLOG_INFO, LOG_TAG, "-----------------------------");
	if(!strcmp(choice, "Vehicle Dash")){
		sprintf(file_path, "%s%s", RES_DIR, EDJ_DASH_FILE);
		ad->choice = 0;
	}else{
		sprintf(file_path, "%s%s", RES_DIR, EDJ_WHEEL_FILE);
		ad->choice = 1;
	}

	//if(ad->layout)evas_object_del(ad->layout);
	ad->layout = elm_layout_add(ad->nf);
	dlog_print(DLOG_DEBUG, LOG_TAG, "Layout added %p", ad->layout);
	elm_layout_file_set(ad->layout, file_path, GRP_MAIN);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(ad->layout, EEXT_CALLBACK_BACK, win_back_cb, data);
	evas_object_show(ad->layout);
	dlog_print(DLOG_DEBUG, LOG_TAG, "Dialer created %s.", file_path);

	view_set_image(ad->layout, "sw.button.bg", images[20]);
	view_set_color(ad->layout, "sw.button.bg", 8, 36, 61, 255);

	view_set_image(ad->layout, "sw.button.call", images[21]);
	view_set_color(ad->layout, "sw.button.call", 100, 214, 146, 255);

	sprintf(file_path, images[22], 0);
	view_set_image(ad->layout, "sw.button.delete", file_path);
	view_set_color(ad->layout, "sw.button.delete", 255, 255, 255, 255);

	view_dialer_set_entry(data, "sw.entry.dial");
	view_set_color(ad->layout, "sw.entry.dial", 250, 250, 250, 255);


	Evas_Object *rect = evas_object_rectangle_add(evas_object_evas_get(ad->layout));
	evas_object_color_set(rect, 255, 255, 255, 0);
	evas_object_repeat_events_set(rect, EINA_TRUE);
	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, rect);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, _rectangle_mouse_down_cb, data);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, _rectangle_mouse_up_cb, data);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, _rectangle_mouse_move_cb, data);
	evas_object_show(rect);



	dlog_print(DLOG_INFO, LOG_TAG,  "Top Item Before Dial %p - %p\n", ad->nf_it, elm_naviframe_top_item_get(ad->nf));
	it = elm_naviframe_item_push(ad->nf, "Dial", NULL, NULL, ad->layout, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
	dlog_print(DLOG_INFO, LOG_TAG,  "Top Item After Dial %p - %p\n", it, ad->nf_it);

}

static void _win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

void view_destroy(void *data)
{
	appdata_s *ad = (appdata_s *)data;

	if (ad->win == NULL) {
		return;
	}

	evas_object_del(ad->win);
}

void view_set_image(Evas_Object *parent, const char *part_name, const char *image_path)
{
	Evas_Object *image = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	image = elm_object_part_content_get(parent, part_name);
	if (image == NULL) {
		image = elm_image_add(parent);
		if (image == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "failed to create an image object.");
			return;
		}
		elm_object_part_content_set(parent, part_name, image);
	}

	dlog_print(DLOG_ERROR, LOG_TAG, "Set image to %s", image_path);
	if (EINA_FALSE == elm_image_file_set(image, image_path, NULL)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set image. %s", image_path);
		return;
	}
	evas_object_show(image);

	return;
}

void view_set_color(Evas_Object *parent, const char *part_name, int r, int g, int b, int a)
{
	Evas_Object *obj = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	obj = elm_object_part_content_get(parent, part_name);
	if (obj == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get parent.");
		return;
	}

	/* Set color of target part object */
	evas_object_color_set(obj, r, g, b, a);
}

void view_set_button(Evas_Object *parent, const char *part_name, const char *style, const char *image_path, const char *text,
		Evas_Object_Event_Cb down_cb, Evas_Object_Event_Cb up_cb, Evas_Smart_Cb clicked_cb, void *user_data)
{
	Evas_Object *btn = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return;
	}

	btn = elm_button_add(parent);
	if (btn == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create button.");
		return;
	}

	if (style)
		elm_object_style_set(btn, style);

	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(parent, part_name, btn);

	if (text)
		elm_object_text_set(btn, text);

	if (image_path)
		view_set_image(btn, NULL, image_path);

	if (down_cb)
		evas_object_event_callback_add(btn , EVAS_CALLBACK_MOUSE_DOWN, down_cb, user_data);
	if (up_cb)
		evas_object_event_callback_add(btn, EVAS_CALLBACK_MOUSE_UP, up_cb, user_data);
	if (clicked_cb)
		evas_object_smart_callback_add(btn, "clicked", clicked_cb, user_data);

	evas_object_show(btn);
}


void view_dialer_set_entry(void *data, const char *part_name)
{
	appdata_s *ad = (appdata_s *)data;
	ad->entry = elm_entry_add(ad->layout);
	evas_object_size_hint_weight_set(ad->entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_entry_single_line_set(ad->entry, EINA_TRUE);
	elm_entry_scrollable_set(ad->entry, EINA_TRUE);
	elm_entry_input_panel_enabled_set(ad->entry, EINA_FALSE);
	elm_entry_editable_set(ad->entry, EINA_FALSE);
	elm_entry_context_menu_disabled_set(ad->entry, EINA_TRUE);
	elm_entry_text_style_user_push(ad->entry, DIAL_TEXT_STYLE_NORMAL);
	elm_object_part_content_set(ad->layout, part_name, ad->entry);
	elm_entry_entry_set(ad->entry, "Tap to Check");
//	if (clicked_cb) {
//		evas_object_smart_callback_add(ad->entry, "clicked", clicked_cb, data);
//	}
}

int view_dialer_set_entry_text(void *data, int operation, const char *text)
{
	appdata_s *ad = (appdata_s *)data;

	if (ad->entry == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Entry object is NULL.");
		return -1;
	}

	//if (operation == ENTRY_TEXT_CLEAR_ALL)
	//	elm_object_text_set(ad->entry, "");
	if (operation == ENTRY_TEXT_ADD_TEXT) {
		int num = atoi(text);
		int val = 0;
		switch(num){
			case 0:val = (ad->choice==0)?0:17;	break;
			case 1:val = (ad->choice==0)?5:4;	break;
			case 2:val = (ad->choice==0)?1:6;	break;
			case 3:val = (ad->choice==0)?2:7;	break;
			case 4:val = (ad->choice==0)?11:9;	break;
			case 5:val = (ad->choice==0)?3:18;	break;
			case 6:val = (ad->choice==0)?14:10;	break;
			case 7:val = (ad->choice==0)?16:12;	break;
			case 8:val = (ad->choice==0)?19:13;	break;
			case 9:val = (ad->choice==0)?8:15;	break;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "pressed %d. Value %d", num, val);
		elm_entry_entry_set(ad->entry, get_signal_value(val, true));

		char buf[PATH_MAX];
		tinnos_signal_s *t = get_tinnos_signal(val);

		switch(val){

			case Signal_Body_Lights_IsLowBeamOn:
			case Signal_Body_Lights_IsParkingOn:
			case Signal_Drivetrain_InternalCombustionEngine_EOP:
			case Signal_Drivetrain_BatteryManagement_BatteryStatus:
			case Signal_Drivetrain_InternalCombustionEngine_OilGauge:
			case Signal_Chassis_Axle_Row1_Wheel_Right_Tire_PressureLow:
			case Signal_Chassis_Axle_Row1_Wheel_Left_Tire_PressureLow:
			case Signal_Chassis_Axle_Row2_Wheel_Right_Tire_PressureLow:
			case Signal_Chassis_Axle_Row2_Wheel_Left_Tire_PressureLow:
				sprintf(buf, images[val], t->b);
				break;
			case Signal_Drivetrain_Transmission_Speed:
				if(t->i>60)
					sprintf(buf, images[val], 1);
				else
					sprintf(buf, images[val], 0);
				break;
			case Signal_Chassis_Axle_Row1_Wheel_Right_Tire_Pressure:
			case Signal_Chassis_Axle_Row1_Wheel_Left_Tire_Pressure:
			case Signal_Chassis_Axle_Row2_Wheel_Right_Tire_Pressure:
			case Signal_Chassis_Axle_Row2_Wheel_Left_Tire_Pressure:
				if(t->d>20)
					sprintf(buf, images[val], 1);
				else
					sprintf(buf, images[val], 0);
				break;
			case Signal_Chassis_Axle_Row1_Wheel_Right_Tire_Temperature:
			case Signal_Chassis_Axle_Row1_Wheel_Left_Tire_Temperature:
			case Signal_Chassis_Axle_Row2_Wheel_Right_Tire_Temperature:
			case Signal_Chassis_Axle_Row2_Wheel_Left_Tire_Temperature:
				if(t->d>10)
					sprintf(buf, images[val], 1);
				else
					sprintf(buf, images[val], 0);
				break;
			case Signal_Drivetrain_FuelSystem_Level:
				if(t->i>6)
					sprintf(buf, images[val], 1);
				else
					sprintf(buf, images[val], 0);
				break;

			case Signal_Body_Outside_Temperature:
				if(t->d>20)
					sprintf(buf, images[val], 1);
				else
					sprintf(buf, images[val], 0);
				break;
			default:
				break;

		}

		dlog_print(DLOG_ERROR, LOG_TAG, "Setting icon to %s", buf);

		view_set_image(ad->layout, "sw.button.delete", buf);
		view_set_color(ad->layout, "sw.button.delete", 255, 255, 255, 255);
		toggle = 3;
	} else {
		dlog_print(DLOG_ERROR, LOG_TAG, "view_set_entry_text text operation is invalid!");
		return -1;
	}

	/* Resize Dialer entry text */
	_dialer_text_resize(ad->entry);
	return 1;
}

static int _get_btn_dial_number(int evt_x, int evt_y)
{
	int result = -1;

	/* Calculation x and y from CENTER_REF point */
	int x = evt_x - CENTER_REF_X;
	int y = CENTER_REF_Y - evt_y;

	/* Calculation the slope and radius from CENTER_REF point */
	float slope = (float)y / (float)x;
	float radius = sqrt(x*x + y*y);

	if (radius > RADIUS_MIN) {
		if (x == 0) {
			if (y >= 0)
				result = 0;
			else
				result = 5;
		} else if (y == 0) {
			if (x >= 0)
				result = 2;
			else
				result = 8;
		} else {
			if (slope > SLOPE_72_DEGREE) {
				if (y > 0)
					result = 0;
				else
					result = 5;
			} else if (slope > SLOPE_36_DEGREE) {
				if (y > 0)
					result = 1;
				else
					result = 6;
			} else if (slope > SLOPE_180_DEGREE) {
				if (y > 0)
					result = 2;
				else
					result = 7;
			} else if (slope > SLOPE_144_DEGREE) {
				if (y > 0)
					result = 8;
				else
					result = 3;
			} else if (slope > SLOPE_108_DEGREE) {
				if (y > 0)
					result = 9;
				else
					result = 4;
			} else {
				if (y > 0)
					result = 0;
				else
					result = 5;
			}
		}
	} else {
		dlog_print(DLOG_DEBUG, LOG_TAG, "Ignore touch event under min radius");
	}

	return result;
}



static void _rectangle_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;

	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;

	/* Ignore unmatched touch event with mouse down event */
	if (ad->mouse_down_dial_num == -1)
		return;

	int temp_move_dial = _get_btn_dial_number(ev->output.x, ev->output.y);

	if (ad->mouse_down_dial_num != temp_move_dial)
		ad->mouse_down_dial_num = -1;

	//dlog_print(DLOG_DEBUG, LOG_TAG, "_mouse_move_cb is called down[%d]", ad->mouse_down_dial_num);
}

static void _rectangle_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;
	ad->mouse_down_dial_num = _get_btn_dial_number(ev->output.x, ev->output.y);

	if (ad->mouse_down_dial_num != -1) {
		char signal[9] = {0, };

		/* Trigger touch animation */
		snprintf(signal, sizeof(signal), "%s%d", "button.", ad->mouse_down_dial_num);
		elm_layout_signal_emit(ad->layout, "button.dial.touch", signal);
		dlog_print(DLOG_DEBUG, LOG_TAG, "Timer SET 0x%x", ad->timer);
	}
}

static void _rectangle_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	if (ad->timer != NULL) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "Timer DELETE 0x%x - mouse up", ad->timer);
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}

	/* Ignore unmatched touch event with mouse down event */
	if (ad->mouse_down_dial_num == -1)
		return;

	/* Set new Entry text */
	char new_dial[2] = { 0, };
	snprintf(new_dial, sizeof(new_dial), "%d", ad->mouse_down_dial_num);
	dlog_print(DLOG_DEBUG, LOG_TAG, "Number Pressed %s", new_dial);
	view_dialer_set_entry_text(ad, ENTRY_TEXT_ADD_TEXT, new_dial);

	/* Initialize event records */
	ad->mouse_down_dial_num = -1;
}


/*
 * @brief Resize dialer entry text for long text
 * @param[in] entry Entry object to be modified by this function
 */
static void _dialer_text_resize(Evas_Object *entry)
{
	const char *dial_entry = elm_entry_entry_get(entry);
	int text_length = strlen(dial_entry);

	/* Change Entry text style and size according to length */
	if (text_length < 12)
		elm_entry_text_style_user_push(entry, DIAL_TEXT_STYLE_NORMAL);
	else if (text_length < 15)
		elm_entry_text_style_user_push(entry, DIAL_TEXT_STYLE_SMALL);
	else
		elm_entry_text_style_user_push(entry, DIAL_TEXT_STYLE_SMALLER);

	/* Set entry cursor to end for displaying last updated entry text*/
	elm_entry_cursor_end_set(entry);
}


void refresh_dial(appdata_s *ad){

	if(ad->choice<0)
		return;


	Evas_Object *edje = elm_layout_edje_get(ad->layout);
	dlog_print(DLOG_DEBUG, LOG_TAG, "refreshing . edje = %p layout %p choice %d", edje, ad->layout, ad->choice);

	if(ad->choice==0){
		edje_object_part_text_set(edje, "button.0.digit",	get_signal_value(0, false));
		edje_object_part_text_set(edje, "button.0.txt",		get_signal_name(0));
		edje_object_part_text_set(edje, "button.1.digit",	get_signal_value(5, false));
		edje_object_part_text_set(edje, "button.1.txt",		get_signal_name(5));
		edje_object_part_text_set(edje, "button.2.digit",	get_signal_value(1, false));
		edje_object_part_text_set(edje, "button.2.txt",		get_signal_name(1));
		edje_object_part_text_set(edje, "button.3.digit",	get_signal_value(2, false));
		edje_object_part_text_set(edje, "button.3.txt",		get_signal_name(2));
		edje_object_part_text_set(edje, "button.4.digit",	get_signal_value(11, false));
		edje_object_part_text_set(edje, "button.4.txt",		get_signal_name(11));
		edje_object_part_text_set(edje, "button.5.digit",	get_signal_value(3, false));
		edje_object_part_text_set(edje, "button.5.txt",		get_signal_name(3));
		edje_object_part_text_set(edje, "button.6.digit",	get_signal_value(14, false));
		edje_object_part_text_set(edje, "button.6.txt",		get_signal_name(14));
		edje_object_part_text_set(edje, "button.7.digit",	get_signal_value(16, false));
		edje_object_part_text_set(edje, "button.7.txt",		get_signal_name(16));
		edje_object_part_text_set(edje, "button.8.digit",	get_signal_value(19, false));
		edje_object_part_text_set(edje, "button.8.txt",		get_signal_name(19));
		edje_object_part_text_set(edje, "button.9.digit",	get_signal_value(8, false));
		edje_object_part_text_set(edje, "button.9.txt",		get_signal_name(8));
	}
	if(ad->choice==1){
		edje_object_part_text_set(edje, "button.0.digit",	get_signal_value(17, false));
		edje_object_part_text_set(edje, "button.0.txt",		get_signal_name(17));
		edje_object_part_text_set(edje, "button.1.digit",	get_signal_value(4, false));
		edje_object_part_text_set(edje, "button.1.txt",		get_signal_name(4));
		edje_object_part_text_set(edje, "button.2.digit",	get_signal_value(6, false));
		edje_object_part_text_set(edje, "button.2.txt",		get_signal_name(6));
		edje_object_part_text_set(edje, "button.3.digit",	get_signal_value(7, false));
		edje_object_part_text_set(edje, "button.3.txt",		get_signal_name(7));
		edje_object_part_text_set(edje, "button.4.digit",	get_signal_value(9, false));
		edje_object_part_text_set(edje, "button.4.txt",		get_signal_name(9));
		edje_object_part_text_set(edje, "button.5.digit",	get_signal_value(18, false));
		edje_object_part_text_set(edje, "button.5.txt",		get_signal_name(18));
		edje_object_part_text_set(edje, "button.6.digit",	get_signal_value(10, false));
		edje_object_part_text_set(edje, "button.6.txt",		get_signal_name(10));
		edje_object_part_text_set(edje, "button.7.digit",	get_signal_value(12, false));
		edje_object_part_text_set(edje, "button.7.txt",		get_signal_name(12));
		edje_object_part_text_set(edje, "button.8.digit",	get_signal_value(13, false));
		edje_object_part_text_set(edje, "button.8.txt",		get_signal_name(13));
		edje_object_part_text_set(edje, "button.9.digit",	get_signal_value(15, false));
		edje_object_part_text_set(edje, "button.9.txt",		get_signal_name(15));
	}

	char file_path[PATH_MAX];
	if(toggle)toggle--;
	else toggle = 1;
	if(toggle<=1){
		sprintf(file_path, images[22], toggle);
		view_set_image(ad->layout, "sw.button.delete", file_path);
		view_set_color(ad->layout, "sw.button.delete", 255, 255, 255, 255);
	}

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
	snprintf(buf, 1023, "%s", "W3C+OCF@CES'17");
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

static void
_gl_menu_del(void *data, Evas_Object *obj)
{
	// FIXME: Unrealized callback can be called after this.
	// Accessing Item_Data can be dangerous on unrealized callback.
	item_data *id = (item_data *)data;
	if (id) free(id);
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

	/* Genlist */
	ad->genlist = elm_genlist_add(nf);
	genlist = ad->genlist;
	ad->layout= NULL;
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "selected", gl_selected_cb, NULL);

	circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);

	/* Genlist Title Item style */
	ttc->item_style = "title";
	ttc->func.text_get = _gl_menu_title_text_get;
	ttc->func.del = _gl_menu_del;

	/* Genlist Item style */
	itc->item_style = "default";
	itc->func.text_get = _gl_menu_text_get;
	itc->func.del = _gl_menu_del;

	/* Genlist Padding Item style */
	ptc->item_style = "padding";
	ptc->func.del = _gl_menu_del;

	/* Title Items Here */
	elm_genlist_item_append(genlist, ttc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	/* Main Menu Items Here */
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;

	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, dash_cb, ad);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, dash_cb, ad);

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	/* This button is set for devices which doesn't have H/W back key. */
	btn = elm_button_add(nf);
	elm_object_style_set(btn, "naviframe/end_btn/default");
	ad->nf_it = elm_naviframe_item_push(nf, NULL, btn, NULL, genlist, "empty");
	dlog_print(DLOG_INFO, LOG_TAG,  "Top Item After List %p - %p\n", ad->nf_it, elm_naviframe_top_item_get(ad->nf));
	//elm_naviframe_item_title_enabled_set(ad->nf_it, EINA_FALSE, EINA_FALSE);
	//elm_naviframe_item_pop_cb_set(ad->nf_it, naviframe_pop_cb, ad->win);
}

void
create_base_gui(void *data)
{
	appdata_s *ad = (appdata_s *)data;

	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ad->win, EINA_TRUE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", _win_delete_request_cb, NULL);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	// Eext Circle Surface Creation
	ad->circle_surface = eext_circle_surface_conformant_add(ad->conform);

	/* Indicator */
	/* elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW); */

	/* Base Layout */
	ad->mainlayout = elm_layout_add(ad->conform);
	evas_object_size_hint_weight_set(ad->mainlayout , EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->mainlayout , "layout", "application", "default");
	evas_object_show(ad->mainlayout );

	elm_object_content_set(ad->conform, ad->mainlayout );

	/* Naviframe */
	ad->nf = elm_naviframe_add(ad->mainlayout);
	create_main_view(ad);
	elm_object_part_content_set(ad->mainlayout , "elm.swallow.content", ad->nf);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, win_back_cb, NULL);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}
