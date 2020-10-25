/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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

static char *menu_its[] = {
	"Solid Color",
	"Image - CENTER",
	"Image - SCALE",
	"Image - STRETCH",
	"Image - TILE",
	/* do not delete below */
	NULL
};

typedef struct _item_data
{
	int index;
	Elm_Object_Item *item;
} item_data;

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

	snprintf(buf, 1023, "%s", "Bg");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, 1023, "%s", menu_its[index]);
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
bg_color_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *bg;
	Evas_Object *nf = data;

	bg = elm_bg_add(nf);
	elm_bg_color_set(bg, 66, 162, 206);

	elm_naviframe_item_push(nf, "Solid Color", NULL, NULL, bg, "empty");
}

static void
bg_image_center_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *bg;
	Evas_Object *nf = data;
	char buf[PATH_MAX];

	bg = elm_bg_add(nf);
	elm_bg_option_set(bg, ELM_BG_OPTION_CENTER);
	snprintf(buf, sizeof(buf), "%s/logo.png", ICON_DIR);
	elm_bg_file_set(bg, buf, NULL);

	elm_naviframe_item_push(nf, "Image - CENTER", NULL, NULL, bg, "empty");
}

static void
bg_image_scale_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *bg;
	Evas_Object *nf = data;
	char buf[PATH_MAX];

	bg = elm_bg_add(nf);
	elm_bg_option_set(bg, ELM_BG_OPTION_SCALE);
	snprintf(buf, sizeof(buf), "%s/logo.png", ICON_DIR);
	elm_bg_file_set(bg, buf, NULL);

	elm_naviframe_item_push(nf, "Image - SCALE", NULL, NULL, bg, "empty");
}

static void
bg_image_stretch_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *bg;
	Evas_Object *nf = data;
	char buf[PATH_MAX];

	bg = elm_bg_add(nf);
	elm_bg_option_set(bg, ELM_BG_OPTION_STRETCH);
	snprintf(buf, sizeof(buf), "%s/logo.png", ICON_DIR);
	elm_bg_file_set(bg, buf, NULL);

	elm_naviframe_item_push(nf, "Image - STRETCH", NULL, NULL, bg, "empty");
}

static void
bg_image_tile_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *bg;
	Evas_Object *nf = data;
	char buf[PATH_MAX];

	bg = elm_bg_add(nf);
	elm_bg_option_set(bg, ELM_BG_OPTION_TILE);
	snprintf(buf, sizeof(buf), "%s/logo.png", ICON_DIR);
	elm_bg_file_set(bg, buf, NULL);

	elm_naviframe_item_push(nf, "Image - TILE", NULL, NULL, bg, "empty");
}

void
bg_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *genlist;
	Evas_Object *circle_genlist;
	Evas_Object *nf = ad->nf;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	int index = 0;

	genlist = elm_genlist_add(nf);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "selected", gl_selected_cb, NULL);

	circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);

	ttc->item_style = "title";
	ttc->func.text_get = _gl_menu_title_text_get;
	ttc->func.del = _gl_menu_del;

	itc->item_style = "default";
	itc->func.text_get = _gl_menu_text_get;
	itc->func.del = _gl_menu_del;

	ptc->item_style = "padding";
	ptc->func.del = _gl_menu_del;

	elm_genlist_item_append(genlist, ttc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, bg_color_cb, nf);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, bg_image_center_cb, nf);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, bg_image_scale_cb, nf);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, bg_image_stretch_cb, nf);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, bg_image_tile_cb, nf);

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ptc);

	elm_naviframe_item_push(nf, "Bg", NULL, NULL, genlist, "empty");
}
