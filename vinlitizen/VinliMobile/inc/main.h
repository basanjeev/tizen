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
#include <vinli.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "vinlimobile"

#define KEY_END "XF86Stop"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.vinlimobile"
#endif

#define ELM_DEMO_EDJ "/opt/usr/apps/org.tizen.vinlimobile/res/ui_controls.edj"
#define ICON_DIR "/opt/usr/apps/org.tizen.vinlimobile/res/images"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Evas_Object *popup;
	Evas_Object *en;
} appdata_s;

void download_thread_end_cb(void *data, Ecore_Thread *thread);
void download_thread(void *data, Ecore_Thread *thread);
void download_feedback_cb(void *data, Ecore_Thread *thread, void *msg_data);
Evas_Object *getResult();
void _popup_close_cb(void *, Evas_Object *, void *);

