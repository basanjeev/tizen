#include "main.h"
#include "common.h"
#include "view.h"
#include "rest.h"
#include <dlog.h>

static void app_terminate(void *user_data)
{
	view_destroy(user_data);
}

static bool app_create(void *user_data)
{
	appdata_s *ad = (appdata_s *)user_data;
	create_base_gui(ad);
	return true;
}

int main(int argc, char *argv[])
{
	int ret;

	appdata_s ad;
	ad.mouse_down_dial_num = -1;
	ui_app_lifecycle_callback_s event_callback = {0, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;

	ecore_init();
	ecore_con_init();
	ecore_con_url_init();
	if (!ecore_con_url_pipeline_get())
    	ecore_con_url_pipeline_set(EINA_TRUE);
	ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA, _url_data_cb, &ad);
	ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, _url_complete_cb, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
	}
	ecore_con_url_shutdown();
	ecore_con_shutdown();
	ecore_shutdown();
	return ret;
}


