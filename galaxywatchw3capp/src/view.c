#include "view.h"
#include "rest.h"
#include "common.h"
#include <dlog.h>
#include <app.h>
#include <stdlib.h>

#define DLOG_PRINT(FMT, ...)   dlog_print(DLOG_DEBUG, LOG_TAG, "[%s %d] : %s", __func__, __LINE__, FMT, ##__VA_ARGS__)

static void _rectangle_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _rectangle_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _rectangle_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _dialer_text_resize(Evas_Object *entry);
void server_changed_cb(const char *key, void *user_data);

static int toggle = 0;
static appdata_s *g_appdata;
static Ecore_Timer *timer = NULL;

static const char *main_menu_names[] = {
	"Vehicle Dash", "Wheels", "Settings",
	NULL
};

static w3c_signal_s cache[Signal_Max] = {
		{0, "Vehicle.Body.Lights.IsLowBeamOn", 									INT, 	false, 	"Beam", 	"%d", 		"N/A", " LowBeam : %d", 	{.i = 0}},
		{1, "Vehicle.Drivetrain.BatteryManagement.LowBatteryLevel",				INT, 	false, 	"Btry", 	"%d", 		"N/A", " Battery : %d",	{.i = 0}},
		{2, "Vehicle.Drivetrain.Transmission.Speed",							INT, 	false, 	"kmph", 	"%d", 		"N/A", " Speed : %d kmph",	{.i = 0}},
		{3, "Vehicle.Drivetrain.InternalCombustionEngine.MaxTorque",			INT, 	false, 	"Oil", 		"%d", 		"N/A", " Torque : %d N.m",	{.i = 0}},
		{4, "Vehicle.Chassis.Axle.Row1.Wheel.Right.Tire.Pressure",				INT, 	false, 	"psi", 		"%d", 		"N/A", " R1.psi : %d",		{.i = 0}},
		{5, "Vehicle.Chassis.Axle.Row1.Wheel.Right.Brake.FluidLevel",			INT, 	false, 	"R1F", 		"%d", 		"N/A", " R1.Fluid %d",	{.i = 0}},
		{6, "Vehicle.Chassis.Axle.Row1.Wheel.Right.Tire.Temperature",			INT,	false, 	"°C", 		"%d", 		"N/A", " R1->°C : %d",		{.i = 0}},
		{7, "Vehicle.Chassis.Axle.Row1.Wheel.Left.Tire.Pressure",				INT, 	false, 	"psi", 		"%d", 		"N/A", " L1.psi : %d",		{.i = 0}},
		{8, "Vehicle.Chassis.Axle.Row1.Wheel.Left.Brake.PadWear",				INT, 	false, 	"L1L", 		"%d", 		"N/A", " L1.PadWear %d",	{.i = 0}},
		{9, "Vehicle.Chassis.Axle.Row1.Wheel.Left.Tire.Temperature",			INT,	false, 	"°C", 		"%d", 		"N/A", " L1.°C %d",		{.i = 0}},
		{10, "Vehicle.Chassis.Axle.Row2.Wheel.Right.Tire.Pressure",				INT, 	false, 	"psi", 		"%d", 		"N/A", " R2.psi %d",		{.i = 0}},
		{11, "Vehicle.Chassis.Axle.Row2.Wheel.Right.Brake.BrakesWorn",			INT, 	false, 	"R2B", 		"%d", 		"N/A", " R2.BrakeWorn %d",	{.i = 0}},
		{12, "Vehicle.Chassis.Axle.Row2.Wheel.Right.Tire.Temperature",			INT,	false, 	"°C", 		"%d", 		"N/A", " R2.°C %d",		{.i = 0}},
		{13, "Vehicle.Chassis.Axle.Row2.Wheel.Left.Tire.Pressure",				INT, 	false, 	"psi", 		"%d", 		"N/A", " L2.psi %d",		{.i = 0}},
		{14, "Vehicle.Chassis.Axle.Row2.Wheel.Left.Brake.FluidLevelLow",		INT, 	false, 	"L2F", 		"%d", 		"N/A", " L2.Fluid : %d",	{.i = 0}},
		{15, "Vehicle.Chassis.Axle.Row2.Wheel.Left.Tire.Temperature",			INT,	false, 	"°C", 		"%d", 		"N/A", " L2<-°C %d",		{.i = 0}},
		{16, "Vehicle.Drivetrain.FuelSystem.Level",								INT, 	false, 	"lts", 		"%d", 		"N/A", " Fuel : %d l" ,		{.i = 0}},
		{17, "Vehicle.Drivetrain.Transmission.TravelledDistance",				INT,	false, 	"km", 		"%d",		"N/A", " Dist. : %d km",		{.i = 0}},
		{18, "Vehicle.Body.Lights.IsParkingOn",									INT, 	false, 	"Park", 	"%d", 		"N/A", " Parking : %d",			{.i = 0}},
		{19, "Vehicle.Drivetrain.InternalCombustionEngine.Engine.EOP",			INT, 	false, 	"EOP", 		"%d", 		"N/A", " Oil PSI : %d ",	{.i = 0}}
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


void save_setting(char *address);

void server_changed_cb(const char *key, void *user_data)
{
	char body[1024];
	strncpy(body, "{\"userid\":\"W3C\",\"vin\":\"JLR01\"}", 1024);
	w3c_request(AUTHENTICATE, "agtserver", body, 0);
}

char *get_signal_name(int i){
	if(i>=0 && i<Signal_Max){
		return cache[i].n;
	}
	else
		return (char *)"N/A";
}

w3c_signal_s *get_w3c_signal(int i){
	if(i>=0 && i<Signal_Max){
		return &cache[i];
	}
	return NULL;
}

static void view_set_image(Evas_Object *parent, const char *part_name, const char *image_path)
{
	Evas_Object *image = NULL;

	if (parent == NULL) {
		DLOG_PRINT("parent is NULL.");
		return;
	}

	image = elm_object_part_content_get(parent, part_name);
	if (image == NULL) {
		image = elm_image_add(parent);
		if (image == NULL) {
			DLOG_PRINT("failed to create an image object.");
			return;
		}
		elm_object_part_content_set(parent, part_name, image);
	}

	DLOG_PRINT("Set image to %s", image_path);
	if (EINA_FALSE == elm_image_file_set(image, image_path, NULL)) {
		DLOG_PRINT("failed to set image. %s", image_path);
		return;
	}
	evas_object_show(image);

	return;
}

static void view_set_color(Evas_Object *parent, const char *part_name, int r, int g, int b, int a)
{
	Evas_Object *obj = NULL;

	if (parent == NULL) {
		DLOG_PRINT("parent is NULL.");
		return;
	}

	obj = elm_object_part_content_get(parent, part_name);
	if (obj == NULL) {
		DLOG_PRINT("failed to get parent.");
		return;
	}

	/* Set color of target part object */
	evas_object_color_set(obj, r, g, b, a);
}

static void refresh_dial(appdata_s *ad, int i){
	if(ad->choice<0)
		return;

	Evas_Object *edje = elm_layout_edje_get(ad->layout);
	DLOG_PRINT("refreshing . choice %d - %d ", ad->mouse_down_dial_num, ad->val);


	int val = -1;

	if(i==0 || i==17)val = 0;
	if(i==5 || i==4)val = 1;
	if(i==1 || i==6)val = 2;
	if(i==2 || i==7)val = 3;
	if(i==9 || i==11)val = 4;
	if(i==3 || i==18)val = 5;
	if(i==14 || i==10)val = 6;
	if(i==16 || i==12)val = 7;
	if(i==19 || i==13)val = 8;
	if(i==8 || i==15)val = 9;

	if(val<0)return;

	char part[40];
	snprintf(part, 40, "button.%d.digit", val);
	edje_object_part_text_set(edje, part, get_signal_value(i, false));
	snprintf(part, 40, "button.%d.txt", val);
	edje_object_part_text_set(edje, "button.0.txt", get_signal_name(i));


	char file_path[PATH_MAX];
	if(toggle)toggle--;
	else toggle = 1;
	if(toggle<=1){
		sprintf(file_path, images[22], toggle);
		view_set_image(ad->layout, "sw.button.delete", file_path);
		view_set_color(ad->layout, "sw.button.delete", 255, 255, 255, 255);
	}

}

static Eina_Bool trigger_auto_update(void *data)
{
	appdata_s *ad = (appdata_s *)data;
	srand (time(NULL));
	int temp = rand()%10;

	if(ad->mouse_down_dial_num==temp){
		ad->mouse_down_dial_num+=1;
		if(ad->mouse_down_dial_num>9)
			ad->mouse_down_dial_num = 0;
	}else{
		ad->mouse_down_dial_num = temp;
	}

	switch(ad->mouse_down_dial_num)
	{
		case 0:ad->val = (ad->choice==0)?0:17;	break;
		case 1:ad->val = (ad->choice==0)?5:4;	break;
		case 2:ad->val = (ad->choice==0)?1:6;	break;
		case 3:ad->val = (ad->choice==0)?2:7;	break;
		case 4:ad->val = (ad->choice==0)?11:9;	break;
		case 5:ad->val = (ad->choice==0)?3:18;	break;
		case 6:ad->val = (ad->choice==0)?14:10;	break;
		case 7:ad->val = (ad->choice==0)?16:12;	break;
		case 8:ad->val = (ad->choice==0)?19:13;	break;
		case 9:ad->val = (ad->choice==0)?8:15;	break;
		default:break;
	}
	DLOG_PRINT("Random Choice[%d] %s", ad->val, cache[ad->val].n);
	_rectangle_mouse_up_cb(ad, NULL, NULL, NULL);
	return EINA_FALSE;
}


void set_signal_value(int i, int value)
{
	if(i>=0 && i<Signal_Max){
		DLOG_PRINT(": Setting %s = %d", cache[i].n, value);
		cache[i].i = value;
		if(g_appdata->nf_it != elm_naviframe_top_item_get(g_appdata->nf)){
			refresh_dial(g_appdata, i);
			if(timer)ecore_timer_del(timer);
			timer = NULL;
			timer = ecore_timer_add(1.0, trigger_auto_update, g_appdata);
		}
	}
}

char *get_signal_value(int i, bool l){
	if(i>=0 && i<Signal_Max){
		DLOG_PRINT("format = %d", l);
		char *ptr = (l)?cache[i].l:cache[i].f;

		if(l)DLOG_PRINT("Signal %s has changed %d format = %s", cache[i].n, cache[i].ch, ptr);
		switch(cache[i].t){
			case BOOL:
				sprintf(cache[i].v, ptr, (cache[i].b)?"On":"Off");
				break;
			case INT:
				sprintf(cache[i].v, ptr, cache[i].i);
				break;
			case DOUBLE:
				sprintf(cache[i].v, ptr, cache[i].d);
				break;
			default:
				break;
		}
		DLOG_PRINT("Getting Signal %s Value = %s", cache[i].n, cache[i].v);
		cache[i].ch = false;
		return cache[i].v;
	}
	return (char *)"N/A";
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	DLOG_PRINT("win_back_cb is called");

	appdata_s *ad = (appdata_s *)data;

	if(ad->nf_it != elm_naviframe_top_item_get(ad->nf)){
		DLOG_PRINT( "2");
		//DLOG_PRINT( "Top Item %p - %p\n", ad->nf_it, elm_naviframe_top_item_get(ad->nf));
		elm_naviframe_item_pop_to(ad->nf_it);
		//elm_naviframe_item_promote(ad->nf_it);
		//elm_naviframe_item_pop_to(ad->nf_it);
		evas_object_del(ad->layout);
		evas_object_show(ad->mainlayout);
		evas_object_show(ad->genlist);
		if(ad->choice==2)
		{
			char address[128];
			snprintf(address, 128, "%s", elm_object_part_text_get(ad->entry, NULL));
			save_setting(address);
		}
		ad->choice = -1;
		if(timer)ecore_timer_del(timer);
		timer = NULL;
	}else{
		DLOG_PRINT( "1");
		ui_app_exit();
	}
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
		DLOG_PRINT("Entry object is NULL.");
		return -1;
	}

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
			default:break;
		}

		ad->val = val;

		w3c_request(GET, cache[val].path, NULL, val);
		elm_entry_entry_set(ad->entry, get_signal_value(val, true));
		DLOG_PRINT("pressed %d. Value %d", num, val);

		char buf[PATH_MAX];
		w3c_signal_s *t = get_w3c_signal(val);
		sprintf(buf, images[val], t->i%2);
		DLOG_PRINT("Setting icon to %s", buf);
		view_set_image(ad->layout, "sw.button.delete", buf);
		view_set_color(ad->layout, "sw.button.delete", 255, 255, 255, 255);
		toggle = 3;
	} else {
		DLOG_PRINT("view_set_entry_text text operation is invalid!");
		return -1;
	}

	/* Resize Dialer entry text */
	_dialer_text_resize(ad->entry);
	return 1;
}

void dash_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	char file_path[PATH_MAX];

	Elm_Object_Item *it = (Elm_Object_Item*) event_info;
	const char *choice = elm_object_item_part_text_get(it, "elm.text");
	DLOG_PRINT("Printing default part text %s", choice);
	DLOG_PRINT("-----------------------------");
	if(!strcmp(choice, "Settings")){
		sprintf(file_path, "%s%s", RES_DIR, EDJ_WHEEL_FILE);
		ad->choice = 2;
	}else{
		sprintf(file_path, "%s%s", RES_DIR, EDJ_DASH_FILE);
		if(!strcmp(choice, "Vehicle Dash")){
			ad->choice = 0;
		}
		if(!strcmp(choice, "Wheels")){
			ad->choice = 1;
		}
		timer = ecore_timer_add(1.0, trigger_auto_update, ad);
	}

	//if(ad->layout)evas_object_del(ad->layout);
	ad->layout = elm_layout_add(ad->nf);
	DLOG_PRINT("Layout added %p", ad->layout);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(ad->layout, EEXT_CALLBACK_BACK, win_back_cb, ad);
	evas_object_show(ad->layout);

	if(ad->choice < 2)
	{
		elm_layout_file_set(ad->layout, file_path, GRP_MAIN);
		DLOG_PRINT("Dialer created %s.", file_path);

		view_set_image(ad->layout, "sw.button.bg", images[20]);
		view_set_color(ad->layout, "sw.button.bg", 8, 36, 61, 255);

		view_set_image(ad->layout, "sw.button.call", images[21]);
		view_set_color(ad->layout, "sw.button.call", 100, 214, 146, 255);

		sprintf(file_path, images[22], 0);
		view_set_image(ad->layout, "sw.button.delete", file_path);
		view_set_color(ad->layout, "sw.button.delete", 255, 255, 255, 255);

		view_dialer_set_entry(data, "sw.entry.dial");
		view_set_color(ad->layout, "sw.entry.dial", 250, 250, 250, 255);
		it = elm_naviframe_item_push(ad->nf, "Dial", NULL, NULL, ad->layout, NULL);
		elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
	}
	else
	{
		ad->entry = elm_entry_add(ad->nf);
		char *address = NULL;
		int ret = preference_get_string("SERVER_ADDRESS", &address);
		if(ret == PREFERENCE_ERROR_NONE){
			elm_object_text_set(ad->entry, address);
			free(address);
		}else{
			elm_object_text_set(ad->entry, "172.30.1.38:8888");
			preference_set_string("SERVER_ADDRESS", "172.30.1.38:8888");
		}
		elm_entry_text_style_user_push(ad->entry, "DEFAULT='font=Tizen:style=Light font_size=21 color=#0ff align=center'");
		evas_object_show(ad->entry);
		it = elm_naviframe_item_push(ad->nf, "Settings", NULL, NULL, ad->entry, NULL);
		DLOG_PRINT( "Adding ENTRY");
	}
	Evas_Object *rect = evas_object_rectangle_add(evas_object_evas_get(ad->layout));
	evas_object_color_set(rect, 255, 255, 255, 0);
	evas_object_repeat_events_set(rect, EINA_TRUE);
	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, rect);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, _rectangle_mouse_down_cb, data);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, _rectangle_mouse_up_cb, data);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, _rectangle_mouse_move_cb, data);
	evas_object_show(rect);
	DLOG_PRINT( "Top Item After Dial %p - %p\n", it, ad->nf_it);
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
		DLOG_PRINT("Ignore touch event under min radius");
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

	//DLOG_PRINT("_mouse_move_cb is called down[%d]", ad->mouse_down_dial_num);
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
		DLOG_PRINT("Timer SET 0x%x", ad->timer);
	}
}

static void _rectangle_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	if (ad->timer != NULL) {
		DLOG_PRINT("Timer DELETE 0x%x - mouse up", ad->timer);
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}

	/* Ignore unmatched touch event with mouse down event */
	if (ad->mouse_down_dial_num == -1)
		return;

	/* Set new Entry text */
	char new_dial[2] = { 0, };
	snprintf(new_dial, sizeof(new_dial), "%d", ad->mouse_down_dial_num);
	DLOG_PRINT("Number Pressed %s", new_dial);
	view_dialer_set_entry_text(ad, ENTRY_TEXT_ADD_TEXT, new_dial);

	/* Initialize event records */
	//ad->mouse_down_dial_num = -1;
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
	snprintf(buf, 1023, "%s", "W3C VSS Demo");
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
	Evas_Object *circle_genlist;
	Evas_Object *btn;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	int index = 0;

	/* Genlist */
	ad->genlist = elm_genlist_add(ad->nf);
	ad->layout= NULL;
	elm_genlist_mode_set(ad->genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(ad->genlist, "selected", gl_selected_cb, NULL);

	// Eext Circle Surface Creation
	ad->circle_surface = eext_circle_surface_naviframe_add(ad->nf);
	circle_genlist = eext_circle_object_genlist_add(ad->genlist, ad->circle_surface);
	//eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
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
	elm_genlist_item_append(ad->genlist, ttc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	/* Main Menu Items Here */
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(ad->genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, dash_cb, ad);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(ad->genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, dash_cb, ad);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(ad->genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, dash_cb, ad);

	elm_genlist_item_append(ad->genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	btn = elm_button_add(ad->nf);
	elm_object_style_set(btn, "naviframe/end_btn/default");
	ad->nf_it = elm_naviframe_item_push(ad->nf, NULL, btn, NULL, ad->genlist, "empty");
	DLOG_PRINT( "Top Item After List %p - %p\n", ad->nf_it, elm_naviframe_top_item_get(ad->nf));
}

Eina_Bool setup_auth(void *data){
    server_changed_cb("SERVER_ADDRESS", NULL);
    return EINA_FALSE;
}

void
create_base_gui(void *data)
{
	appdata_s *ad = (appdata_s *)data;

	g_appdata = ad;

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
	eext_object_event_callback_add(ad->mainlayout, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);

	save_setting("172.30.1.38:8888");
	ecore_timer_add(1.0, setup_auth, NULL);
	preference_set_changed_cb ("SERVER_ADDRESS", server_changed_cb, NULL);
}
