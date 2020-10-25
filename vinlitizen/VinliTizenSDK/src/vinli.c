#include "vinli_private.h"

static _vinli_config_s vinliconfig;

const char *prefs[] = {
			VINLI_CLIENT_ID,
			VINLI_ACCESS_TOKEN,
			VINLI_REDIRECT_URL,
			VINLI_REFRESH_TOKEN,
			VINLI_EXPIRES
};

_vinli_api_s vinlidef[] = {
		{0, 0, "VINLI_AUTHENTICATE", "https://auth.vin.li/oauth/authorization/new"},
		{0, 1, "LIST_ALL_DEVICES", "https://platform.vin.li/api/v1/devices"},
		{0, 2, "GET_DEVICE", "https://platform.vin.li/api/v1/devices/%s"},
		{1, 3, "REGISTER_DEVICE", "https://platform.vin.li/api/v1/devices",
				"{\"device\" : {\"id\" : \"%s\"}"
		},
		{2, 4, "DEREGISTER_DEVICE", "https://platform.vin.li/api/v1/devices/%s"},
		{0, 5, "LIST_ALL_VEHICLES", "https://platform.vin.li/api/v1/devices/%s/vehicles"},
		{0, 6, "LIST_LATEST_VEHICLE", "https://platform.vin.li/api/v1/devices/%s/vehicles/_latest"},
		{0, 7, "LIST_VEHICLE_INFO", "https://platform.vin.li/api/v1/vehicles/%s"},
		{0, 8, "LIST_ALL_TRANSACTIONS", "https://platform.vin.li/api/v1/transactions"},
		{0, 9, "LIST_ALL_TELEMETRY_MESSAGES", "https://telemetry.vin.li/api/v1/devices/%s"},
		{0, 10, "GET_TELEMETRY_MESSAGE", "https://telemetry.vin.li/api/v1/messages/%s"},
		{0, 11, "LIST_LOCATIONS", "GET https://telemetry.vin.li/api/v1/devices/%s/locations?fields=rpm,vehicleSpeed", },
		{0, 12, "GET_TELEMETRY_SNAPSHOT", "https://telemetry.vin.li/api/v1/devices/%s/snapshots?fields=rpm,vehicleSpeed,calculatedLoadValue,fuelType"},
		{0, 13, "LIST_ALL_EVENTS", "https://events.vin.li/api/v1/devices/%s/events"},
		{0, 14, "GET_EVENT", "https://events.vin.li/api/v1/events/%s"},
		{1, 15, "CREATE_SUBSCRIPTION", "https://events.vin.li/api/v1/devices/%s/subscriptions",
				"{\"subscription\" : {\"eventType\" : \"%s\", \"url\": \"%s\"}}"
		},
		{0, 16, "LIST_ALL_SUBSCRIPTIONS", "https://events.vin.li/api/v1/devices/%s/subscriptions"},
		{0, 17, "GET_SUBSCRIPTION", "https://events.vin.li/api/v1/subscriptions/%s"},
		{1, 18, "UPDATE_SUBSCRIPTION", "https://events.vin.li/api/v1/devices/%s/subscriptions",
				"{\"subscription\" : {\"url\": \"%s\",\"appData\": \"{\"message\":\"%s\"}}}"
		},
		{2, 19, "DELETE_SUBSCRIPTION", "https://events.vin.li/api/v1/subscriptions/%s"},
		{0, 20, "GET_NOTIFICATION", "https://events.vin.li/api/v1/notifications/%s"},
		{0, 21, "GET_NOTIFICATION_FOR_SUBSCRIPTION", "https://events.vin.li/api/v1/subscriptions/%s/notifications"},
		{0, 22, "GET_NOTIFICATION_FOR_EVENT", "https://events.vin.li/api/v1/events/%s/notifications"},
		{0, 23, "LIST_ALL_DTC_CODES", "https://diagnostic.vin.li/api/v1/vehicles/%s/codes"},
		{0, 24, "GET_DTC_CODE", "https://diagnostic.vin.li/api/v1/codes?number=%s"},
		{0, 25, "LIST_ALL_DEVICE_TRIPS", "https://trips.vin.li/api/v1/devices/%s/trips"},
		{0, 26, "LIST_ALL_VEHICLE_TRIPS", "https://trips.vin.li/api/v1/vehicles/%s/trips"},
		{0, 27, "GET_TRIP_INFO", "https://trips.vin.li/api/v1/trips/%s"},
		{0, 28, "GET_REPORT_CARD", "https://behavior.vin.li/api/v1/devices/%s/report_cards"},
		{0, 29, "GET_LIFETIME_REPORT_CARD", "https://behavior.vin.li/api/v1/devices/%s/report_cards/overall"},
		{0, 30, "GET_TRIP_REPORT_CARD", "https://behavior.vin.li/api/v1/trips/%s/report_card"},
		{0, 31, "LIST_ALL_DEVICE_COLLITIONS", "https://safety.vin.li/api/v1/devices/%s/collisions"},
		{0, 32, "LIST_ALL_VEHICLE_COLLITIONS", "https://safety.vin.li/api/v1/vehicles/%s/collisions"},
		{0, 33, "GET_COLLITION_INFO", "https://safety.vin.li/api/v1/collisions/%s"},
};

EXPORT_API void vinli_init_preferences()
{
	bool value = false;
	char *temp = NULL;
	int i = 0;

	for(i = 0; i<3; i++)
	{
		if(!preference_is_existing(prefs[i], &value))
		{
			dlog_print(DLOG_INFO, LOG_TAG, "preference_is_existing %s. err = %d", prefs[i], value);
			if(!value)
			{
				if(i==0){
					strncpy(vinliconfig.client_id, VINLI_CLIENT_ID, 1023);
					preference_set_string(prefs[i], vinliconfig.client_id);
				}
				if(i==1){
					strncpy(vinliconfig.access_token, VINLI_ACCESS_TOKEN, 1023);
					preference_set_string(prefs[i], vinliconfig.access_token);
				}
				if(i==2){
					strncpy(vinliconfig.redirect_url, VINLI_OAUTH2_REDIRECT_URL, 1023);
					preference_set_string(prefs[i], vinliconfig.redirect_url);
				}
			}
			preference_get_string(prefs[i], &temp);
			dlog_print(DLOG_INFO, LOG_TAG, "preference_is_existing set %s %s", prefs[i], temp);
			free(temp);
		}
	}
}

void
bundle_cb(const char *key, const int type, const bundle_keyval_t *kv,
	void *user_data)
{
	char *display_str = (char *)user_data;

	char *err_val = NULL;
	size_t err_len = 0;
	bundle_keyval_get_basic_val((bundle_keyval_t *)kv, (void **)&err_val,
		&err_len);
	if (err_val) {
		strcat(display_str, key);
		strcat(display_str, "=");
		strcat(display_str, err_val);
		strcat(display_str, ",");
	}
}

void
token_response_cb(oauth2_response_h response, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "token_response_cb");

	char *access_token = NULL;
	oauth2_response_get_access_token(response, &access_token);

	char *refresh_token = NULL;
	oauth2_response_get_refresh_token(response, &refresh_token);

	long long int expires_in = 0;
	oauth2_response_get_expires_in(response, &expires_in);

	char displayStr[1024] = {0,};
	if (access_token) {
		displayStr[0] = '\0';
		strcpy(displayStr, "access token = ");
		strcat(displayStr, access_token);
		vinli_config_set("access_token", access_token);

	} else {
		oauth2_error_h e_handle =  NULL;
		char* error_msg = NULL;

		oauth2_response_get_error(response, &e_handle);
		oauth2_error_get_description(e_handle, &error_msg);

		if (error_msg)
			strcpy(displayStr, error_msg);
		else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code, &platform_error_code);

			if (error_code != 0 || platform_error_code != 0)
				sprintf(displayStr, "Error = [%d][%d]", error_code, platform_error_code);
			else
				strcpy(displayStr, "Unknown server error");
		}
	}

	if (refresh_token) {
		strcat(displayStr, "\r\n");
		strcat(displayStr, "refresh token = ");
		strcat(displayStr, refresh_token);
		vinli_config_set("refresh_token", refresh_token);
	}

	if (expires_in != 0) {
		strcat(displayStr, "\r\n");
		strcat(displayStr, "expires in = ");
		char expires_str[128] = {0};
		sprintf(expires_str, "%lld", expires_in);
		strcat(displayStr, expires_str);
		vinli_config_set("expires", expires_str);
	}

	dlog_print(DLOG_INFO, LOG_TAG, "token_response_cb %s", displayStr);
	if(vinliconfig.appcb){
		vinliconfig.appcb(VINLI_AUTHENTICATE, access_token,vinliconfig.user_data);
	}
	return;
}

void
code_access_token_cb(oauth2_response_h response, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "auth2_access_token_cb");

	char *access_token = NULL;
	char displayStr[1024] = {0,};
	oauth2_response_get_access_token(response, &access_token);
	if (access_token) {
		displayStr[0] = '\0';
		strcpy(displayStr, "access token = ");
		strcat(displayStr, access_token);
	} else {
		oauth2_error_h e_handle = NULL;

		oauth2_response_get_error(response, &e_handle);
		char* error_msg = NULL;

		oauth2_response_get_error(response, &e_handle);
		oauth2_error_get_description(e_handle, &error_msg);

		if (error_msg)
			strcpy(displayStr, error_msg);
		else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code, &platform_error_code);

			if (error_code != 0 || platform_error_code != 0)
				sprintf(displayStr, "Error = [%d][%d]", error_code, platform_error_code);
			else
				strcpy(displayStr, "Unknown server error");
		}
	}
	dlog_print(DLOG_INFO, LOG_TAG, "code_access_token_cb %s", displayStr);
}

void
grant_response_cb(oauth2_response_h response, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "grant_response_cb");

	char *auth_code = NULL;
	oauth2_response_get_authorization_code(response, &auth_code);
	if (auth_code) {
		oauth2_manager_h mgr = NULL;
		int ret = oauth2_manager_create(&mgr);

		oauth2_request_h request = (oauth2_request_h) user_data;

		ret = oauth2_request_set_authorization_code(request, auth_code);

		if (mgr && request) {
			ret = oauth2_manager_request_access_token(mgr,
				request, code_access_token_cb, NULL);
		}
	} else {
		char displayStr[1024] = {0,};

		oauth2_error_h e_handle =  NULL;
		char* error_msg = NULL;

		oauth2_response_get_error(response, &e_handle);
		oauth2_error_get_description(e_handle, &error_msg);

		if (error_msg)
			strcpy(displayStr, error_msg);
		else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code, &platform_error_code);

			if (error_code != 0 || platform_error_code != 0)
				sprintf(displayStr, "Error = [%d][%d]", error_code, platform_error_code);
			else
				strcpy(displayStr, "Unknown server error");
		}

		dlog_print(DLOG_INFO, LOG_TAG, "grant_response_cb %s", displayStr);
	}
}


void download_thread_end_cb(void *data, Ecore_Thread *thread)
{
    dlog_print(DLOG_INFO, LOG_TAG, "Thread cleanup()");
}

void download_thread(void *data, Ecore_Thread *thread)
{
	_vinli_request_s *vr = (_vinli_request_s *)(data);
	CURLcode error_code = curl_easy_perform(vr->curl);
	dlog_print(DLOG_INFO, LOG_TAG, "Waiting for request to finish %s", curl_easy_strerror(error_code));
    if(error_code == CURLE_OK)
    {
      dlog_print(DLOG_INFO, LOG_TAG, "Cleanup");
      eina_strbuf_free(vr->strbuf);
      curl_easy_cleanup(vr->curl);
    }
	if(vinliconfig.appcb){
		vinliconfig.appcb(vr->api->type , NULL, vinliconfig.user_data);
	}
}

int progress_cb(void *data, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if(dltotal!=0 && dltotal == dlnow)
	{
		//elm_object_text_set(getResult(), eina_strbuf_string_get(data));
	}
	return 0;
}

static size_t write_cb(void *buffer, size_t size, size_t nitems, void *data)
{
    if (!data) {
        dlog_print(DLOG_ERROR, LOG_TAG, "data is NULL");
        return 0;
    }
    size_t nwritten = size * nitems;
    dlog_print(DLOG_INFO, LOG_TAG, "Write Invoked: %d ", nwritten);

    eina_strbuf_append_length(data, buffer, nwritten);
    dlog_print(DLOG_INFO, LOG_TAG, "nwritten: %d ", nwritten);
    return nwritten;
}




EXPORT_API const char *vinli_get_req_name(vinli_request_e reqtype){
	if(reqtype>=VINLI_AUTHENTICATE && reqtype < VINLI_REQUEST_END)
	{
		return vinlidef[reqtype].req_name;
	}
	return "Invalid Request";
}

EXPORT_API int vinli_get_req_type(vinli_request_e reqtype){
	if(reqtype>=VINLI_AUTHENTICATE && reqtype < VINLI_REQUEST_END)
	{
		return vinlidef[reqtype].type;
	}
	return 0;
}
EXPORT_API const char *vinli_get_url(vinli_request_e reqtype){
	if(reqtype>=VINLI_AUTHENTICATE && reqtype < VINLI_REQUEST_END)
	{
		return vinlidef[reqtype].url;
	}
	return "Invalid Request";
}

EXPORT_API const char *vinli_get_req_body_format(vinli_request_e reqtype){
	if(reqtype>=VINLI_AUTHENTICATE && reqtype < VINLI_REQUEST_END)
	{
		return vinlidef[reqtype].format;
	}
	return "Invalid Request";
}

EXPORT_API int vinli_get_num_requests()
{
	return sizeof(vinlidef)/sizeof(vinlidef[0]);
}

EXPORT_API void vinli_config_set(const char *key, const char *value)
{
	if(!strcmp(key, prefs[0]))
	{
		strncpy(vinliconfig.client_id, value, 1023);
	}
	if(!strcmp(key, prefs[1]))
	{
		strncpy(vinliconfig.access_token, value, 1023);
	}
	if(!strcmp(key, prefs[2]))
	{
		strncpy(vinliconfig.redirect_url, value, 1023);
	}
	preference_set_string(key, value);
	dlog_print(DLOG_INFO, LOG_TAG, "preference_set %s %s", key, value);

}

EXPORT_API void vinli_init(vinli_app_cb cb, void *user_data)
{
	_vinli_api_s *va = &vinlidef[VINLI_AUTHENTICATE];
	vinli_init_preferences();
	vinliconfig.appcb = cb;
	vinliconfig.user_data = user_data;

	if(strcmp(vinliconfig.access_token, VINLI_ACCESS_TOKEN))
	{
		dlog_print(DLOG_INFO, LOG_TAG, "Access Token Found %s", vinliconfig.access_token);
		return;
	}
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);
	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);
	ret = oauth2_request_set_auth_end_point_url(request, va->url);

	ret = oauth2_request_set_redirection_url(request, vinliconfig.redirect_url);
	ret = oauth2_request_set_client_id(request, vinliconfig.client_id);
	ret = oauth2_request_set_response_type(request, OAUTH2_RESPONSE_TYPE_TOKEN);

	char *ptr;
	oauth2_request_get_auth_end_point_url(request, &ptr);
	dlog_print(DLOG_INFO, LOG_TAG, ptr);

	if (mgr && request) {
		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, NULL);
	}
	dlog_print(DLOG_INFO, LOG_TAG, "Access Token Found %s", vinliconfig.access_token);
}

EXPORT_API void vinli_deinit()
{

}

EXPORT_API void vinli_request_eventtype_set(vinli_request_h handle, char *param)
{
	if(param && handle)
	{
		_vinli_request_s *vr = (_vinli_request_s *)handle;
		strncpy(vr->eventtype, param, 9);
	}
}

EXPORT_API void vinli_request_url_set(vinli_request_h handle, char *param)
{
	if(param && handle)
	{
		_vinli_request_s *vr = (_vinli_request_s *)handle;
		strncpy(vr->url, param, 1023);
	}
}

EXPORT_API void vinli_request_message_set(vinli_request_h handle, char *param)
{
	if(param && handle)
	{
		_vinli_request_s *vr = (_vinli_request_s *)handle;
		strncpy(vr->message, param, 1023);
	}
}

////TODO : Manage when multiple params are needed in
//EXPORT_API void vinli_request_url_set(vinli_request_h handle, char *param)
//{
//	_vinli_request_s *vr = (_vinli_request_s *)handle;
//
//	if(param && handle)
//	{
//		if(strstr(vr->api->url, "%s"))
//		{
//			//snprintf(buffer, 1023, vr->api->url, "80247822-aa96-4f6e-b362-88b4341f4302");
//			snprintf(buffer, 1023, vr->api->url, param);
//		}else{
//			strncpy(buffer, vr->api->url, 1023);
//		}
//		curl_easy_setopt(vr->curl, CURLOPT_URL, buffer);
//	}
//}

EXPORT_API vinli_request_h vinli_create_request(vinli_request_e reqtype)
{
	_vinli_request_s *vr = (_vinli_request_s *)calloc(1, sizeof (_vinli_request_s));
	char buffer[1024];
	vr->strbuf = eina_strbuf_new();
	vr->curl = curl_easy_init();
	vr->api = &vinlidef[reqtype];
    char *header = "Authorization: Bearer %s";
    char *header1 = "Content-Type: application/json";
    char *header2 = "Accept: application/json";

    snprintf(buffer, 1023, header, vinliconfig.access_token);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, buffer);
	dlog_print(DLOG_INFO, LOG_TAG, "Auth Header  = %s", buffer);


	CURLcode error_code = curl_easy_setopt(vr->curl, CURLOPT_WRITEDATA, vr->strbuf);
	error_code = curl_easy_setopt(vr->curl, CURLOPT_WRITEFUNCTION, write_cb);
    error_code = curl_easy_setopt(vr->curl, CURLOPT_NOPROGRESS, 0L);
	error_code = curl_easy_setopt(vr->curl, CURLOPT_PROGRESSDATA, vr->strbuf);
	error_code = curl_easy_setopt(vr->curl, CURLOPT_PROGRESSFUNCTION, progress_cb);

	dlog_print(DLOG_INFO, LOG_TAG, "Request Type = %d", vr->api->type);
	switch(vr->api->type)
	{
		case 0: //GET
			dlog_print(DLOG_INFO, LOG_TAG, "Doing a GET request");
			curl_easy_setopt(vr->curl, CURLOPT_HTTPGET, 1);
			break;
		case 1: // POST
			dlog_print(DLOG_INFO, LOG_TAG, "Doing a POST request");
			curl_easy_setopt(vr->curl, CURLOPT_HTTPPOST, 1);

			if(vr->api->type==REGISTER_DEVICE || vr->api->type==CREATE_SUBSCRIPTION || vr->api->type==UPDATE_SUBSCRIPTION)
			{
				headers = curl_slist_append(headers, header1);
			    headers = curl_slist_append(headers, header2);
				curl_easy_setopt(vr->curl, CURLOPT_POSTFIELDS, buffer);

				if(vr->api->type==REGISTER_DEVICE)
					snprintf(buffer, 1023, vr->body, vinliconfig.client_id);
				if(vr->api->type==CREATE_SUBSCRIPTION)
					snprintf(buffer, 1023, vr->body, vr->eventtype, vr->url);
				if(vr->api->type==UPDATE_SUBSCRIPTION)
					snprintf(buffer, 1023, vr->body, vr->url, vr->message);
			}
			break;
		case 2: // DELETE
			{
				dlog_print(DLOG_INFO, LOG_TAG, "Doing a DELETE request");
				curl_easy_setopt(vr->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
			    headers = curl_slist_append(headers, header2);
			}
			break;
	}
    curl_easy_setopt(vr->curl, CURLOPT_HTTPHEADER, headers);
    char *access_token = NULL;
	dlog_print(DLOG_INFO, LOG_TAG, "Processing Request %s", vr->api->req_name);
	if(!strcmp(access_token, "NOT_SET"))
	{
		dlog_print(DLOG_INFO, LOG_TAG, "Authenticate with Vinli First");
		return NULL;
	}
	dlog_print(DLOG_INFO, LOG_TAG, "Request URL = %s error code = %d", buffer);
    return (vinli_request_h)vr;
}


EXPORT_API void vinli_request_start(vinli_request_h handle)
{
	ecore_thread_feedback_run(download_thread, NULL,
                              download_thread_end_cb, NULL,
                              handle, EINA_FALSE);
}

EXPORT_API void vinli_destroy_request(vinli_request_h handle)
{
	// cleanup
}
