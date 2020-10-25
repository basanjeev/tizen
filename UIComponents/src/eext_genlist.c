/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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

#define NUM_OF_ITEMS 101
#define NUM_OF_GENLIST_DEMO_NAMES 101
#define NUM_OF_GENLIST_DEMO_LONG_TEXT 7

static char *menu_its[] = {
	/*** 1line styles ***/
	"title",
	"title_with_groupindex",
	"1text",
	"1text.1icon",
	"1text.1icon.1",
	"1text.1icon.divider",
	"2text",
	"2text.1icon",
	"2text.1icon.1",
	"2text.1icon.divider",
	"3text",
	"4text",
	"editfield",
	"multiline",
	"select_mode",
	/* do not delete below */
	NULL
};

char *eext_genlist_demo_names[] = {
	"Aaliyah", "Aamir", "Aaralyn", "Aaron", "Abagail",
	"Babitha", "Bahuratna", "Bandana", "Bulbul", "Cade", "Caldwell",
	"CaptainFantasticFasterThanSupermanSpidermanBatmanWolverineHulkAndTheFlashCombined",
	"Chandan", "Caster", "Dagan ", "Daulat", "Dag", "Earl", "Ebenzer",
	"Ellison", "Elizabeth", "Filbert", "Fitzpatrick", "Florian", "Fulton",
	"Frazer", "Gabriel", "Gage", "Galen", "Garland", "Gauhar", "Hadden",
	"Hafiz", "Hakon", "Haleem", "Hank", "Hanuman", "Jabali ", "Jaimini",
	"Jayadev", "Jake", "Jayatsena", "Jonathan", "Kamaal", "Jeirk",
	"Jasper", "Jack", "Mac", "Macy", "Marlon", "Milson",
	"Aaliyah", "Aamir", "Aaralyn", "Aaron", "Abagail",
	"Babitha", "Bahuratna", "Bandana", "Bulbul", "Cade", "Caldwell",
	"Chandan", "Caster", "Dagan ", "Daulat", "Dag", "Earl", "Ebenzer",
	"Ellison", "Elizabeth", "Filbert", "Fitzpatrick", "Florian", "Fulton",
	"Frazer", "Gabriel", "Gage", "Galen", "Garland", "Gauhar", "Hadden",
	"Hafiz", "Hakon", "Haleem", "Hank", "Hanuman", "Jabali ", "Jaimini",
	"Jayadev", "Jake", "Jayatsena", "Jonathan", "Kamaal", "Jeirk",
	"Jasper", "Jack", "Mac", "Macy", "Marlon", "Milson",
	NULL
};

char *eext_genlist_demo_names2 = "GenlistDemoLongTitle";

char *eext_long_txt[] = {
	"Hey John, how have you been?",
	"Andy, it's been a long time, how are you man?",
	"I finally have some free time. I just finished taking a big examination, and I'm so relieved that I'm done with it",
	"Wow. How long has it been? It seems like more than a year. I'm doing pretty well. How about you?",
	"I'm playing a video game on my computer because I have nothing to do.",
	"I'm pretty busy right now. I'm doing my homework because I have an exam tomorrow.",
	"I'm taking the day off from work today because I have so many errands. I'm going to the post office to send some packages to my friends.",
	NULL
};

typedef struct _Item_Data
{
	int index;
	Elm_Object_Item *item;
} Item_Data;

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

	snprintf(buf, 1023, "%s", "Eext Genlist");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	Item_Data *id = (Item_Data *)data;
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
	Item_Data *id = (Item_Data *)data;
	if (id) free(id);
}

typedef struct _genlist_view_data
{
	appdata_s* ad;
	Evas_Object *btn;
	Evas_Object *layout;
	Evas_Object *ctxpopup;
	unsigned int check_cnt;
	Eina_Bool is_select_mode;
} genlist_view_data;

typedef struct _item_data
{
	genlist_view_data *gvd;
	int index;
	Elm_Object_Item *item;
	Evas_Object *check;
	Eina_Bool checked;
	Eina_Bool disabled;
} item_data;


static void
_gl_del(void *data, Evas_Object *obj)
{
	/* FIXME: Unrealized callback can be called after this.
	Accessing Item_Data can be dangerous on unrealized callback. */
	item_data *id = (item_data *)data;
	if (id) free(id);
}

static void
_gl_ctxpopup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	genlist_view_data *gvd = (genlist_view_data *)data;
	if (gvd) free(gvd);
}

static void
_gl_icon_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	int param = (int) data;

	Eina_Bool state = elm_check_state_get(obj);
	printf("Check %d:%d\n", param, state);
}

static Evas_Object*
_gl_icon_get(void *data, Evas_Object *obj, const char *part)
{
	item_data *id = (item_data *)data;
	Evas_Object *content = NULL;

	if (strcmp(part, "elm.icon")) return NULL;

	content = elm_check_add(obj);
	elm_check_state_set(content, EINA_FALSE);
	evas_object_smart_callback_add(content, "changed", _gl_icon_clicked_cb, (void*)(id->index));
	elm_check_state_pointer_set(content, &id->checked);
	evas_object_show(content);

	return content;
}

static void
_gl_check_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	item_data *id = (item_data *)data;
	char buf[5];
	id->gvd->check_cnt += (id->checked)? 1:-1;
	snprintf(buf, 5, "%.2d", id->gvd->check_cnt);
	elm_object_text_set(id->gvd->btn, buf);
}

static void
_gl_longpress_cb(void *data, Evas_Object *obj, void *event_info)
{
	item_data *id = NULL;
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	genlist_view_data *gvd = (genlist_view_data *)data;
	elm_genlist_item_selected_set(it, EINA_FALSE);

	if (!gvd->is_select_mode) {
		gvd->is_select_mode = EINA_TRUE;
		id = elm_object_item_data_get(it);
		id->checked = EINA_TRUE;
		elm_genlist_realized_items_update(obj);

		char buf[5];
		gvd->check_cnt++;
		snprintf(buf, 5, "%.2d", gvd->check_cnt);
		elm_object_text_set(gvd->btn, buf);
		elm_object_signal_emit(gvd->layout, "select_mode,button,show", "");
	}
}

static Evas_Object*
_gl_check_get(void *data, Evas_Object *obj, const char *part)
{
	item_data *id = (item_data *)data;

	if ((!id->gvd->is_select_mode) || strcmp(part, "elm.swallow.center_check")) return NULL;

	id->check = elm_check_add(obj);
	elm_object_style_set(id->check, "genlist/select_mode");
	elm_check_state_set(id->check, EINA_FALSE);
	evas_object_smart_callback_add(id->check, "changed", _gl_check_clicked_cb, data);
	elm_check_state_pointer_set(id->check, &id->checked);
	evas_object_repeat_events_set(id->check, EINA_FALSE);
	evas_object_propagate_events_set(id->check, EINA_FALSE);
	evas_object_show(id->check);

	return id->check;
}

static Evas_Object*
_gl_divider_icon_get(void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *btn = NULL;
	Evas_Object *ic = NULL;

	if (strcmp(part, "elm.icon")) return NULL;

	btn = elm_button_add(obj);
	elm_object_style_set(btn, "list_divider_btn");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);

	ic = elm_image_add(btn);
	elm_image_file_set(ic, ICON_DIR "/tw_btn_delete_holo_dark.png", NULL);
	elm_object_content_set(btn, ic);
	evas_object_propagate_events_set(btn, EINA_FALSE);

	return btn;
}

static char*
_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text.1") || !strcmp(part, "elm.text.2")) index++;

	snprintf(buf, 1023, "%s:%s", part, eext_genlist_demo_names[index%NUM_OF_GENLIST_DEMO_NAMES]);

	return strdup(buf);
}

static char*
_gl_title_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];

	snprintf(buf, 1023, "%s:%s", part, eext_genlist_demo_names2);

	return strdup(buf);
}

static char *
_gl_long_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	snprintf(buf, 1023, "%s:%s", part, eext_long_txt[index%NUM_OF_GENLIST_DEMO_LONG_TEXT]);
	return strdup(buf);
}

static void
_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(it, EINA_FALSE);

	return;
}

static void
_gl_select_mode_sel(void *data, Evas_Object *obj, void *event_info)
{
	item_data *id = NULL;
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	char buf[5];

	genlist_view_data *gvd = (genlist_view_data *)data;
	elm_genlist_item_selected_set(it, EINA_FALSE);

	if (!gvd->is_select_mode) return;

	id = elm_object_item_data_get(it);
	id->checked = !id->checked;
	elm_check_state_set(id->check, id->checked);
	gvd->check_cnt += (id->checked)? 1:-1;
	snprintf(buf, 5, "%.2d", gvd->check_cnt);
	elm_object_text_set(gvd->btn, buf);
}

static void
_gl_ctxpopup_select_all_cb(void *data, Evas_Object *obj, void *event_info)
{
	char buf[5];

	genlist_view_data *gvd = (genlist_view_data *)data;
	Evas_Object *layout = gvd->layout;
	Evas_Object *genlist = elm_layout_content_get(layout, "elm.swallow.content");
	Evas_Object *select_btn = gvd->btn;
	Evas_Object *ctxpopup = gvd->ctxpopup;
	Elm_Object_Item *it = elm_genlist_first_item_get(genlist);

	while(it) {
		item_data *id = elm_object_item_data_get(it);
		if (!id) {
			it = elm_genlist_item_next_get(it);
			continue;
		}
		/* For realized items, set state of real check object */
		Evas_Object *ck = elm_object_item_part_content_get(it, "elm.swallow.center_check");
		if (ck) elm_check_state_set(ck, EINA_TRUE);
		/* For all items (include unrealized), just set pointer state */
		id->checked = EINA_TRUE;
		it = elm_genlist_item_next_get(it);
	}
	// The return value of the elm_genlist_items_count = List item + Padding item
	gvd->check_cnt = elm_genlist_items_count(genlist) - 2;
	snprintf(buf, 5, "%.2d", gvd->check_cnt);
	elm_object_text_set(select_btn, buf);

	if (ctxpopup) elm_ctxpopup_dismiss(ctxpopup);
}

static void
_gl_ctxpopup_deselect_all_cb(void *data, Evas_Object *obj, void *event_info)
{
	char buf[5];

	genlist_view_data *gvd = (genlist_view_data *)data;
	Evas_Object *layout = gvd->layout;
	Evas_Object *genlist = elm_layout_content_get(layout, "elm.swallow.content");
	Evas_Object *select_btn = gvd->btn;
	Evas_Object *ctxpopup = gvd->ctxpopup;

	Elm_Object_Item *it = elm_genlist_first_item_get(genlist);
	while(it) {
		item_data *id = elm_object_item_data_get(it);
		if (!id) {
			it = elm_genlist_item_next_get(it);
			continue;
		}
		/* For realized items, set state of real check object */
		Evas_Object *ck = elm_object_item_part_content_get(it, "elm.swallow.center_check");
		if (ck) elm_check_state_set(ck, EINA_FALSE);
		/* For all items (include unrealized), just set pointer state */
		id->checked = EINA_FALSE;
		it = elm_genlist_item_next_get(it);
	}
	gvd->check_cnt = 0;
	snprintf(buf, 5, "%.2d", gvd->check_cnt);
	elm_object_text_set(select_btn, buf);

	if (ctxpopup) elm_ctxpopup_dismiss(ctxpopup);
}

static void
_gl_dismissed_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *ctxpopup = (Evas_Object *)data;

	evas_object_smart_callback_del(ctxpopup, "dismissed", _gl_dismissed_cb);
	evas_object_del(ctxpopup);
}

static void
_gl_move_ctxpopup(Evas_Object *ctxpopup, Evas_Object *btn)
{
	Evas_Coord x, y, w, h;

	evas_object_geometry_get(btn, &x, &y, &w, &h);
	evas_object_move(ctxpopup, x + (w / 2), y + (h /2));
}

static void
_create_ctxpopup_action_button(void *data, Evas_Object *obj, void *event_info)
{
	genlist_view_data *gvd = (genlist_view_data *)data;
	Evas_Object *layout = gvd->layout;
	Evas_Object *ctxpopup = NULL;
	Evas_Object *genlist = NULL;
	Elm_Object_Item *eo_it1 = NULL;
	Elm_Object_Item *eo_it2 = NULL;

	ctxpopup = elm_ctxpopup_add(layout);
	elm_object_style_set(ctxpopup, "select_mode");
	elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_DOWN,
						ELM_CTXPOPUP_DIRECTION_DOWN,
						ELM_CTXPOPUP_DIRECTION_DOWN,
						ELM_CTXPOPUP_DIRECTION_DOWN);
	evas_object_smart_callback_add(ctxpopup, "dismissed", _gl_dismissed_cb, ctxpopup);

	genlist = elm_layout_content_get(layout, "elm.swallow.content");
	gvd->layout = layout;
	gvd->ctxpopup = ctxpopup;

	if (gvd->check_cnt < (elm_genlist_items_count(genlist) - 2))
		eo_it1 = elm_ctxpopup_item_append(ctxpopup, _("Select all"), NULL, _gl_ctxpopup_select_all_cb, gvd);
	if (gvd->check_cnt)
		eo_it2 = elm_ctxpopup_item_append(ctxpopup, _("Deselect all"), NULL, _gl_ctxpopup_deselect_all_cb, gvd);

	evas_object_show(ctxpopup);
	if (eo_it1 && eo_it2) {
		elm_object_item_style_set(eo_it1, "select_mode/top");
		elm_object_item_style_set(eo_it2, "select_mode/bottom");
	}
	_gl_move_ctxpopup(ctxpopup, obj);
}

static Evas_Object*
_create_select_mode_genlist(genlist_view_data *gvd)
{
	char buf[5];
	Evas_Object * genlist = NULL;

	gvd->layout = elm_layout_add(gvd->ad->nf);
	elm_layout_theme_set(gvd->layout, "layout", "select_mode", "default");
	evas_object_show(gvd->layout);
	genlist = elm_genlist_add(gvd->layout);
	elm_object_style_set(genlist, "focus_bg");
	elm_layout_content_set(gvd->layout, "elm.swallow.content", genlist);

	evas_object_smart_callback_add(genlist, "longpressed", _gl_longpress_cb, gvd);
	elm_genlist_longpress_timeout_set(genlist, 1.0);

	gvd->btn = elm_button_add(gvd->layout);
	elm_object_style_set(gvd->btn, "select_mode");
	snprintf(buf, 5, "%.2d", gvd->check_cnt);
	elm_object_text_set(gvd->btn, buf);
	evas_object_smart_callback_add(gvd->btn, "clicked", _create_ctxpopup_action_button, gvd);

	elm_layout_content_set(gvd->layout, "elm.swallow.icon", gvd->btn);

	return genlist;
}

static Eina_Bool
_nf_pop_cb(void *data, Elm_Object_Item *it)
{
	Evas_Object *genlist = (Evas_Object *)data;

	evas_object_hide(genlist);

	return EINA_TRUE;
}

static void
_create_genlist(void *data, Evas_Object *obj, void *event_info)
{
	if (!data) return;

	appdata_s *ad = (appdata_s *)data;
	genlist_view_data *gvd;
	int index = 0;
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	Elm_Object_Item *item = NULL;
	Evas_Object *genlist = NULL;
	Evas_Object *circle_genlist = NULL;
	const char *style = elm_object_item_part_text_get(it, "elm.text");

	gvd = (genlist_view_data *) calloc(1, sizeof(genlist_view_data));
	gvd->ad = ad;

	/* Create item class */
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *gtc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();

	if (!strcmp(style, "title") || !strcmp(style, "title_with_groupindex"))
		itc->item_style = "2text";
	else if (!strcmp(style, "select_mode"))
		itc->item_style = "1text";
	else
		itc->item_style = style;

	if (!strcmp(style, "multiline"))
		itc->func.text_get = _gl_long_text_get;
	else
		itc->func.text_get = _gl_text_get;
	itc->func.content_get = _gl_icon_get;
	itc->func.del = _gl_del;

	if (!strcmp(style, "title")) {
		ttc->item_style = "title";
		ttc->func.text_get = _gl_title_text_get;
		ttc->func.content_get = _gl_icon_get;
		ttc->func.del = _gl_del;
	} else if (!strcmp(style, "title_with_groupindex")) {
		ttc->item_style = "title_with_groupindex";
		ttc->func.text_get = _gl_text_get;
		ttc->func.content_get = _gl_icon_get;
		ttc->func.del = _gl_del;

		gtc->item_style = "groupindex";
		gtc->func.text_get = _gl_text_get;
		gtc->func.content_get = _gl_icon_get;
		gtc->func.del = _gl_del;
	}

	if(!strcmp(style, "select_mode")) {
		gvd->layout = elm_layout_add(ad->nf);
		genlist = _create_select_mode_genlist(gvd);
		itc->func.content_get = _gl_check_get;
		evas_object_event_callback_add(genlist, EVAS_CALLBACK_DEL, _gl_ctxpopup_del_cb, (void *) gvd);
	} else
		genlist = elm_genlist_add(ad->nf);

	/* COMPRESS MODE
		Because genlist shows full window, compress mode should be used. */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	elm_object_style_set(genlist, "focus_bg");
	ptc->item_style = "padding";

	if (!strstr(style, "multiline"))
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);

	if (strstr(style, "divider"))
		itc->func.content_get = _gl_divider_icon_get;

	if (strcmp(style, "title") && strcmp(style, "title_with_groupindex"))
		elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	for (index = 0; index < NUM_OF_ITEMS; index++) {
		item_data *id = calloc(sizeof(item_data), 1);
		id->index = index;
		id->gvd = gvd;

		if (!strcmp(style, "title")) {
			if (index == 0)
				item = elm_genlist_item_append(
					genlist,			// genlist object
					ttc,				// item class
					id,				// data
					NULL,
					ELM_GENLIST_ITEM_GROUP,
					_gl_sel,
					NULL);
			else
				item = elm_genlist_item_append(
					genlist,			// genlist object
					itc,				// item class
					id,				// data
					NULL,
					ELM_GENLIST_ITEM_NONE,
					_gl_sel,
					NULL);
		} else if(!strcmp(style, "title_with_groupindex")) {
			if (index == 0)
				item = elm_genlist_item_append(
					genlist,			// genlist object
					ttc,				// item class
					id,				// data
					NULL,
					ELM_GENLIST_ITEM_GROUP,
					_gl_sel,
					NULL);
			else if (index == 1)
				item = elm_genlist_item_append(
					genlist,			// genlist object
					gtc,				// item class
					id,				// data
					NULL,
					ELM_GENLIST_ITEM_GROUP,
					_gl_sel,
					NULL);
			else
				item = elm_genlist_item_append(
					genlist,			// genlist object
					itc,				// item class
					id,				// data
					NULL,
					ELM_GENLIST_ITEM_NONE,
					_gl_sel,
					NULL);
		} else if (!strcmp(style, "select_mode"))
			item = elm_genlist_item_append(
				genlist,				// genlist object
				itc,					// item class
				id,					// data
				NULL,
				ELM_GENLIST_ITEM_NONE,
				_gl_select_mode_sel,
				gvd);
		else
			item = elm_genlist_item_append(
				genlist,				// genlist object
				itc,					// item class
				id,					// data
				NULL,
				ELM_GENLIST_ITEM_NONE,
				_gl_sel,
				NULL);
		id->item = item;

		if (!strcmp(itc->item_style, "groupindex"))
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(gtc);
	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ptc);


	if (!strcmp(style, "select_mode"))
		elm_naviframe_item_push(ad->nf, NULL, NULL, NULL, gvd->layout, "empty");
	else if (!strcmp(style, "1text")) {
		Elm_Object_Item *it;
		it = elm_naviframe_item_push(ad->nf, NULL, NULL, NULL, genlist, "empty");
		elm_naviframe_item_pop_cb_set(it, _nf_pop_cb, genlist);
	} else
		elm_naviframe_item_push(ad->nf, NULL, NULL, NULL, genlist, "empty");
}

void
eext_genlist_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = NULL;
	Evas_Object *genlist = NULL;
	Evas_Object *circle_genlist;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	Item_Data *id;
	int i = 0;

	ad = (appdata_s *)data;
	if (ad == NULL) return;

	genlist = elm_genlist_add(ad->nf);
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

	for (i = 0; menu_its[i]; i++) {
		id = calloc(sizeof(Item_Data), 1);
		id->index = i;
		id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _create_genlist, ad);
	}

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ptc);

	elm_naviframe_item_push(ad->nf, NULL, NULL, NULL, genlist, "empty");
}
