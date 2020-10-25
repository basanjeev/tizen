#include "geniviocf.h"


static void _radio_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	ad->master.user = (0==elm_radio_value_get(obj))?false:true;
	dlog_print(DLOG_INFO, LOG_TAG,  "User change %d", ad->master.user);
}

void show_user_screen(void *data){
	appdata_s *ad;
	Evas_Object *box = NULL;
	Evas_Object *box1 = NULL;
	Evas_Object *box2 = NULL;
	Evas_Object *rdg = NULL;
	//int i;

	if (!data) return;
	ad = (appdata_s *)data;

	box = elm_box_add(ad->nf);
	elm_box_horizontal_set(box, EINA_FALSE);
	evas_object_show(box);

	box1 = elm_box_add(box);
	elm_box_horizontal_set(box1, EINA_TRUE);
	evas_object_show(box1);
	elm_box_pack_end(box, box1);
	box2 = elm_box_add(box);
	elm_box_horizontal_set(box2, EINA_TRUE);
	evas_object_show(box2);
	elm_box_pack_end(box, box2);

	Evas_Object *title = elm_label_add(box);
	elm_object_text_set(title, ad->master.res->name);
	evas_object_show(title );
	elm_box_pack_start(box, title );

	char buf[1024];
	snprintf(buf, 1023, "%s/u0.png", ICON_DIR);
	Evas_Object *onti = elm_image_add(box1);
	elm_image_file_set(onti, buf, "");
	evas_object_show(onti);
	Evas_Object *ont = elm_label_add(box1);
	elm_object_text_set(ont, "User A       ");
	evas_object_show(ont);
	elm_box_pack_end(box1, ont);
	snprintf(buf, 1023, "%s/u1.png", ICON_DIR);
	Evas_Object *offti = elm_image_add(box2);
	elm_image_file_set(offti, buf, "");
	evas_object_show(offti);
	Evas_Object *offt = elm_label_add(box2);
	elm_object_text_set(offt, "User B        ");
	evas_object_show(offt);
	elm_box_pack_end(box2, offt);


	//FIXME initial value
	Evas_Object *on = elm_radio_add(box1);
	elm_check_state_set(on, EINA_FALSE);
	elm_radio_state_value_set(on, 0);
	evas_object_smart_callback_add(on, "changed", _radio_clicked_cb, ad);
	evas_object_show(on);
	elm_box_pack_end(box1, on);
	rdg = on;
	Evas_Object *off = elm_radio_add(box2);
	elm_radio_state_value_set(off, 1);
	evas_object_smart_callback_add(off, "changed", _radio_clicked_cb, ad);
	evas_object_show(off);
	elm_radio_group_add(off, rdg);
	elm_box_pack_end(box2, off);

	Elm_Object_Item *it = elm_naviframe_item_push(ad->nf, _("User"), NULL, NULL, box, NULL);
	elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
}
