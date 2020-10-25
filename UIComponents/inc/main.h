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
#include <Elementary.h>
#include <system_settings.h>
#include <dlog.h>
#include <efl_extension.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "uicomponents"

#define KEY_END "XF86Stop"

#if !defined(PACKAGE)
#define PACKAGE "org.example.uicomponents"
#endif

#define ELM_DEMO_EDJ "/opt/usr/apps/org.example.uicomponents/res/ui_controls.edj"
#define ICON_DIR "/opt/usr/apps/org.example.uicomponents/res/images"


typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Evas_Object *datetime;
	Evas_Object *popup;
	Evas_Object *button;
	Eext_Circle_Surface *circle_surface;
	struct tm saved_time;
} appdata_s;

struct _menu_item {
   char *name;
   void (*func)(void *data, Evas_Object *obj, void *event_info);
};

void bottom_button_cb(void *data, Evas_Object * obj, void *event_info);
void button_cb(void *data, Evas_Object * obj, void *event_info);
void check_cb(void *data, Evas_Object * obj, void *event_info);
void ctxpopup_cb(void *data, Evas_Object * obj, void *event_info);
void datetime_cb(void *data, Evas_Object * obj, void *event_info);
void entry_cb(void *data, Evas_Object * obj, void *event_info);
void genlist_cb(void *data, Evas_Object * obj, void *event_info);
void image_cb(void *data, Evas_Object * obj, void *event_info);
void layout_cb(void *data, Evas_Object * obj, void *event_info);
void naviframe_cb(void *data, Evas_Object * obj, void *event_info);
void progressbar_cb(void *data, Evas_Object * obj, void *event_info);
void index_cb(void *data, Evas_Object * obj, void *event_info);
void radio_cb(void *data, Evas_Object * obj, void *event_info);
void scroller_cb(void *data, Evas_Object * obj, void *event_info);
void popup_cb(void *data, Evas_Object * obj, void *event_info);
void bg_cb(void *data, Evas_Object * obj, void *event_info);
void map_cb(void *data, Evas_Object * obj, void *event_info);
void eext_slider_cb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED);
void eext_scroller_cb(void *data, Evas_Object *obj, void *event_info);
void eext_spinner_cb(void *data, Evas_Object *obj, void *event_info);
void eext_progressbar_cb(void *data, Evas_Object *obj, void *event_info);
void eext_genlist_cb(void *data, Evas_Object *obj, void *event_info);
void eext_datetime_cb(void *data, Evas_Object * obj, void *event_info);
void eext_more_option_cb(void *data, Evas_Object *obj, void *event_info);
void eext_rotary_selector_cb(void *data, Evas_Object * obj, void *event_info);

//void progress_cb(void *data, Evas_Object * obj, void *event_info);
