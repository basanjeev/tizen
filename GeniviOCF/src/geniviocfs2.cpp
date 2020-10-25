#include "geniviocfs2.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <map>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "OCPlatform.h"
#include "OCApi.h"
#include <Elementary.h>

using namespace OC;
using namespace std;
namespace PH = std::placeholders;
void  send_message(rvidata *p);

typedef std::map<OCResourceIdentifier, std::shared_ptr<OCResource>> DiscoveredResourceMap;
DiscoveredResourceMap discoveredResources;
std::shared_ptr<OCResource> curResource;
int ocfinitialized = 0;

Client myclient;
static char *icon_path_list[] = {
	ICON_DIR"/refresh.png",
	ICON_DIR"/location.png",
	ICON_DIR"/lt.png",
	ICON_DIR"/rt.png",
	ICON_DIR"/lhs.png",
	ICON_DIR"/rhs.png",
	ICON_DIR"/fs.png",
	ICON_DIR"/fd.png",
	ICON_DIR"/fr.png",
	ICON_DIR"/fu.png",
	ICON_DIR"/fac.png",
	ICON_DIR"/fa.png",
	ICON_DIR"/fan.png",
	ICON_DIR"/dm.png",
	ICON_DIR"/df.png",
	ICON_DIR"/dr.png",
	NULL
};

static char *icon_name_list[] = {
	"Refresh",
	"Location",
	"Left Temp. (15-30)",
	"Right Temp.(15-30)",
	"Left Seat Heat (0-3)",
	"Right Seat Heat(0-3)",
	"Fan Speed (0-100)",
	"Fan Down (0/1)",
	"Fan Right (0/1)",
	"Fan Up (0/1)",
	"Fan A/C (0/1)",
	"Fan Auto (0/1)",
	"Fan Recirculate (0/1)",
	"DeFrost Max (0/1)",
	"DeFrost Front (0/1)",
	"DeFrost Rear (0/1)",
	NULL
};

map<string, int> intprops = {
	{"leftTemperature", 0},
	{"rightTemperature", 0},
	{"leftSeatHeat", 0},
	{"rightSeatHeat", 0},
	{"fanSpeed", 0},
	{"fanDown", 0},
	{"fanRight", 0},
	{"fanUp", 0},
	{"fanAC", 0},
	{"fanAuto", 0},
	{"fanRecirc", 0},
	{"defrostMax", 0},
	{"defrostFront", 0},
	{"defrostRear", 0}
};

map<string, double> doubleprops = {
	{"lat", 0},
	{"lon", 0},
	{"bearing", 0}
};

char *popup_menu_names[] = {"Lat %f", "Long %f", "Bearing %f"};


static FILE* client_open(const char* /*path*/, const char *mode)
{
//	/return fopen("./oic_svr_db_client.json", mode);
	return NULL;
}


void foundResource(std::shared_ptr<OCResource> resource)
{
	std::cout << "In foundResource\n";
	std::string resourceURI;
	std::string hostAddress;
	try
	{
		if(discoveredResources.find(resource->uniqueIdentifier()) == discoveredResources.end())
		{
			std::cout << "Found resource " << resource->uniqueIdentifier() <<
				" for the first time on server with ID: "<< resource->sid()<<std::endl;
			discoveredResources[resource->uniqueIdentifier()] = resource;
		}
		else
		{
			std::cout<<"Found resource "<< resource->uniqueIdentifier() << " again!"<<std::endl;
		}

		if(curResource)
		{
			std::cout << "Found another resource, ignoring"<<std::endl;
			return;
		}
		if(resource)
		{
			std::cout<<"DISCOVERED Resource:"<<std::endl;
			resourceURI = resource->uri();
			std::cout << "\tURI of the resource: " << resourceURI << std::endl;
			hostAddress = resource->host();
			std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
			std::cout << "\tList of resource types: " << std::endl;
			for(auto &resourceTypes : resource->getResourceTypes())
			{
				std::cout << "\t\t" << resourceTypes << std::endl;
			}
			std::cout << "\tList of resource interfaces: " << std::endl;
			for(auto &resourceInterfaces : resource->getResourceInterfaces())
			{
				std::cout << "\t\t" << resourceInterfaces << std::endl;
			}
			if(resourceURI == "/a/rvi")
			{
				curResource = resource;
				dlog_print(DLOG_INFO, LOG_TAG,  "RVI resource detected");
				ocfinitialized = 1;
			}
		}
		else
		{
			std::cout << "Resource is invalid" << std::endl;
		}
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception in foundResource: "<< e.what() << std::endl;
	}
	std::cout << "-----------------------Exit foundResource\n";
}



void initOCFStack()
{
	std::ostringstream requestURI;
	OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };
	PlatformConfig cfg {
		OC::ServiceType::InProc,
			OC::ModeType::Client,
			"0.0.0.0",
			0,
			OC::QualityOfService::LowQos,
			&ps
	};
	OCPlatform::Configure(cfg);
	try
	{
		std::cout.setf(std::ios::boolalpha);
		requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=core.rvi";
		OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResource);
		std::cout<< "Finding Resource... " <<std::endl;
	}catch(OCException& e)
	{
		oclog() << "Exception in main: "<<e.what();
	}
}

void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
	try
	{
		if(eCode == OC_STACK_OK)
		{
			dlog_print(DLOG_INFO, LOG_TAG,  "PUT request was successful");
		}
		else
		{
			dlog_print(DLOG_INFO, LOG_TAG,  "PUT request was UNsuccessful");
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
	}
}

void putClientRepresentation(std::shared_ptr<OCResource> resource, string &key, int value)
{
	if(resource)
	{
		OCRepresentation rep;
		std::cout << "Putting representation..."<<std::endl;
		rep.setValue(key, value);
		resource->put(rep, QueryParamsMap(), &onPut);
	}
}

void onGet(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
	try
	{
		if(eCode == OC_STACK_OK)
		{
			dlog_print(DLOG_INFO, LOG_TAG,  "GET request was successful %s", rep.getUri().c_str());
			std::cout << "Attributes : " << rep.numberOfAttributes() << std::endl;
			typedef std::map<std::string, int>::iterator it_type;
			for(it_type iterator = intprops.begin(); iterator != intprops.end(); iterator++) {
				std::cout << "Key  : " << iterator->first << " Value = "<<rep[iterator->first]<<std::endl;
				iterator->second = rep[iterator->first];
			}
			typedef std::map<std::string, double>::iterator itd_type;
			for(itd_type iterator = doubleprops.begin(); iterator != doubleprops.end(); iterator++) {
				std::cout << "Key  : " << iterator->first << " Value = "<<rep[iterator->first]<<std::endl;
				iterator->second = rep[iterator->first];
			}

			rvidata r;
			r.lt = rep["leftTemperature"];
			r.rt = rep["rightTemperature"];
			r.lhs = rep["leftSeatHeat"];
			r.rhs = rep["rightSeatHeat"];
			r.fs = rep["fanSpeed"];
			r.fd = rep["fanDown"];
			r.fr = rep["fanRight"];
			r.fu = rep["fanUp"];
			r.fac = rep["fanAC"];
			r.fa = rep["fanAuto"];
			r.fr = rep["fanRecirc"];
			r.dm = rep["defrostMax"];
			r.df = rep["defrostFront"];
			r.dr = rep["defrostRear"];

			send_message(&r);



		}
		else
		{
			dlog_print(DLOG_INFO, LOG_TAG,  "GET request was UNsuccessful");
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
	}
}

void getClientRepresentation(std::shared_ptr<OCResource> resource)
{
	if(resource)
	{
		std::cout << "Getting Client Representation..."<<std::endl;
		// Invoke resource's get API with the callback parameter
		QueryParamsMap test;
		resource->get(test, &onGet);
	}
}


void  send_message(rvidata *p)
{
	bundle *b = bundle_create();
	bundle_add_byte(b, "rvimsg", p, sizeof(rvidata));
	int ret = message_port_send_message("org.openinterconnect.geniviocfs2", "s2port", b);
	dlog_print(DLOG_INFO, LOG_TAG, "Message port send status = %d", ret);
}


void message_port_cb(int id, const char *remote_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *data)
{
	appdata_s *ad = (appdata_s *)data;

	size_t size;
	void *ptr;

	bundle_get_byte(message, "rvimsg", &ptr, &size);
	dlog_print(DLOG_INFO, LOG_TAG, "message_port_cb size received = %d", size);
	memcpy(&ad->r, ptr, size);
	dlog_print(DLOG_INFO, LOG_TAG, "message_port_cb defrost=%i", ad->r.dm);
}


static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	/* Let window go to hide state. */
	if(ad->nf_it == elm_naviframe_top_item_get(ad->nf)){
		elm_naviframe_item_pop (ad->nf);
		dlog_print(DLOG_INFO, LOG_TAG,  "Top Item is For Exit\n");
		ui_app_exit();
	}
	else{
		dlog_print(DLOG_INFO, LOG_TAG,  "Top Item is Not NULL\n");
		elm_naviframe_item_pop (ad->nf);
	}
}

void
_change_cb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    int val = elm_slider_value_get(obj);
}

void int_slider(appdata_s *ad, char *text, int min, int max){
	Evas_Object *nf = ad->nf;

	Evas_Object *layout = elm_layout_add(nf);
	evas_object_show(layout);
	Evas_Object *sl = elm_slider_add(layout);
	elm_slider_unit_format_set(sl, "%d");
	elm_slider_indicator_format_set(sl, "%d");
	elm_slider_span_size_set(sl, 120);
	elm_slider_min_max_set(sl, min, max);
	elm_object_text_set(sl, text);
	elm_slider_step_set(sl, 1);
	//elm_slider_indicator_show_on_focus_set(sl, EINA_TRUE);
	evas_object_size_hint_align_set(sl, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(sl, EVAS_HINT_EXPAND, 0.0);
	evas_object_show(sl);
	//evas_object_smart_callback_add(sl, "changed", _change_cb, );
	elm_naviframe_item_push(nf, "Adjust Value", NULL, NULL, layout, "empty");
}

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

	snprintf(buf, 1023, "%s", "Vehicle Location");
	return strdup(buf);
}

static char *
_gl_menu_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = (item_data *)data;
	int index = id->index;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, 1023, popup_menu_names[index], 23.5);
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

void list(void *data)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *genlist;
	Evas_Object *circle_genlist;
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ttc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *ptc = elm_genlist_item_class_new();
	item_data *id;
	int index = 0;

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

	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	id = (item_data *)calloc(sizeof(item_data), 1);
	id->index = index++;
	evas_object_show(genlist);

	elm_genlist_item_append(genlist, ptc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc);
	elm_genlist_item_class_free(ttc);
	elm_genlist_item_class_free(ptc);

	dlog_print(DLOG_INFO, LOG_TAG,  "Vehicle Location\n");

	elm_naviframe_item_push(ad->nf, "Vehicle Location", NULL, NULL, genlist, "empty");
}


void showmap(void *data)
{

	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	Evas_Object *map;
	Elm_Map_Overlay *overlay;

	map = elm_map_add(nf);
	evas_object_size_hint_weight_set(map, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(map);

	elm_map_zoom_set(map, 10);
	elm_map_region_bring_in(map, ad->r.lat, ad->r.lon);

	overlay = elm_map_overlay_add(map, ad->r.lat, ad->r.lon);
	elm_map_overlay_color_set(overlay, 0x00, 0xfa, 0x9a, 0xff);
	elm_map_overlay_displayed_zoom_min_set(overlay, 5);

	elm_naviframe_item_push(nf, "Vehicle Location", NULL, NULL, map, "empty");
}

static void
_item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Eext_Object_Item *item;
	const char *main_text;
	const char *sub_text;
	/* Get current seleted item object */
	item = eext_rotary_selector_selected_item_get(obj);

	/* Get set text for the item */
	main_text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	sub_text = eext_rotary_selector_item_part_text_get(item, "selector,sub_text");

	dlog_print(DLOG_INFO, LOG_TAG,  "Item Selected! Currently Selected %s, %s\n", main_text, sub_text);
}

static void
_item_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	Eext_Object_Item *item;
	const char *main_text;
	const char *sub_text;

	appdata_s *ad = (appdata_s *)data;

	/* Get current seleted item object */
	item = eext_rotary_selector_selected_item_get(obj);

	/* Get set text for the item */
	main_text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	sub_text = eext_rotary_selector_item_part_text_get(item, "selector,sub_text");

	int len = sizeof(icon_name_list) / sizeof(icon_name_list[0]);
	int i = 0;
	for(i = 0; i<len; i++){
		if(!strcmp(icon_name_list[i], sub_text)){
			break;
		}
	}
	dlog_print(DLOG_INFO, LOG_TAG,  "%s, %s - %d\n", icon_name_list[i], sub_text, i);

	if(i==len) return;

	switch(i)
	{
		case 0:
		{
			if(!ocfinitialized){
				initOCFStack();
			}else{
				getClientRepresentation(curResource);
			}
		}
		break;
		case 1: //location
		{
			//to do a GET and in the callback - provide map()
			showmap(ad);
		}
		break;

		case 2: //Left Temp.
		{
			int_slider(ad, icon_name_list[i], 15, 30);

		}
		break;

		case 3: //"Right Temp."
		{

		}
		break;

		case 4: //"Left Seat",
		{

		}
		break;

		case 5: //"Right Seat",
		{

		}
		break;

		case 6: //"Fan Speed",
		{

		}
		break;

		case 7: //"Fan Down",
		{

		}
		break;

		case 8: //"Fan Right",
		{

		}
		break;

		case 9: //"Fan Up",
		{

		}
		break;

		case 10: //"Fan A/C",
		{

		}
		break;

		case 11: //"Fan Auto",
		{

		}
		break;

		case 12: //"Fan Recirculate",
		{

		}
		break;

		case 13: //"DeFrost Max",
		{

		}
		break;

		case 14: //"DeFrost Front",
		{

		}
		break;

		case 15: //"DeFrost Rear",
		{

		}
		break;
	}
	dlog_print(DLOG_INFO, LOG_TAG,  "Item Clicked!, Currently Selected %s, %s\n", main_text, sub_text);
}

void
_item_create(Evas_Object *rotary_selector)
{
	Evas_Object *image;
	Eext_Object_Item * item;
	char buf[255] = {0};
	int i, j, k;

	for (i = 0, k = 0; i < ROTARY_SELECTOR_PAGE_COUNT; i++)
	{
		for (j = 0; j < ROTARY_SELECTOR_PAGE_ITEM_COUNT; j++)
		{
			if(k>=TOTAL_CONTROLS)
				break;

			item = eext_rotary_selector_item_append(rotary_selector);
			image = elm_image_add(rotary_selector);
			elm_image_file_set(image, icon_path_list[k], NULL);
			eext_rotary_selector_item_part_content_set(item,
															 "item,icon",
															 EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL,
															 image);
			snprintf(buf, sizeof(buf), "RVI Control");
			eext_rotary_selector_item_part_text_set(item, "selector,main_text", buf);


			snprintf(buf, sizeof(buf), "%s", icon_name_list[k]);
			eext_rotary_selector_item_part_text_set(item, "selector,sub_text", buf);
			k++;
		}
	}
}

static void
create_base_gui(appdata_s *ad)
{
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->circle_surface = eext_circle_surface_conformant_add(ad->conform);

	/* Base Layout */
	ad->layout = elm_layout_add(ad->conform);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->layout, "layout", "application", "default");
	evas_object_show(ad->layout);

	elm_object_content_set(ad->conform, ad->layout);

	ad->nf = elm_naviframe_add(ad->layout);
	elm_object_part_content_set(ad->layout, "elm.swallow.content", ad->nf);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);


	Evas_Object *rotary_selector = eext_rotary_selector_add(ad->nf);
	eext_rotary_object_event_activated_set(rotary_selector, EINA_TRUE);
	_item_create(rotary_selector);
	evas_object_smart_callback_add(rotary_selector, "item,selected", _item_selected_cb, ad);
	evas_object_smart_callback_add(rotary_selector, "item,clicked", _item_clicked_cb, ad);
	ad->nf_it = elm_naviframe_item_push(ad->nf, _("Rotary Selector"), NULL, NULL, rotary_selector, "empty");

	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	appdata_s *ad = (appdata_s *)data;
	create_base_gui(ad);
	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;


    int port = message_port_register_local_port("s2port", &message_port_cb, &ad);
    dlog_print(DLOG_INFO, LOG_TAG, "Local Message Port = %d", port);

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
