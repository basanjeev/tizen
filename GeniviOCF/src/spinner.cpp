#include <geniviocf.h>

Evas_Object *focused_obj = NULL;

static Eina_Bool
_spinner_naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
   eext_rotary_object_event_activated_set(focused_obj, EINA_FALSE);
   return EINA_TRUE;
}

static void
_btn_clicked(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *nf = (Evas_Object *)data;

   elm_naviframe_item_pop(nf);
}

static void _focused_cb(void *data, Evas_Object *obj, void *event_info)
{
   focused_obj = obj;
   eext_rotary_object_event_activated_set(obj, EINA_TRUE);
}

void show_spinner(void *data, char *units, double begin, double end, double incr, char *text)
{
   appdata_s *ad = (appdata_s *)data;
   Evas_Object *layout, *unit, *bottom_button, *circle_spinner;
   Elm_Object_Item *nf_it;

   layout = elm_layout_add(ad->nf);
   elm_layout_theme_set(layout, "layout", "circle", "spinner");

   evas_object_show(layout);

   unit = elm_spinner_add(layout);
   elm_object_style_set(unit,"circle");
   circle_spinner = eext_circle_object_spinner_add(unit, ad->circle_surface);
   eext_circle_object_spinner_angle_set(circle_spinner, 2.0);
   evas_object_show(unit);

   elm_spinner_wrap_set(unit, EINA_FALSE);
   elm_spinner_label_format_set(unit, "%1.1f");
   elm_spinner_min_max_set(unit, begin, end);
   elm_spinner_step_set(unit, incr);
   elm_object_part_text_set(unit, "elm.text", units);
   evas_object_smart_callback_add(unit, "focused", _focused_cb, unit);
   elm_object_part_content_set(layout, "elm.swallow.content", unit);

   bottom_button = elm_button_add(layout);
   elm_object_style_set(bottom_button, "bottom");
   elm_object_text_set(bottom_button, "SET");
   evas_object_smart_callback_add(bottom_button, "clicked", _btn_clicked, ad->nf);
   elm_object_part_content_set(layout, "elm.swallow.btn", bottom_button);

   elm_object_part_text_set(layout, "elm.text", text);
   nf_it = elm_naviframe_item_push(ad->nf, _("Spinner"), NULL, NULL, layout, NULL);
   elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
   elm_naviframe_item_pop_cb_set(nf_it, _spinner_naviframe_pop_cb, NULL);
}
