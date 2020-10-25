/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

Evas_Object *focused_obj = NULL;

static void _timer_cb(void *data, Evas_Object *obj, void *event_info);
static void _default_cb(void *data, Evas_Object *obj, void *event_info);

static struct _menu_item _menu_its[] = {
       {"Set timer", _timer_cb},
       {"Default", _default_cb},
       {NULL, NULL}
};

Evas_Object *second;

static void _list_click(void *data, Evas_Object * obj, void *event_info)
{
   Elm_Object_Item *it = (Elm_Object_Item *) elm_list_selected_item_get(obj);

   if (!it)
     {
        fprintf(stderr, "list item is NULL\n");
        return;
     }

   elm_list_item_selected_set(it, EINA_FALSE);
}

static void _focused_cb(void *data, Evas_Object *obj, void *event_info)
{
   focused_obj = obj;
   eext_rotary_object_event_activated_set(obj, EINA_TRUE);
}

static Eina_Bool
_spinner_naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
   eext_rotary_object_event_activated_set(focused_obj, EINA_FALSE);

   return EINA_TRUE;
}

static void
_btn_clicked(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *nf = data;

   elm_naviframe_item_pop(nf);
}

static void _timer_cb(void *data, Evas_Object * obj, void *event_info)
{
   appdata_s *ad = data;
   Evas_Object *layout, *hour, *minute, *bottom_button;
   Elm_Object_Item *nf_it;

   layout = elm_layout_add(ad->nf);
   //This customized layout for two or more spinner object in one layout.
   elm_layout_file_set(layout, ELM_DEMO_EDJ, "eext_spinner_layout");
   evas_object_show(layout);

   hour = elm_spinner_add(layout);
   elm_object_style_set(hour,"circle");
   //Circle spinner add for circular visual interaction with rotary event.
   eext_circle_object_spinner_add(hour, ad->circle_surface);
   elm_spinner_min_max_set(hour, 0.0, 23.0);
   elm_object_part_text_set(hour, "elm.text", "Hour");
   evas_object_smart_callback_add(hour, "focused", _focused_cb, hour);
   elm_object_part_content_set(layout, "hour", hour);

   minute = elm_spinner_add(layout);
   elm_object_style_set(minute,"circle");
   //Circle spinner add for circular visual interaction with rotary event.
   eext_circle_object_spinner_add(minute, ad->circle_surface);
   elm_spinner_min_max_set(minute, 0.0, 59.0);
   elm_object_part_text_set(minute, "elm.text", "Minute");
   evas_object_smart_callback_add(minute, "focused", _focused_cb, minute);
   elm_object_part_content_set(layout, "minute", minute);

   second = elm_spinner_add(layout);
   elm_object_style_set(second,"circle");
   //Circle spinner add for circular visual interaction with rotary event.
   eext_circle_object_spinner_add(second, ad->circle_surface);
   elm_spinner_min_max_set(second, 0.0, 59.0);
   elm_object_part_text_set(second, "elm.text", "Second");
   evas_object_smart_callback_add(second, "focused", _focused_cb, second);
   elm_object_part_content_set(layout, "second", second);

   bottom_button = elm_button_add(layout);
   elm_object_style_set(bottom_button, "bottom");
   elm_object_text_set(bottom_button, "SET");
   evas_object_smart_callback_add(bottom_button, "clicked", _btn_clicked, ad->nf);
   elm_object_part_content_set(layout, "elm.swallow.button", bottom_button);

   elm_object_part_text_set(layout, "elm.text.title", "Set timer");

   elm_object_focus_set(second, EINA_TRUE);

   nf_it = elm_naviframe_item_push(ad->nf, _("Spinner"), NULL, NULL, layout, NULL);
   elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
   elm_naviframe_item_pop_cb_set(nf_it, _spinner_naviframe_pop_cb, NULL);
}

static void _default_cb(void *data, Evas_Object * obj, void *event_info)
{
   appdata_s *ad = data;
   Evas_Object *layout, *unit, *bottom_button, *circle_spinner;
   Elm_Object_Item *nf_it;

   layout = elm_layout_add(ad->nf);
   elm_layout_theme_set(layout, "layout", "circle", "spinner");

   evas_object_show(layout);

   unit = elm_spinner_add(layout);
   elm_object_style_set(unit,"circle");
   //Circle spinner add for circular visual interaction with rotary event.
   circle_spinner = eext_circle_object_spinner_add(unit, ad->circle_surface);
   //This angle per each spinner value will be multiplied with step value
   //and set the circle spinner picker offset, when every rotary event called.
   eext_circle_object_spinner_angle_set(circle_spinner, 2.0);
   evas_object_show(unit);

   elm_spinner_wrap_set(unit, EINA_FALSE);
   elm_spinner_label_format_set(unit, "%1.1f");
   elm_spinner_min_max_set(unit, 9.0, 99.0);
   //The circle spinner picker move angle will be calculated below formula
   //{360/(99(Spinner max)-9(Spinner min)) * 2.0(Circle spinner angle) * 7.5(Spinner step)}
   elm_spinner_step_set(unit, 7.5);
   elm_object_part_text_set(unit, "elm.text", "unit");
   evas_object_smart_callback_add(unit, "focused", _focused_cb, unit);
   elm_object_part_content_set(layout, "elm.swallow.content", unit);

   bottom_button = elm_button_add(layout);
   elm_object_style_set(bottom_button, "bottom");
   elm_object_text_set(bottom_button, "SET");
   evas_object_smart_callback_add(bottom_button, "clicked", _btn_clicked, ad->nf);
   elm_object_part_content_set(layout, "elm.swallow.btn", bottom_button);

   elm_object_part_text_set(layout, "elm.text", "Title");
   nf_it = elm_naviframe_item_push(ad->nf, _("Spinner"), NULL, NULL, layout, NULL);
   elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
   elm_naviframe_item_pop_cb_set(nf_it, _spinner_naviframe_pop_cb, NULL);
}

static Evas_Object *_create_list_winset(appdata_s *ad)
{
   Evas_Object *li;
   struct _menu_item *menu_its;
   int idx = 0;

   if (!ad) return NULL;

   li = elm_list_add(ad->nf);
   elm_list_mode_set(li, ELM_LIST_COMPRESS);
   evas_object_smart_callback_add(li, "selected", _list_click, NULL);

   menu_its = _menu_its;
   while (menu_its[idx].name != NULL) {
        elm_list_item_append(li, menu_its[idx].name, NULL, NULL, menu_its[idx].func, ad);
        ++idx;
   }
   elm_list_go(li);

   return li;
}

void eext_spinner_cb(void *data, Evas_Object * obj, void *event_info)
{
   appdata_s *ad;

   Evas_Object *list;

   ad = (appdata_s *)data;
   if (!ad) return;

   list = _create_list_winset(ad);
   elm_naviframe_item_push(ad->nf, _("Spinner"), NULL, NULL, list, NULL);
}
