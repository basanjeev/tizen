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
#include <Elementary.h>
#include "tinnosserver.h"

#define KEY_END "XF86Stop"

struct _menu_item {
   char *name;
   void (*func)(void *data, Evas_Object *obj, void *event_info);
};

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "w3cocf"

#if !defined(PACKAGE)
#define PACKAGE "org.openconnectivity.w3cocf"
#endif

#define RES_DIR "/opt/usr/apps/org.openconnectivity.w3cocf/res/"
#define ICON_DIR RES_DIR"images"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *mainlayout;


	Evas_Object *entry;
	Evas_Object *genlist;
	Ecore_Timer *timer;
	Elm_Object_Item *nf_it;

	Evas_Object *nf;
	Eext_Circle_Surface *circle_surface;


	char edj_file[PATH_MAX];
	int mouse_down_dial_num;
	int choice;
} appdata_s;

//ocf API
void initOCFStack(appdata_s *);

//Common API
void send_message(bool p);
void init_global_resources(appdata_s *);
void refresh_dial(appdata_s *ptr);
tinnos_signal_s *get_tinnos_signal(int i);

bool has_signal_changed(int i);
char *get_signal_name(int i);
char *get_signal_value(int i, bool l);

#endif
