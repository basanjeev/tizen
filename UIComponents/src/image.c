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

#define IMAGE_WIDTH 1024
#define IMAGE_HEIGHT 768
#define BUF_SIZE IMAGE_WIDTH * IMAGE_HEIGHT * 4

static void
_image1_clicked_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *image = data;
	Elm_Image_Orient orient;
	int w,h;

	orient = elm_image_orient_get(image);

	if (orient >= 7)
		orient = 0;
	else
		orient++;

	elm_image_object_size_get(image , &w, &h);
	dlog_print(DLOG_INFO, LOG_TAG, "image clicked! orient = %d , width = %d , height = %d\n", orient, w, h);
	elm_image_orient_set(image , orient);
	evas_object_show(image);
}

static void
_image2_clicked_cb(void *data , Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *image = data;
	Eina_Bool smooth = EINA_FALSE;
	static int scale_num = 0;
	int w,h;
	int prescale = 0;

	smooth = !elm_image_smooth_get(image);

	if (scale_num > 2)
	{
		scale_num = 0;
		elm_image_no_scale_set(image, EINA_TRUE);
	}

	elm_image_object_size_get( image , &w, &h);

	scale_num++;
	dlog_print(DLOG_INFO, LOG_TAG, "image clicked! smooth = %d, width = %d , height = %d , prescale = %d  \n", smooth, w, h, prescale );
	elm_image_smooth_set(image, smooth);
	evas_object_show(image);
}
static void
_image3_clicked_cb(void *data , Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *image = data;
	int w, h;

	elm_image_object_size_get(image , &w, &h);
	dlog_print(DLOG_INFO, LOG_TAG, "image clicked! width = %d , height = %d \n", w, h);
}

static void
_image4_clicked_cb(void *data , Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *image = data;
	Eina_Bool play = EINA_FALSE;
	int w,h;

	elm_image_object_size_get(image , &w, &h);

	play = !elm_image_animated_play_get(image);
	dlog_print(DLOG_INFO, LOG_TAG, "image clicked! play = %d width = %d, height = %d \n", play, w, h);
	elm_image_animated_play_set(image, play);
}

void
image_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *naviframe = ad->nf;
	Evas_Object *scroller, *circle_scroller, *layout;
	Evas_Object *image1;
	Evas_Object *image2;
	Evas_Object *image3;
	Evas_Object *image4;
	Evas_Object *image_jpg;
	Evas_Object *image_bmp;
	int size;
	char image_file_buf[BUF_SIZE];
	char buf[256];
	FILE *fp;

	scroller = elm_scroller_add(naviframe);
	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);

	circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	layout = elm_layout_add(scroller);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "image_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	image1 = elm_image_add(layout);

	snprintf(buf, sizeof(buf), "%s/png_image.png",ICON_DIR);
	elm_image_file_set(image1, buf, NULL);
	elm_object_part_content_set(layout, "image1", image1);
	elm_image_no_scale_set(image1, EINA_TRUE);
	elm_image_resizable_set(image1, EINA_FALSE, EINA_FALSE);
	elm_image_smooth_set(image1, EINA_FALSE);
	elm_image_aspect_fixed_set(image1, EINA_TRUE);
	elm_image_fill_outside_set(image1, EINA_TRUE);
	elm_image_editable_set(image1, EINA_TRUE);

	image2 = elm_image_add(layout);
	elm_image_prescale_set(image2, 2);
	snprintf(buf, sizeof(buf), "%s/grid_image/1_raw.png",ICON_DIR);
	elm_image_file_set(image2, buf, NULL);
	elm_object_part_content_set(layout, "image2", image2);
	elm_image_resizable_set(image2, EINA_TRUE, EINA_FALSE);
	elm_image_smooth_set(image2, EINA_FALSE);
	elm_image_aspect_fixed_set(image2, EINA_TRUE);
	elm_image_fill_outside_set(image2, EINA_TRUE);
	elm_image_editable_set(image2, EINA_TRUE);

	image3 = elm_image_add(layout);
	snprintf(buf, sizeof(buf), "%s/grid_image/2_raw.png",ICON_DIR);

	/* File Open */
	fp = fopen(buf, "rb");
	fseek(fp, 0, SEEK_END);
	size = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(image_file_buf, 1, size, fp);

	elm_image_memfile_set(image3, &image_file_buf, size, "png", NULL);
	elm_object_part_content_set(layout, "image3", image3);
	elm_image_no_scale_set(image3, EINA_TRUE);
	elm_image_resizable_set(image3, EINA_FALSE, EINA_TRUE);
	elm_image_smooth_set(image3, EINA_FALSE);
	elm_image_aspect_fixed_set(image3, EINA_TRUE);
	elm_image_fill_outside_set(image3, EINA_FALSE);
	elm_image_editable_set(image3, EINA_TRUE);

	image4 = elm_image_add(layout);
	snprintf(buf, sizeof(buf), "%s/animated_logo.gif", ICON_DIR);
	elm_image_file_set(image4, buf, NULL);
	if (elm_image_animated_available_get(image4))
	{
		elm_image_animated_set(image4, EINA_TRUE);
		elm_image_animated_play_set(image4, EINA_TRUE);
	}
	elm_object_part_content_set(layout, "image_gif", image4);
	elm_image_resizable_set(image4, EINA_FALSE, EINA_FALSE);
	elm_image_smooth_set(image4, EINA_FALSE);
	elm_image_aspect_fixed_set(image4, EINA_TRUE);
	elm_image_fill_outside_set(image4, EINA_TRUE);
	elm_image_editable_set(image4, EINA_TRUE);
	elm_object_content_set(scroller, layout);

	image_jpg = elm_image_add(layout);
	snprintf(buf, sizeof(buf), "%s/jpg_image.jpg", ICON_DIR);
	elm_image_file_set(image_jpg, buf, NULL);
	elm_object_part_content_set(layout, "image_jpg", image_jpg);
	elm_image_no_scale_set(image_jpg, EINA_TRUE);
	elm_image_resizable_set(image_jpg, EINA_FALSE, EINA_TRUE);
	elm_image_smooth_set(image_jpg, EINA_FALSE);
	elm_image_aspect_fixed_set(image_jpg, EINA_TRUE);
	elm_image_fill_outside_set(image_jpg, EINA_FALSE);
	elm_image_editable_set(image_jpg, EINA_TRUE);

	image_bmp = elm_image_add(layout);
	snprintf(buf, sizeof(buf), "%s/bmp_image.bmp", ICON_DIR);
	elm_image_file_set(image_bmp, buf, NULL);
	elm_object_part_content_set(layout, "image_bmp", image_bmp);
	elm_image_no_scale_set(image_bmp, EINA_TRUE);
	elm_image_resizable_set(image_bmp, EINA_FALSE, EINA_TRUE);
	elm_image_smooth_set(image_bmp, EINA_FALSE);
	elm_image_aspect_fixed_set(image_bmp, EINA_TRUE);
	elm_image_fill_outside_set(image_bmp, EINA_FALSE);
	elm_image_editable_set(image_bmp, EINA_TRUE);

	evas_object_smart_callback_add(image1, "clicked", _image1_clicked_cb , image1);
	evas_object_smart_callback_add(image1, "clicked", _image2_clicked_cb , image1);
	evas_object_smart_callback_add(image3, "clicked", _image3_clicked_cb , image3);
	evas_object_smart_callback_add(image4, "clicked", _image4_clicked_cb , image4);

	elm_naviframe_item_push(naviframe, "Image", NULL, NULL, scroller, "empty");
}
