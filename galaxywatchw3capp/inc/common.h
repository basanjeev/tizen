/*
 * common.h
 *
 *  Created on: Feb 10, 2020
 *      Author: sanjeev
 */

#ifndef COMMON_H_
#define COMMON_H_

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "W3CVSS"

#if !defined(PACKAGE)
#define PACKAGE "org.w3c.galaxywatchw3capp"
#endif

#include <Elementary.h>
#include <efl_extension.h>

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
	int val;
} appdata_s;


#endif /* COMMON_H_ */
