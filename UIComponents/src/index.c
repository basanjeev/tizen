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

#define NUM_ITEMS             5
#define NUM_INDEX             5
#define NUM_ITEMS_CIRCLE_EVEN 6
#define NUM_INDEX_CIRCLE_EVEN 6

char *pagecontrol_menu_names[] = {
	"Thumbnail", "Circle Even Queue", "Circle Odd Queue",
	NULL
};

typedef struct _item_data
{
	int index;
	Elm_Object_Item *item;
} item_data;

static void
_gl_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(it, EINA_FALSE);
}

static char *
_gl_menu_title_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];

	snprintf(buf, 1023, "%s", "PageControl");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, 1023, "%s", pagecontrol_menu_names[index]);
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

typedef struct _page_data page_data;
struct _page_data
{
	Evas_Object *main_layout;
	Evas_Object *scroller;
	Evas_Object *box;
	Evas_Object *mapbuf[NUM_ITEMS_CIRCLE_EVEN];
	Evas_Object *index;
	Evas_Object *page_layout[NUM_ITEMS_CIRCLE_EVEN];
	Evas_Object *left_right_rect;
	int cur_page;
	int prev_page;
	Elm_Object_Item *it[NUM_ITEMS_CIRCLE_EVEN];

	Elm_Object_Item *last_it;
	Elm_Object_Item *new_it;
	int min_page, max_page;
};

static void _index_sync(void *data);

static void
_on_index_mouse_down_cb(void *data, Evas *e, Evas_Object *o, void *event_info)
{
	page_data *pd = (page_data *)data;

	/* Keep the last index item active and save the selected index item */
	if (!pd->last_it) return;

	int level = elm_index_item_level_get(o);
	pd->new_it = elm_index_selected_item_get(o, level);
	elm_index_item_selected_set(pd->last_it, EINA_TRUE);
}

static void
_on_index_mouse_up_cb(void *data, Evas *e, Evas_Object *o, void *event_info)
{
	page_data *pd = (page_data *)data;

	/* Keep the last index item active and move to the page of the currently selected index item */
	if (!pd->last_it) return;
	elm_index_item_selected_set(pd->last_it, EINA_TRUE);

	if (!pd->new_it) return;

	int idx = (int) elm_object_item_data_get(pd->new_it);
	if (idx == pd->cur_page) return;

	elm_scroller_page_bring_in(pd->scroller, idx, 0);
}

static void
_on_index_mouse_move_cb(void *data, Evas *e, Evas_Object *o, void *event_info)
{
	page_data *pd = (page_data *)data;

	/* Keep the last index item active and save the currently selected index item */
	if (!pd->last_it) return;

	int level = elm_index_item_level_get(o);
	pd->new_it = elm_index_selected_item_get(o, level);
	elm_index_item_selected_set(pd->last_it, EINA_TRUE);
}

static void
_index_refresh(void *data)
{
	int i, num_index;
	page_data *pd = (page_data *)data;

        num_index = pd->max_page + 1;

	elm_index_item_clear(pd->index);
	if (pd->cur_page < pd->min_page)
	{
		for(i = pd->cur_page ; i < pd->cur_page + num_index ; i++)
		{
			printf("Added : %d\n", i);
			elm_index_item_append(pd->index, NULL, NULL, (void *) i);
		}
		pd->min_page = pd->cur_page;
		pd->min_page = pd->cur_page + num_index - 1;
	}
	else
	{
		for(i = pd->cur_page - num_index + 1; i < pd->cur_page + 1 ; i++)
		{
			printf("Added : %d\n", i);
			elm_index_item_append(pd->index, NULL, NULL, (void *) i);
		}
		pd->min_page = pd->cur_page - num_index;
		pd->min_page = pd->cur_page - 1;
	}
	elm_index_level_go(pd->index, 0);
	_index_sync(pd);
}

static void
_index_sync(void *data)
{
	page_data *pd = (page_data *)data;
	Elm_Object_Item *it;
	it = elm_index_item_find(pd->index, (void *)pd->cur_page);
	if (it) {
		elm_index_item_selected_set(it, EINA_TRUE);
		pd->last_it = it;
		pd->new_it = it;
	}
	else
		_index_refresh(pd);
}

static void
_layout_del_cb(void *data , Evas *e, Evas_Object *obj, void *event_info)
{
	page_data *pd = (page_data *)data;
	free(pd);
}

static void
_layout_resize_cb(void *data , Evas *e, Evas_Object *obj, void *event_info)
{
	int w1, h1, w2, h2;
	page_data *pd = (page_data *)data;

	evas_object_geometry_get(obj, NULL, NULL, &w1, &h1);
	evas_object_geometry_get(pd->main_layout, NULL, NULL, &w2, &h2);

	evas_object_size_hint_min_set(pd->left_right_rect, (w2 - w1) / 2, h2 / 2);
	elm_scroller_page_size_set(pd->scroller, w1, h1);
}

static Eina_Bool
animator_cb(void *data)
{
	int i, num_items;
	page_data *pd = (page_data *)data;

        num_items = pd->max_page + 1;

	//Since mapbuf is not perfect, Enable them after the size calculation is finished
	for (i = 0; i < num_items; ++i)
	{
		elm_mapbuf_enabled_set(pd->mapbuf[i], EINA_TRUE);
	}
	return ECORE_CALLBACK_CANCEL;
}

static void
_scroll(void *data, Evas_Object *obj, void *ei)
{
	int cur_page;
	page_data *pd = (page_data *)data;
	elm_scroller_current_page_get(pd->scroller, &cur_page, NULL);
	if (cur_page != pd->cur_page) {
		printf("scroll: %d\n", pd->cur_page);
		pd->prev_page = pd->cur_page;
		pd->cur_page = cur_page;
		if ((pd->cur_page >= NUM_ITEMS_CIRCLE_EVEN) || (pd->prev_page >= NUM_ITEMS_CIRCLE_EVEN))
			return;
		elm_object_signal_emit(pd->page_layout[pd->cur_page], "elm,state,thumbnail,select", "elm");
		elm_object_signal_emit(pd->page_layout[pd->prev_page], "elm,state,thumbnail,unselect", "elm");
		_index_sync(pd);
	}
}

static void
_create_view_layout(Evas_Object *parent, char *layout_file, char *index_style, char it_style[][20], Eina_Bool even_num)
{
	Evas_Object *layout, *scroller, *box, *left_right_rect, *img, *page_layout, *index;
	Elm_Object_Item *nf_it;
	char img_path[PATH_MAX];
	int i, max_items;

	page_data *pd = calloc(1, sizeof(page_data));

	/* Create Layout */
	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, layout_file);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);
	evas_object_event_callback_add(layout, EVAS_CALLBACK_DEL, _layout_del_cb, pd);
	pd->main_layout = layout;

	/* Create Scroller */
	scroller = elm_scroller_add(layout);
	elm_scroller_loop_set(scroller, EINA_FALSE, EINA_FALSE);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set(scroller, 1, 0);
	elm_object_scroll_lock_y_set(scroller, EINA_TRUE);
	elm_object_part_content_set(layout, "scroller", scroller);
	evas_object_show(scroller);
	evas_object_smart_callback_add(scroller, "scroll", _scroll, pd);
	pd->scroller = scroller;

	/* Create Box */
	box = elm_box_add(scroller);
	elm_box_horizontal_set(box, EINA_TRUE);
	elm_object_content_set(scroller, box);
	evas_object_show(box);
	pd->box = box;

	/* Create Rect object for page_layout center align in Box */
	left_right_rect = evas_object_rectangle_add(evas_object_evas_get(box));
	elm_box_pack_end(box, left_right_rect);

	/* Create Pages */
	if (even_num)
		max_items = NUM_INDEX_CIRCLE_EVEN;
	else
		max_items = NUM_ITEMS;
	for (i = 0; i < max_items; ++i)
	{
		page_layout = elm_layout_add(box);
		pd->page_layout[i] = page_layout;
		evas_object_size_hint_weight_set(page_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(page_layout, 0, EVAS_HINT_FILL);
		elm_layout_theme_set(page_layout, "layout", "body_thumbnail", "default");
		evas_object_show(page_layout);

		img = elm_image_add(page_layout);
		snprintf(img_path, sizeof(img_path), "%s/100_%d.jpg", ICON_DIR, (i % 6) + 1);
		elm_image_file_set(img, img_path, NULL);
		elm_object_part_content_set(page_layout, "elm.icon", img);

		/* Make unselect state all of the pages except first one */
		if (i)
			elm_object_signal_emit(page_layout, "elm,state,thumbnail,unselect", "elm");

		elm_box_pack_end(box, page_layout);
	}

	/*Add resize callback for get a actual size of layout and main layout */
	evas_object_event_callback_add(pd->page_layout[0], EVAS_CALLBACK_RESIZE, _layout_resize_cb, pd);

	elm_box_pack_end(box, left_right_rect);
	pd->left_right_rect = left_right_rect;

	/* Add animator for page transfer effect */
	ecore_animator_add(animator_cb, pd);

	/* Create Index */
	index = elm_index_add(layout);
	elm_object_style_set(index, index_style);
	evas_object_size_hint_weight_set(index, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(index, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_index_horizontal_set(index, EINA_TRUE);
	elm_index_autohide_disabled_set(index, EINA_TRUE);
	elm_object_part_content_set(layout, "controller", index);

	for (i = 0; i < max_items; ++i)
	{
		pd->it[i] = elm_index_item_append(index, NULL, NULL, (void *) i);
		if (it_style)
			elm_object_item_style_set(pd->it[i], it_style[i]);
	}

	pd->min_page = 0;
	pd->max_page = max_items - 1;
	elm_index_level_go(index, 0);
	elm_index_item_selected_set(pd->it[0], EINA_TRUE);

	pd->cur_page = 0;
	pd->index = index;
	pd->last_it = pd->it[0];

	evas_object_event_callback_add(index, EVAS_CALLBACK_MOUSE_DOWN, _on_index_mouse_down_cb, pd);
	evas_object_event_callback_add(index, EVAS_CALLBACK_MOUSE_MOVE, _on_index_mouse_move_cb, pd);
	evas_object_event_callback_add(index, EVAS_CALLBACK_MOUSE_UP, _on_index_mouse_up_cb, pd);

	nf_it = elm_naviframe_item_push(parent, NULL, NULL, NULL, layout, NULL);
	elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
}

static void
_body_thumbnail_queue_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *nf = (Evas_Object *)data;

	_create_view_layout(nf, "elmdemo-test/index_thumbnail", "thumbnail", NULL, EINA_FALSE);
}

static void
_body_circle_even_queue_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *nf = (Evas_Object *)data;
	char it_style[NUM_INDEX_CIRCLE_EVEN][20] = {
           "item/even_8",
           "item/even_9",
           "item/even_10",
           "item/even_11",
           "item/even_12",
           "item/even_13",
	};

	_create_view_layout(nf, "elmdemo-test/index_circle", "circle", it_style, EINA_TRUE);
}

static void
_body_circle_odd_queue_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *nf = (Evas_Object *)data;
	char it_style[NUM_INDEX][20] = {
           "item/odd_8",
           "item/odd_9",
           "item/odd_10",
           "item/odd_11",
           "item/odd_12",
	};

	_create_view_layout(nf, "elmdemo-test/index_circle", "circle", it_style, EINA_FALSE);
}

static Evas_Object*
_create_genlist(appdata_s *ad)
{
	Evas_Object *genlist;
	Evas_Object *circle_genlist;
	Evas_Object *parent = ad->nf;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	int index = 0;

	genlist = elm_genlist_add(parent);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "selected", _gl_selected_cb, NULL);

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
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _body_thumbnail_queue_cb, parent);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _body_circle_even_queue_cb, parent);
	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _body_circle_odd_queue_cb, parent);

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	return genlist;
}

void index_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *genlist;

	genlist = _create_genlist(ad);

	elm_naviframe_item_push(ad->nf, "Scroll", NULL, NULL, genlist, "empty");
}
