#ifndef _REST_H_
#define _REST_H_

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <cJSON.h>

typedef enum {
    AUTHENTICATE,
    AUTHORIZE,
    GET,
    POST
}reqtype_e;

typedef void (*request_cb)(void *data);

typedef struct _vss_request_s{
    reqtype_e reqtype;
    Eina_Strbuf *url;
    Eina_Strbuf *body;
    Eina_Strbuf *response;
    Ecore_Con_Url *url_con;
    cJSON *value;
    int index;
    request_cb cb;
}vss_request_s;

void w3c_request(reqtype_e reqtype, const char *path, const char *body, int index);
Eina_Bool _url_data_cb(void *data, int type, void *event_info);
Eina_Bool _url_complete_cb(void *data, int type, void *event_info);

#endif
