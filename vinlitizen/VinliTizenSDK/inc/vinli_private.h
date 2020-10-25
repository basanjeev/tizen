#ifndef VINLI_PRIVATE_H_
#define VINLI_PRIVATE_H_

#include "vinli.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int  type; // 0 = GET / 1 = POST / 2 = DELETE
	vinli_request_e  req;
	char *req_name;
	char *url;
	char *format;
} _vinli_api_s;

typedef struct
{
	_vinli_api_s *api;
	char *body;
	CURL *curl;
	Eina_Strbuf *strbuf;

	char eventtype[10];
	char url[1024];
	char message[1024];
}_vinli_request_s;

typedef struct
{
	char access_token[1024];
	char client_id[1024];
	char redirect_url[1024];
	char refresh_token[1024];
	char expires[128];
	vinli_app_cb appcb;
	void *user_data;
}_vinli_config_s;

#ifdef __cplusplus
}
#endif
#endif /* VINLI_PRIVATE_H_ */
