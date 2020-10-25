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

#include "geniviocf.h"

#define ROTARY_SELECTOR_PAGE_COUNT 1

#define NUM_ITEMS             2
#define NUM_INDEX             6
#define NUM_ITEMS_CIRCLE_EVEN 5
#define NUM_INDEX_CIRCLE_EVEN 6

const char * users[] = {
		"User A", "User B"
};
const char *scenes[] = {
		"Leaving Home", "Coming Home"
};

gesture_s gestures[] = {
		{TAP1, 				ELM_GESTURE_N_TAPS, 			"One Tap", 			"Head Lights", 		ICON_DIR"/gest0"},
		{TAP2, 				ELM_GESTURE_N_DOUBLE_TAPS, 		"Two Tap", 			"Door Lock", 		ICON_DIR"/gest1"},
		{TAP3, 				ELM_GESTURE_N_TRIPLE_TAPS,		"Three Tap",		"Door Unnlock", 	ICON_DIR"/gest2"},
		{LINELEFT, 			ELM_GESTURE_N_LINES,			"Swipe Left", 		"Roof Close", 		ICON_DIR"/gest3"},
		{LINERIGHT, 		ELM_GESTURE_N_LINES,			"Swipe Right", 		"Roof Open", 		ICON_DIR"/gest4"}
//		{LINEUP, 			ELM_GESTURE_N_LINES,			"Swipe Up",			"Raise Windows", 	ICON_DIR"/gest5"},
//		{LINEDOWN, 			ELM_GESTURE_N_LINES,			"Swipe Down", 		"Lower Windows", 	ICON_DIR"/gest6"}
};

static int scene_changed = 1;

typedef struct _item_data
{
	int index;
	Elm_Object_Item *item;
} item_data;


static Evas_Event_Flags
handle_event_start(void *data, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s_1.png", gestures[ad->master.gesture].bg);
	dlog_print(DLOG_INFO, LOG_TAG,  "BG Image %s", buf);
	elm_bg_file_set(ad->gestbg, buf, NULL);
}

static Evas_Event_Flags
handle_event_end(void *data, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Elm_Gesture_Line_Info *l;
	if(ad->master.gesture >= LINELEFT && ad->master.gesture <= LINEDOWN ){
		l = (Elm_Gesture_Line_Info *) event_info;
		dlog_print(DLOG_INFO, LOG_TAG,  "Gesture %s Angle %1f", gestures[ad->master.gesture].cause, l->angle);

		double start = 0.0;
		double end = 0.0;
		double offset = 0.0;


		switch(ad->master.gesture){
			case LINELEFT:
				start = 220.0; end = 300.0;
				break;
			case LINERIGHT:
				start = 60.0; end = 120.0;
				break;
			case LINEUP:
				start = 310.0; end = 400.0; offset = 360.0;
				break;
			case LINEDOWN:
				start = 150.0; end = 230.0;
				break;
		}
		if((l->angle + offset)<=start || (l->angle + offset)>=end){
			dlog_print(DLOG_INFO, LOG_TAG,  "Wrong Gesture %f %f", start, end);
			return EVAS_EVENT_FLAG_ON_HOLD;
		}
	}else{
		//t = (Elm_Gesture_Taps_Info *) event_info;
		dlog_print(DLOG_INFO, LOG_TAG,  "Gesture %s", gestures[ad->master.gesture].cause);
		// post based onevent
	}

	char buf[1024];
	snprintf(buf, sizeof(buf), "%s_1.png", gestures[ad->master.gesture].bg);
	dlog_print(DLOG_INFO, LOG_TAG,  "BG Image %s", buf);
	elm_bg_file_set(ad->gestbg, buf, NULL);

	ad->master.res = get_res_by_restype(VEHICLECONTROL);
	ad->master.user = -1;
	ad->master.scene = -1;
#ifdef OCF
	postOCF(&ad->master);
#endif
	show_graphic_popup(ad, "Executing Gesture Control", buf, 2);
	dlog_print(DLOG_INFO, LOG_TAG, "Completed Posting %s", gestures[ad->master.gesture].cause);
	return EVAS_EVENT_FLAG_ON_HOLD;
}


void gesture_callbacks_set(appdata_s *ad, bool set){
	dlog_print(DLOG_INFO, LOG_TAG,  "%s gesture callbacks %s", (set)?"Setting" : "Clearing", __func__);
	//elm_gesture_layer_cb_set(ad->gl, gestures[ad->master.gesture].etype, ELM_GESTURE_STATE_START, (set)?handle_event_start:NULL, ad);
	elm_gesture_layer_cb_set(ad->gl, gestures[ad->master.gesture].etype, ELM_GESTURE_STATE_END,   (set)?handle_event_end:NULL, ad);
}

void handle_gesture(appdata_s *ad)
{
	ad->gestbg = elm_bg_add(ad->nf);
	elm_bg_option_set(ad->gestbg, ELM_BG_OPTION_STRETCH);
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s_0.png", gestures[ad->master.gesture].bg);
	dlog_print(DLOG_INFO, LOG_TAG,  "BG Image %s", buf);
	elm_bg_file_set(ad->gestbg, buf, NULL);
	evas_object_show(ad->gestbg);

	ad->gl = elm_gesture_layer_add(ad->gestbg);
	elm_gesture_layer_attach(ad->gl, ad->gestbg);
	gesture_callbacks_set(ad, true);

	ad->gestit = elm_naviframe_item_push(ad->nf, "Gestures Screen", NULL, NULL, ad->gestbg, "empty");
	elm_naviframe_item_title_enabled_set(ad->gestit, EINA_FALSE, EINA_FALSE);
    dlog_print(DLOG_INFO, LOG_TAG,  "%s", __func__);
}

static void
_index_clicked_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	int choice = 0;
	appdata_s *ad = (appdata_s *)data;
	if(scene_changed == 2){
		Evas_Object *image = (Evas_Object *)obj;
		const char *path=NULL;
		char *ptr;
		char buf[1024];
		elm_image_file_get(image, &path, NULL);
		dlog_print(DLOG_INFO, LOG_TAG, "Image is %s", path);
		scene_changed = 0;
		ptr =strstr((char *)path, "/scene");
		if(ptr){
			int len = strlen("/scene");
			ptr+=len;
			*(ptr+1) = '\0';
			choice = 100; //POST a NOTI
			ad->master.scene = atoi(ptr);
			dlog_print(DLOG_INFO, LOG_TAG, "SCENE %s", scenes[ad->master.scene]);
			sprintf(buf, "SCENE Changed to %s", scenes[ad->master.scene]);
			if(ad->master.scene)
				show_graphic_popup(ad, buf,ICON_DIR"/0_1.jpg", 2);
			else
				show_graphic_popup(ad, buf,ICON_DIR"/0_0.jpg", 2);
		}
		ptr =strstr((char *)path, "/user");
		if(ptr){
			int len = strlen("/user");
			ptr+=len;
			*(ptr+1) = '\0';
			dlog_print(DLOG_INFO, LOG_TAG, "Ptr %s", ptr);
			ad->master.user = atoi(ptr);
			dlog_print(DLOG_INFO, LOG_TAG, "User is %s", users[ad->master.user]);
			sprintf(buf, "%s selected", users[ad->master.user]);

			char imgpath[PATH_MAX];
			sprintf(imgpath, ICON_DIR"/%d.png", ad->master.user);
			dlog_print(DLOG_INFO, LOG_TAG, "Image is %s", imgpath);
			show_graphic_popup(ad, buf, imgpath, 2);
		}
		ptr =strstr((char *)path, "/gest");
		if(ptr){
			int len = strlen("/gest");
			ptr+=len;
			*(ptr+1) = '\0';
			dlog_print(DLOG_INFO, LOG_TAG, "Ptr %s", ptr);
			ad->master.gesture = (gesture_e)atoi(ptr);
			dlog_print(DLOG_INFO, LOG_TAG, "Gesture is %s", gestures[ad->master.gesture].cause);
			sprintf(buf, "Gesture  : %s for %s", gestures[ad->master.gesture].cause, gestures[ad->master.gesture].effect );
			handle_gesture(ad);
			return; // dont post from here - let gesture UI do it
		}

#ifdef OCF
		if(choice==100){
			char msg[1024];
			snprintf(msg, 1023, "%s Scenario for %s",  ((!ad->master.user)?"User A":"User B"), (!ad->master.scene)?"Leaving Home":"Coming Home");
			dlog_print(DLOG_INFO, LOG_TAG, msg);
			ad->master.res = get_res_by_restype(VEHICLECONTROL);
			ad->master.gesture = NOGESTURE;
			postOCF(&ad->master);
			dlog_print(DLOG_INFO, LOG_TAG, "Completed Posting");
		}
#endif
	}else if(scene_changed == 1){
		dlog_print(DLOG_INFO, LOG_TAG, "Ready for Scene %d", choice);
		scene_changed = 2;
	}else if(scene_changed == 0){
		scene_changed = 1;
	}
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
	dlog_print(DLOG_INFO, LOG_TAG, "Moved");

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
	dlog_print(DLOG_INFO, LOG_TAG, "refreshed");
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
	dlog_print(DLOG_INFO, LOG_TAG, "synced");
	scene_changed = 0;
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
		dlog_print(DLOG_INFO, LOG_TAG, "scroll: %d\n", pd->cur_page);
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
_create_view_layout(void *data, char *layout_file, char *index_style, char it_style[][20])
{
	Evas_Object *layout, *scroller, *box, *left_right_rect, *img, *page_layout, *index;
	char img_path[PATH_MAX];
	int i, max_items;

	appdata_s *ad = (appdata_s *)data;
	Evas_Object *parent = ad->nf;

	const char *choice = elm_object_item_part_text_get(ad->it, "elm.text");
	dlog_print(DLOG_INFO, LOG_TAG, "Printing default part text %s", choice);
	dlog_print(DLOG_INFO, LOG_TAG, "-----------------------------");

	page_data *pd = (page_data *)calloc(1, sizeof(page_data));

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

	Evas_Object *circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);


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
	scene_changed = 0;
	if(!strcmp("Gestures", choice))
		max_items = NUM_ITEMS_CIRCLE_EVEN;
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

		if(!strcmp("Select User", choice)){
			snprintf(img_path, sizeof(img_path), "%s/user%d.png", ICON_DIR, i);
			evas_object_smart_callback_add(img, "clicked", _index_clicked_cb , ad);
		}else if(!strcmp("Gestures", choice)){
			snprintf(img_path, sizeof(img_path), "%s/gest%d.png", ICON_DIR, i);
			evas_object_smart_callback_add(img, "clicked", _index_clicked_cb , ad);
		}else{
			snprintf(img_path, sizeof(img_path), "%s/scene%d.png", ICON_DIR, i);
			evas_object_smart_callback_add(img, "clicked", _index_clicked_cb , ad);
		}

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

	Elm_Object_Item *it = elm_naviframe_item_push(parent, NULL, NULL, NULL, layout, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
}



static void
_body_circle_even_queue_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	char it_style[NUM_ITEMS_CIRCLE_EVEN][20] = {
           "item/even_8",
           "item/even_9",
           "item/even_10",
           "item/even_11",
           "item/even_12",
	};

	ad->it = (Elm_Object_Item*) event_info;
	_create_view_layout(ad, (char *)"elmdemo-test/index_circle", (char *)"circle", it_style);
}

void index_cb(void *data, Evas_Object *obj, void *event_info)
{
	_body_circle_even_queue_cb(data, NULL, event_info); //pass the choice
}
