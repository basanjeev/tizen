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

typedef struct _Progressbar_Data
{
   Ecore_Timer *p_timer_1;
   Ecore_Timer *p_timer_2;
   Evas_Object *layout;
   Evas_Object *progressbar1;
   Evas_Object *progressbar2;
} Progressbar_Data;

static Eina_Bool
_progressbar_timer_cb_1(void *data)
{
	double progress_1;
	char buf[PATH_MAX];
	Progressbar_Data *pd = (Progressbar_Data *)data;

	/* Get current progress value to increase */
	progress_1 = eext_circle_object_value_get(pd->progressbar1);
	progress_1 += 5.0;

	if (progress_1 > 100)
		progress_1 = 0.0;

	/* Set increased value as new progress */
	eext_circle_object_value_set(pd->progressbar1, progress_1);

	snprintf(buf, sizeof(buf), "%d %%", (int) progress_1);
	elm_object_part_text_set(pd->layout, "elm.text.progressbar1", buf);

	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_progressbar_timer_cb_2(void *data)
{
	double progress_2;
	char buf[PATH_MAX];
	Progressbar_Data *pd = (Progressbar_Data *)data;

	/* Get current progress value to increase */
	progress_2 = eext_circle_object_value_get(pd->progressbar2);
	progress_2 += 5.0;

	if (progress_2 > 100)
		progress_2 = 0.0;

	/* Set increased value as new progress */
	eext_circle_object_value_set(pd->progressbar2, progress_2);

	snprintf(buf, sizeof(buf), "%d %%", (int) progress_2);
	elm_object_part_text_set(pd->layout, "elm.text.progressbar2", buf);

	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_naviframe_item_pop_cb(void *data, Elm_Object_Item *it)
{
	Progressbar_Data *pd = (Progressbar_Data *)data;

	if (pd->p_timer_1) {
        ecore_timer_del(pd->p_timer_1);
        pd->p_timer_1 = NULL;
	}
	if (pd->p_timer_2)
	{
		ecore_timer_del(pd->p_timer_2);
		pd->p_timer_2 = NULL;
	}

	return EINA_TRUE;
}

void
eext_progressbar_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	static Progressbar_Data pd = { NULL, };
	Evas_Object *layout = NULL;
	Evas_Object *progressbar1 = NULL;
	Evas_Object *progressbar2 = NULL;
	Elm_Object_Item *nf_it = NULL;

	pd.layout = layout = elm_layout_add(nf);
	elm_layout_file_set(layout, ELM_DEMO_EDJ, "eext_progressbar_layout");
	evas_object_show(layout);

	/* Add new eext_circle_object_progressbar with Eext_Circle_Surface argument as NULL.
	Since the circle_object is not connected to Eext_Circle_Surface object,
	it will be rendered independently along with Evas_Object and set as a content of layout. */
	pd.progressbar1 = progressbar1 = eext_circle_object_progressbar_add(layout, NULL);
	elm_object_part_content_set(layout, "elm.swallow.content", progressbar1);

	/* Set the circle object's properties.
	- circle object's value has a range from 0.0 to 100.0
	- Circle object has a shape of Red(R: 255, G:0, B: 0, A: 0)
	and 15-width circle line with radius of 70 */
	eext_circle_object_value_min_max_set(progressbar1, 0.0, 100.0);
	eext_circle_object_radius_set(progressbar1, 70);
	eext_circle_object_color_set(progressbar1, 255, 0, 0, 255);
	eext_circle_object_line_width_set(progressbar1, 15);
	evas_object_show(progressbar1);

	pd.p_timer_1 = ecore_timer_add(0.2, _progressbar_timer_cb_1, &pd);

	/* Add new eext_circle_object_progressbar.
	Since the circle_object is connected to Eext_Circle_Surface object,
	it will be rendered by Eext_Circle_Surface object */
	pd.progressbar2 = progressbar2 = eext_circle_object_progressbar_add(layout, ad->circle_surface);

	/* Set the circle object's properties.
	- circle object's value has a range from 0.0 to 100.0 */
	eext_circle_object_value_min_max_set(progressbar2, 0.0, 100.0);
	evas_object_show(progressbar2);

	pd.p_timer_2 = ecore_timer_add(0.1, _progressbar_timer_cb_2, &pd);

	nf_it = elm_naviframe_item_push(nf, _("Progress Fullscreen"), NULL, NULL, layout, "empty");
	elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
	elm_naviframe_item_pop_cb_set(nf_it, _naviframe_item_pop_cb, &pd);
}
