#ifndef _OCF_H_
#define _OCF_H_

//Tizen Headers
#include <app.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <message_port.h>
#include <bundle.h>

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
#include "OCPlatform.h"
#include "OCApi.h"

#define KEY_END "XF86Stop"

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

#define MSGPORT "s2geniviport"
#define ICON_DIR "/opt/usr/apps/org.openconnectivity.geniviocf/res/images/"
#define ELM_DEMO_EDJ "/opt/usr/apps/org.openconnectivity.geniviocf/res/ui_controls.edj"
#define ROTARY_SELECTOR_PAGE_COUNT 1
#define DUMMY_DEVID "UNDEFINED"

const char common_strings[][25] = {
		"modeStandard",
		"modeMovie",
		"modeDynamic"

		"modeStandard",
		"modeMovie",
		"modeMusic",

		"DTV",
		"HDMI1",
		"HDMI2",
		"HDMI3",
		"HDMI4"

		"Good Morning",
		"Start Workout",
		"Leaving Home",
		"Coming Home",
		"Movie Mode",
		"Good Night",
		"Invalid Scene"
};

typedef enum {
	//TV
    TVPOWER,
    VOLMUTE,
    PICMODE,
    SOUNDMODE,
	SOURCE,

	//Family Hub
	FHDCV,
	FHDF,
	FHDC,
	FHFREEZER,
	FHCCT,
	FHCDT,
	FHFCT,
	FHFDT,
	FHCVCT,
	FHCVDT,
	FHMODE,

	//AC
	ACPOWER,
	AIRFLOW,
	ACMODE,
	ACTEMP,
	ACSENSORS,

	//RVC
	RVCPOWER,
	RVCMODE,

#if 0
	AN_AMB,
	AN_AMB_UNIT,
	AN_SETPOINT,
	AN_SP_UNIT,
	AN_SPNAME,
	AN_SPMODE,
	AN_HVACNAME,
	AN_HVACMODE,
#endif

	MSP4,
	MAX_RESOURCES
} ocfrestype_e;

typedef enum {
	TV,
	FAMILYHUB,
	AC,
	RVC,
	SURFACE,
	MAX_DEVICES
}devtype_e;


typedef struct {
	char name[30];
	char namedata[30];
	char value[30];
	union {
		bool b;
		int i;
		double d;
		char c[5][30];
	};//valuedata
}nvp_s;


typedef struct {
	int count;
	char devid[100];
	bool power;
	int volume;
	bool mute;
	char picmode[20];
	char soundmode[20];
	nvp_s source[5];
}tv_s;


typedef struct {
	int count;
	char devid[100];
	bool dcv;
	bool df;
	bool dc;
	bool rapidfreeze;
	bool rapidcool;

	//use name for units
	nvp_s cct;
	nvp_s cdt;
	nvp_s fct;
	nvp_s fdt;
	nvp_s cvct;
	nvp_s cvdt;

}fh_s;


typedef struct {
	int count;
	char devid[100];
	bool power;
	nvp_s airflow;
	nvp_s modes;
	nvp_s temp;
	nvp_s sensors[4];

}ac_s;


typedef struct {
	int count;
	char devid[100];
	bool power;
	nvp_s modes;
}rvc_s;

typedef struct {
	char devid[100];
	int count;
	nvp_s scene;
}ms_s;

typedef struct {
	ocfrestype_e restype;
	char name[100];
	char ruri[100];
	char icon[1024];
	devtype_e devtype;
	bool found;
}lr_resources_s; //master


typedef struct {
	tv_s tv;
	fh_s fh;
	ac_s ac;
	rvc_s rvc;
	ms_s surface;
}lr_data_s;


typedef struct {
	lr_data_s data;
	lr_resources_s *res;
}lr_s;



typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Evas_Object *bg;
	Eext_Circle_Surface *circle_surface;

	Elm_Object_Item *nf_it;
	Elm_Object_Item *it;
	Evas_Object *event_label;
	Evas_Object *start;
	Evas_Object *stop;
	Evas_Object *progressbar;
	Evas_Object *popup;
	Evas_Object *image;

	Evas_Object *rotary_selector;

	Evas_Object *slider_layout;
	Evas_Object *slider;
	Evas_Object *scroller;
	Evas_Object *circle_scroller;

	Evas_Object *datetime;
	struct tm saved_time;

	lr_s master;
	int choice;

} appdata_s;


//ocf API
void initOCFStack(appdata_s *);
void postOCF(lr_s *s);
void putOCF(lr_s *);
void getOCF(lr_s *);
void findDeviceResource(lr_s *);
lr_resources_s *get_livingroom_thing(ocfrestype_e );

//Common API
void send_message(lr_resources_s *p);
void init_global_resources(appdata_s *);
char *get_device_name(devtype_e type);
lr_resources_s *get_res_by_devtype(devtype_e devtype);
lr_resources_s *get_res_by_name(const char *name);
lr_resources_s *get_res_by_restype(ocfrestype_e restype);
char *get_device_name(devtype_e devtype);
void list_all_gestures();



//UI API
void create_base_gui(appdata_s *ad);
void index_cb(void *data, Evas_Object * obj, void *event_info);
void eext_rotary_selector_cb(void *data, Evas_Object * obj, void *event_info);
void show_power_screen(void *data);
void show_graphic_popup(appdata_s *ad, char *text, int timeout);
void show_spinner(void *data, char *units, double begin, double end, double incr, char *text);

#endif
