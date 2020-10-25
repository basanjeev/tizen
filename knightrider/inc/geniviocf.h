#ifndef _GENIVIOCF_H_
#define _GENIVIOCF_H_

//Tizen Headers
#include <app.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <message_port.h>
#include <bundle.h>
#include <net_connection.h>

#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <map>
#include <cstdlib>
#include <functional>
#include <Elementary.h>

#include <mutex>
#include <condition_variable>
#ifdef OCF
#include "OCPlatform.h"
#include "OCApi.h"
#endif

#define KEY_END "XF86Stop"
using namespace std;

struct _menu_item {
   char *name;
   void (*func)(void *data, Evas_Object *obj, void *event_info);
};

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "geniviocf"

#if !defined(PACKAGE)
#define PACKAGE "org.openconnectivity.geniviocf"
#endif

#define MSGPORT "geniviport"
#define ICON_DIR "/opt/usr/apps/org.openconnectivity.geniviocf/res/images"
#define ELM_DEMO_EDJ "/opt/usr/apps/org.openconnectivity.geniviocf/res/ui_controls.edj"
#define ROTARY_SELECTOR_PAGE_COUNT 1
#define DUMMY_DEVID "UNDEFINED"

typedef enum {
	VEHICLECONTROL,
	FHNOTI,
    MAX_RESOURCES
} ocfrestype_e;

typedef enum {
	GENIVI_VEHICLE,
	FH,
	MAX_DEVICES
}devtype_e;

typedef enum {
	NOGESTURE = -1,
	TAP1,
	TAP2,
	TAP3,
	LINELEFT,
	LINERIGHT,
	LINEUP,
	LINEDOWN,
	SOS,
}gesture_e;

typedef struct {
	gesture_e type;
	Elm_Gesture_Type etype;
	char cause[100];
	char effect[100];
	char bg[1024];
} gesture_s;


typedef struct {
	ocfrestype_e restype;
	devtype_e devtype;
	char sid[500];
	char name[100];
	char ruri[100];
	char icon[1024];
	bool found;
	string host;
}lr_resources_s; //master

typedef struct {
	int user;
	int scene;
	gesture_e gesture;
	lr_resources_s *res;
}geniviocf_s; //master

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Evas_Object *bg;
	Evas_Object *rect;
	Evas_Object *gl;
	Evas_Object *gestbg;
	Eext_Circle_Surface *circle_surface;

	Elm_Object_Item *nf_it;
	Elm_Object_Item *gestit;

	Elm_Object_Item *it;
	Evas_Object *event_label;
	Evas_Object *start;
	Evas_Object *stop;
	Evas_Object *progressbar;
	Evas_Object *popup;
	Evas_Object *image;
	Evas_Object *spinner;

	Evas_Object *rotary_selector;

	Evas_Object *slider_layout;
	Evas_Object *slider;
	Evas_Object *scroller;
	Evas_Object *circle_scroller;

	Evas_Object *datetime;
	struct tm saved_time;

	geniviocf_s master;

} appdata_s;
//ocf API
void initOCFStack(appdata_s *);
void postOCF(geniviocf_s *s);
void findDeviceResource(geniviocf_s *);
void findDevices(appdata_s *ad);
void observeOCF(lr_resources_s *s);

//Common API
void send_message(lr_resources_s *p);
void init_global_resources(appdata_s *);
char *get_device_name(devtype_e type);
lr_resources_s *get_res_by_devtype(devtype_e devtype);
lr_resources_s *get_res_by_name(const char *name);
lr_resources_s *get_res_by_restype(ocfrestype_e restype);
char *get_device_name(devtype_e devtype);


//UI API
void create_base_gui(appdata_s *ad);
void index_cb(void *data, Evas_Object * obj, void *event_info);
void eext_rotary_selector_cb(void *data, Evas_Object * obj, void *event_info);
void show_power_screen(void *data);
void show_graphic_popup(appdata_s *ad, char *text, char *img, int timeout);
void show_spinner(void *data, char *units, double begin, double end, double incr, char *text);
void _gesture_clicked_cb(void *data, Evas_Object *obj, void *event_info);
void gesture_callbacks_set(appdata_s *ad, bool set);

#endif
