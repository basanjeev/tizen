/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */

#include <app.h>
#include <system_settings.h>
#include <dlog.h>
#include <Elementary.h>
#include <efl_extension.h>
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


#include <mutex>
#include <condition_variable>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "ocfauto"

#define KEY_END "XF86Stop"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.ocf.auto"
#endif

#define ELM_DEMO_EDJ "/opt/usr/apps/org.tizen.ocf.auto/res/ui_controls.edj"
#define ICON_DIR "/opt/usr/apps/org.tizen.ocf.auto/res/images"
#define MSGPORT "geniviport"
#define ROTARY_SELECTOR_PAGE_COUNT 1
#define DUMMY_DEVID "UNDEFINED"

typedef enum {
    USERPROFILE,
    MAX_RESOURCES
} ocfrestype_e;

typedef enum {
	GENIVI_VEHICLE,
	MAX_DEVICES
}devtype_e;

typedef struct {
	ocfrestype_e restype;
	char name[100];
	char ruri[100];
	char icon[1024];
	devtype_e devtype;
	bool found;
	char sid[500];
}lr_resources_s; //master


typedef struct {
	ocfrestype_e restype;
	int user;
	int scene;
	lr_resources_s *res;
}geniviocf_s; //master

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
int initOCFStack(appdata_s *);
void postOCF(geniviocf_s *s);
void findDeviceResource(geniviocf_s *);
int findDevices(appdata_s *ad);

//Common API
void send_message(geniviocf_s *p);
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
void show_graphic_popup(appdata_s *ad, char *text, int timeout);
void show_spinner(void *data, char *units, double begin, double end, double incr, char *text);
