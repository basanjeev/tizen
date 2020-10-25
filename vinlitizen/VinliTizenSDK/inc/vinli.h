#ifndef _VINLI_H_
#define _VINLI_H_

#include "Ecore.h"
#include <oauth2.h>
#include <bundle.h>
#include <dlog.h>
#include <app_preference.h>
#include <curl/curl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VINLI_CLIENT_ID 	"client_id"
#define VINLI_ACCESS_TOKEN 	"access_token"
#define VINLI_REDIRECT_URL 	"redirect_url"
#define VINLI_REFRESH_TOKEN "refresh_token"
#define VINLI_EXPIRES 		"expires"

#define VINLI_OAUTH2_REDIRECT_URL "https://dummy_redirect_url"

typedef struct _vinli_request_s *vinli_request_h;

typedef enum {
	VINLI_AUTHENTICATE,
	LIST_ALL_DEVICES,
	GET_DEVICE,
	REGISTER_DEVICE,
	DEREGISTER_DEVICE,
	LIST_ALL_VEHICLES,
	LIST_LATEST_VEHICLE,
	LIST_VEHICLE_INFO,
	LIST_ALL_TRANSACTIONS,
	LIST_ALL_TELEMETRY_MESSAGES,
	GET_TELEMETRY_MESSAGE,
	LIST_LOCATIONS,
	GET_TELEMETRY_SNAPSHOT,
	LIST_ALL_EVENTS,
	GET_EVENT,
	CREATE_SUBSCRIPTION,
	LIST_ALL_SUBSCRIPTIONS,
	GET_SUBSCRIPTION,
	UPDATE_SUBSCRIPTION,
	DELETE_SUBSCRIPTION,
	GET_NOTIFICATION,
	GET_NOTIFICATION_FOR_SUBSCRIPTION,
	GET_NOTIFICATION_FOR_EVENT,
	LIST_ALL_DTC_CODES,
	GET_DTC_CODE,
	LIST_ALL_DEVICE_TRIPS,
	LIST_ALL_VEHICLE_TRIPS,
	GET_TRIP_INFO,
	GET_REPORT_CARD,
	GET_LIFETIME_REPORT_CARD,
	GET_TRIP_REPORT_CARD,
	LIST_ALL_DEVICE_COLLITIONS,
	LIST_ALL_VEHICLE_COLLITIONS,
	GET_COLLITION_INFO,
	VINLI_REQUEST_END
} vinli_request_e;

typedef void (*vinli_app_cb)(vinli_request_e type, void *value, void *user_data);

// API for getting RESTful interface details
EXPORT_API const char *vinli_get_req_name(vinli_request_e);
EXPORT_API int vinli_get_req_type(vinli_request_e);
EXPORT_API int vinli_get_req_type(vinli_request_e );
EXPORT_API const char *vinli_get_req_url(vinli_request_e);
EXPORT_API const char *vinli_get_req_body_format(vinli_request_e);
EXPORT_API int vinli_get_num_requests();

//Individual request APi
EXPORT_API vinli_request_h vinli_create_request();
EXPORT_API void vinli_destroy_request(vinli_request_h);
EXPORT_API void vinli_request_url_set(vinli_request_h handle, char *param);
EXPORT_API void vinli_request_start(vinli_request_h handle);
EXPORT_API void vinli_config_set(const char *key, const char *value);
EXPORT_API void vinli_init(vinli_app_cb, void *);
EXPORT_API void vinli_deinit();

#ifdef __cplusplus
}
#endif

#endif // _VINLI_H_

