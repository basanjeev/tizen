#include "rest.h"
#include "common.h"
#include "cJSON.h"
#include <app_preference.h>
#include <dlog.h>

void set_signal_value(int i, int value);


void save_setting(char *addr)
{
	char ip[16];
    char address[128];
    strncpy(address, addr, 128);
	dlog_print(DLOG_INFO, LOG_TAG, "Check %s", address);
	preference_set_string("SERVER_ADDRESS", address);
	char *port = strchr(address, ':');
	*port = '\0';
	strcpy(ip, address);
	port++;
	dlog_print(DLOG_INFO, LOG_TAG, "IP : %s PORT %s", ip, port);
	preference_set_string("IP_ADDRESS", ip);
	preference_set_string("RESTPORT", port);
	dlog_print(DLOG_INFO, LOG_TAG, "Check %s : %s", ip, port);
}

void w3c_request(reqtype_e reqtype, const char *path, const char *body, int index)
{
    dlog_print(DLOG_DEBUG, LOG_TAG, "Path %s", path);
    vss_request_s *req = (vss_request_s *)calloc(1, sizeof (vss_request_s));
    if(req)
    {
        req->url = eina_strbuf_new();
        eina_strbuf_append(req->url, "http://");
        req->reqtype = reqtype;

        char *serverip = NULL;
        preference_get_string("IP_ADDRESS", &serverip);
        // dlog_print(DLOG_DEBUG, LOG_TAG, "IP_ADDRESS %s", serverip);

        if(!serverip){
            return;
        }

        eina_strbuf_append(req->url, serverip);
        eina_strbuf_append(req->url, ":");

        char *serverport = NULL;
        preference_get_string("RESTPORT", &serverport);
        // dlog_print(DLOG_DEBUG, LOG_TAG, "RESTPORT %s", serverport);

        if(reqtype==AUTHENTICATE)
            eina_strbuf_append(req->url, "8500");
        else if(reqtype==AUTHORIZE)
            eina_strbuf_append(req->url, "8600");
        else
            eina_strbuf_append(req->url, serverport);

        eina_strbuf_append(req->url, "/");
        eina_strbuf_append(req->url, path);
        free(serverip);
        free(serverport);
        dlog_print(DLOG_DEBUG, LOG_TAG, "%s", eina_strbuf_string_get(req->url));
        const char *url = eina_strbuf_string_get(req->url);
        Eina_Bool result = EINA_FALSE;
        if(reqtype==AUTHENTICATE || reqtype==AUTHORIZE || reqtype==POST)
        {
            req->url_con = ecore_con_url_custom_new(url, "POST");
            ecore_con_url_timeout_set (req->url_con, 5.0);
            ecore_con_url_data_set(req->url_con, req);
            dlog_print(DLOG_DEBUG, LOG_TAG, "Body %s", body);
        	result = ecore_con_url_post(req->url_con, body, strlen(body), "application/json");
        }
        else
        {
            req->url_con = ecore_con_url_custom_new(url, "GET");
            ecore_con_url_timeout_set (req->url_con, 5.0);
            char *token = NULL;
            preference_get_string("ACCESS_TOKEN", &token);
            char header[1024];
            snprintf(header, 1024, "Bearer %s", token);
            free(token);
            ecore_con_url_additional_header_add(req->url_con, "Authorization", header);
            req->index = index;
            ecore_con_url_data_set(req->url_con, req);
        	result = ecore_con_url_get(req->url_con);
        }
        dlog_print(DLOG_DEBUG, LOG_TAG, "REQUEST : %s : Result %d", eina_strbuf_string_get(req->url), result);
    }
}

Eina_Bool get_access_token(void *data){
    char *grant_token = NULL;
    char body[1024];
    dlog_print(DLOG_DEBUG, LOG_TAG, "Getting Access Token");
    preference_get_string("ACCESS_GRANT_TOKEN", &grant_token);
    snprintf(body, 1024, "{\"scope\" : \"VehicleReadWrite\", \"token\" : \"%s\"}", grant_token);
    w3c_request(AUTHORIZE, "atserver", body, 0);
    free(grant_token);
    return EINA_FALSE;
}

Eina_Bool
_url_data_cb(void *data, int type, void *event_info)
{
    Ecore_Con_Event_Url_Data *url_data = event_info;
    vss_request_s *req = (vss_request_s *)ecore_con_url_data_get(url_data->url_con);

    if(NULL == req->response){
        req->response = eina_strbuf_new();
    }
    dlog_print(DLOG_DEBUG, LOG_TAG, "Adding %d bytes", url_data->size);
    eina_strbuf_append_length(req->response, (char *)url_data->data, url_data->size);
    return EINA_TRUE;
}

Eina_Bool
_url_complete_cb(void *data, int type, void *event_info)
{
    Ecore_Con_Event_Url_Complete *url_complete = event_info;
    vss_request_s *req = (vss_request_s *)ecore_con_url_data_get(url_complete->url_con);
    const char *response = eina_strbuf_string_get(req->response);
    dlog_print(DLOG_DEBUG, LOG_TAG, "%s", response);
    cJSON *json = cJSON_Parse(response);
    if(NULL!=json)
    {
    	switch(req->reqtype){
			case AUTHENTICATE:
				req->value = cJSON_GetObjectItem(json, "token");
				if(req->value){
					preference_set_string("ACCESS_GRANT_TOKEN", req->value->valuestring);
                    ecore_timer_add(0.5, get_access_token, NULL);
				}else{
					preference_set_string("ACCESS_GRANT_TOKEN", "");
				}
				dlog_print(DLOG_DEBUG, LOG_TAG, "Grant Token \n%s", req->value->valuestring);
				break;
			case AUTHORIZE:
				req->value = cJSON_GetObjectItem(json, "token");
				if(req->value){
					preference_set_string("ACCESS_TOKEN", req->value->valuestring);
				}else{
					preference_set_string("ACCESS_TOKEN", "");
				}
				dlog_print(DLOG_DEBUG, LOG_TAG, "Access Token \n%s", req->value->valuestring);
				break;
			case GET:
                req->value = cJSON_GetObjectItem(json, "value");
				if(req->value){
                    dlog_print(DLOG_DEBUG, LOG_TAG, "Received Value %s", req->value->valuestring);
                    set_signal_value(req->index, atoi(req->value->valuestring));
				}
                break;
			case POST:
				break;
    	}
    }
    if(json)cJSON_Delete(json);
    return EINA_TRUE;
}
