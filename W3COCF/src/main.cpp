#include "livingroom.h"
int initialized = 0;

void  send_message(lr_s *p)
{
	bundle *b = bundle_create();
	bundle_add_byte(b, "ocfmsg", p, sizeof(lr_s));
	int ret = message_port_send_message(PACKAGE, MSGPORT, b);
	dlog_print(DLOG_INFO, LOG_TAG, "Message port send status = %d", ret);
}

static void message_port_cb(int id, const char *remote_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *data)
{
	appdata_s *ad = (appdata_s *)data;
	size_t size;
	void *ptr;
	bundle_get_byte(message, "ocfmsg", &ptr, &size);
	dlog_print(DLOG_INFO, LOG_TAG, "message_port_cb size received = %d", size);
	lr_s *ss = (lr_s *)ptr;
//	//switch(ss->res.restype){
//		case MSP4:{
//			if(!initialized){
//				initialized = 1;
//				//destroy_popup(ad, NULL, NULL);
//			}
//		}
//		break;
//		default:
//			break;
//	}
}

static bool
app_create(void *data)
{
	appdata_s *ad = (appdata_s *)data;
	//FIME: Register for network notification
	elm_app_base_scale_set(1.8);
#ifdef OCF
	initOCFStack(ad);
#endif
	init_global_resources(ad);
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
	appdata_s ad;
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;


    int port = message_port_register_local_port(MSGPORT, &message_port_cb, &ad);
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


#if 0
void launch_shealth()
{
	app_control_h app_control;

	app_control_create(&app_control);
	app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
	app_control_set_app_id(app_control, "com.samsung.shealth");

	int i = app_control_send_launch_request(app_control, NULL, NULL);

	if (i == APP_CONTROL_ERROR_NONE)
	    dlog_print(DLOG_INFO, LOG_TAG, "Succeeded to launch a shealth app.");
	else
	    dlog_print(DLOG_ERROR, LOG_TAG, "Failed to launch a s-health app. %d", i);

	app_control_destroy(app_control);
}

#endif
