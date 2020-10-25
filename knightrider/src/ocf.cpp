#include "main.h"
#include <iotcon.h>


static int skipnoti = 1;
static char devnames[][11] = {
		"AUTOMOTIVE",
};

static char gdevrestypes[][100] = {
		"oic.r.automotive"
};

static lr_resources_s globres[] = {
	{USERPROFILE,	"Vehicle User Profile", 	"/a/vehicle/userprofile", 		ICON_DIR"/user.png", 	GENIVI_VEHICLE, 	true},
};

void init_global_resources(appdata_s *ad){
}



lr_resources_s *get_res_by_uri(const char *ruri){
	for(int i = USERPROFILE; i<MAX_RESOURCES; i++){
		if(!strcmp(globres[i].ruri, ruri))
			return &globres[i];
	}
	return NULL;
}

char *get_device_name(devtype_e devtype){
	if(devtype>=GENIVI_VEHICLE && devtype<MAX_DEVICES)
		return devnames[devtype];
	else
		return "OCF Device";
}

lr_resources_s *get_res_by_devtype(devtype_e devtype){
	for(int i = USERPROFILE; i<MAX_RESOURCES; i++){
		if(devtype == globres[i].devtype)
			return &globres[i];
	}
	return NULL;
}

lr_resources_s *get_res_by_name(const char *name){
	for(int i = USERPROFILE; i<MAX_RESOURCES; i++){
		if(!strcmp(globres[i].name, name))
			return &globres[i];
	}
	return NULL;
}

lr_resources_s *get_res_by_restype(ocfrestype_e restype){
	if(restype>=USERPROFILE && restype <MAX_RESOURCES){
		return &globres[restype];
	}
	return NULL;
}

#if 0

lr_resources_s *get_livingroom_resource_value(ocfrestype_e res){
	if(res>=USERPROFILE && res <MAX_RESOURCES){
		return &globres[res];
	}
	return NULL;
}

static FILE* client_open(const char* /*path*/, const char *mode)
{
//	/return fopen("./oic_svr_db_client.json", mode);
	return NULL;
}

devtype_e get_devtype_from_restype(const char *rt){
	for(int i = GENIVI_VEHICLE; i<MAX_DEVICES; i++){
		if(!strcmp(rt, gdevrestypes[i])){
			dlog_print(DLOG_INFO, LOG_TAG, "Matched out Resource Type %s", rt);
			return (devtype_e)i;
		}
	}
	return MAX_DEVICES;
}

void foundResource(std::shared_ptr<OCResource> resource)
{
#ifdef OCF
	std::string resourceURI;
	std::string sid;
	try
	{
		resourceURI = resource->uri();
		sid = resource->sid();

		//if (lrmap.find(resourceURI + sid) == lrmap.end())
		{
			for(int i = 0; i<MAX_RESOURCES; i++){
				if ((sid.compare(globres[i].sid) == 0) && (resourceURI.compare(globres[i].ruri) == 0) && !globres[i].found){
					dlog_print(DLOG_INFO, LOG_TAG,  "Resource Found %s Assigned %s", resourceURI.c_str(), resource->sid().c_str());
					lrmap[resourceURI + sid] = resource;
					globres[i].found = true;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		dlog_print(DLOG_INFO, LOG_TAG,  "Exception in foundResource");
	}
#endif
}

void foundDevice(std::shared_ptr<OCResource> resource)
{
#ifdef OCF
	std::string sid;
	std::string hostAddress;
	try
	{
		sid = resource->sid();
		devtype_e resourceFor = MAX_DEVICES;
        for(auto &resourceTypes : resource->getResourceTypes())
        {
    		 resourceFor = get_devtype_from_restype(resourceTypes.c_str());
        }

        if(resourceFor==MAX_DEVICES){
        	dlog_print(DLOG_INFO, LOG_TAG,  "No matching devices found");
			return;
        }

		dlog_print(DLOG_INFO, LOG_TAG,  "Device Type = %d %s",resourceFor , resource->uri().c_str());
		for(int i = 0; i<MAX_RESOURCES; i++){
			if (!globres[i].found && (resourceFor == globres[i].devtype)){
				strncpy(globres[i].sid, sid.c_str(), 499);
				dlog_print(DLOG_INFO, LOG_TAG,  "ServerID Found %s Assigned SID %s", sid.c_str(), globres[i].sid);
			}
		}
		std::ostringstream requestURI;
		std::cout.setf(std::ios::boolalpha);
		requestURI << OC_RSRVD_WELL_KNOWN_URI;
		OCPlatform::findResource(resource->host(), requestURI.str(), CT_DEFAULT, &foundResource);
		dlog_print(DLOG_INFO, LOG_TAG,  "Finding Resource...%s ", requestURI.str().c_str());
	}
	catch(std::exception& e)
	{
		dlog_print(DLOG_INFO, LOG_TAG,  "Exception in foundResource");
	}
#endif
}



void initOCFStack(appdata_s *ad)
{
#ifdef OCF
	OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };
	PlatformConfig cfg {
		OC::ServiceType::InProc,
			OC::ModeType::Client,
			"0.0.0.0",
			0,
			OC::QualityOfService::HighQos,
			&ps
	};
	OCPlatform::Configure(cfg);
#endif
}

void onPost(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
	dlog_print(DLOG_INFO, LOG_TAG,  "POST request response code = %d ", eCode);
}

void setRepresentation(OCRepresentation &rep, geniviocf_s *s){
	dlog_print(DLOG_INFO, LOG_TAG,  "Setting Representation for %s", s->res->name);
	switch(s->res->restype){
		case USERPROFILE:{
	    	rep.setValue("scene", s->scene);
	    	rep.setValue("user",  s->user);
	    	dlog_print(DLOG_INFO, LOG_TAG,  "Setting user %d and scene = %d", s->user, s->scene);
	    }
	    break;
	}
	dlog_print(DLOG_INFO, LOG_TAG,  "Representation set");
}

void postOCF(geniviocf_s *s)
{
	string key;
	OCRepresentation rep;

	string temp = s->res->ruri;
	temp += s->res->sid;
	std::shared_ptr<OCResource> resource = lrmap[temp];
	if(resource){
		setRepresentation(rep, s);
		dlog_print(DLOG_INFO, LOG_TAG,  "POSTING %s", globres[s->res->restype].ruri);
#ifdef OCF
		resource->post(rep, QueryParamsMap(), &onPost);
#endif
	}else{
		dlog_print(DLOG_INFO, LOG_TAG,  "Resource Unavailable %s", globres[s->res->restype].ruri);
	}
}

void putOCF(geniviocf_s *s)
{
}


void findDevices(appdata_s *ad){
#ifdef OCF

	for(int i = USERPROFILE; i<MAX_RESOURCES; i++){
		globres[i].found = false;
		try
		{
			std::ostringstream requestURI;
			std::cout.setf(std::ios::boolalpha);
			requestURI << OC_RSRVD_WELL_KNOWN_URI<<"?rt="<<gdevrestypes[i];
			OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundDevice);
			dlog_print(DLOG_INFO, LOG_TAG,  "Finding Device... %s", devnames[i]);
		}catch(OCException& e)
		{
			oclog() << "Exception in main: "<<e.what();
		}
	}

#endif
}

void findDeviceResource(geniviocf_s *s)
{
	int start = 0;
	int end = 0;
	start = USERPROFILE;
	end =USERPROFILE;
	for(int i = USERPROFILE; i<MAX_RESOURCES; i++){
		if(i>=start && i<=end){
			globres[i].found = false;
		}
		else
			globres[i].found = true;
	}
#ifdef OCF
	try
	{
		std::ostringstream requestURI;
		std::cout.setf(std::ios::boolalpha);
		requestURI << OC_RSRVD_WELL_KNOWN_URI<<"?rt="<<gdevrestypes[s->res->devtype];
		OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundDevice);
		dlog_print(DLOG_INFO, LOG_TAG,  "Finding Resource...%s ", requestURI.str().c_str());
	}catch(OCException& e)
	{
		oclog() << "Exception in main: "<<e.what();
	}
#endif
}

#endif


int initOCFStack(appdata_s *ad)
{
	return iotcon_initialize(app_get_resource_path());
}

static void
_parse_representation(iotcon_representation_h repr)
{
    int ret;
    int brightness;
    iotcon_attributes_h attr = NULL;
    ret = iotcon_representation_get_attributes(repr, &attr);
    //ret = iotcon_attributes_get_int(attr, "brightness", &brightness);
    //dlog_print(DLOG_DEBUG, LOG_TAG, "Brightness: %d", brightness);
}

static void
_on_get(iotcon_remote_resource_h resource, iotcon_error_e err,
        iotcon_request_type_e request_type, iotcon_response_h response, void *user_data)
{
    int ret;
    iotcon_representation_h repr;
    iotcon_response_result_e result;
    ret = iotcon_response_get_result(response, &result);
    if (IOTCON_RESPONSE_OK != result)
    ret = iotcon_response_get_representation(response, &repr);
    _parse_representation(repr);
}

static void
_on_put(iotcon_remote_resource_h resource, iotcon_error_e err,
        iotcon_request_type_e request_type, iotcon_response_h response, void *user_data)
{
    int ret;
    iotcon_representation_h repr;
    iotcon_response_result_e result;
    ret = iotcon_response_get_result(response, &result);
    ret = iotcon_response_get_representation(response, &repr);
    _parse_representation(repr);
}

static void
_observe_cb(iotcon_remote_resource_h resource, iotcon_error_e err,
            int sequence_number, iotcon_response_h response, void *user_data)
{
    int ret;
    iotcon_representation_h repr;
    dlog_print(DLOG_DEBUG, LOG_TAG, "sequence: %d", sequence_number);
    ret = iotcon_response_get_representation(response, &repr);
     _parse_representation(repr);
}

static bool
_found_cb(iotcon_remote_resource_h resource, iotcon_error_e err, void *user_data)
{
    int ret;
    char *address;
    char *uri_path;
    ret = iotcon_remote_resource_get_host_address(resource, &address);
    dlog_print(DLOG_DEBUG, LOG_TAG, "host_address: %s", address);
    ret = iotcon_remote_resource_get_uri_path(resource, &uri_path);
    dlog_print(DLOG_DEBUG, LOG_TAG, "uri_path: %s", uri_path);

    iotcon_remote_resource_h _car_resource = NULL;
    ret = iotcon_remote_resource_clone(resource, &_car_resource);
    ret = iotcon_remote_resource_get(_car_resource, NULL, _on_get, NULL);

//    ret = iotcon_remote_resource_observe_register(_car_resource, IOTCON_OBSERVE_IGNORE_OUT_OF_ORDER, NULL, _observe_cb, NULL);
//    ret = iotcon_remote_resource_observe_deregister(_car_resource);
//    iotcon_attributes_destroy(attr);
//    iotcon_representation_destroy(repr);
}


void postOCF(geniviocf_s *s)
{

    int ret;
    iotcon_representation_h repr;
    iotcon_attributes_h attr;
    iotcon_remote_resource_h _car_resource = NULL;
    ret = iotcon_representation_create(&repr);
    ret = iotcon_attributes_create(&attr);
    ret = iotcon_attributes_add_int(attr, "scene", s->scene);
    ret = iotcon_attributes_add_int(attr, "user", s->user);
	dlog_print(DLOG_INFO, LOG_TAG,  "Setting user %d and scene = %d", s->user, s->scene);
    ret = iotcon_representation_set_attributes(repr, attr);
    ret = iotcon_remote_resource_put(_car_resource, repr, NULL, _on_put, NULL);
}



int findDevices(appdata_s *ad){
	int ret;

	for(int i = USERPROFILE; i<MAX_RESOURCES; i++){
		globres[i].found = false;
		iotcon_query_h query;
		const char *res_type = gdevrestypes[i];
		ret = iotcon_query_create(&query);
		ret = iotcon_query_set_resource_type(query, res_type);
		ret = iotcon_find_resource(IOTCON_MULTICAST_ADDRESS, IOTCON_CONNECTIVITY_IP | IOTCON_CONNECTIVITY_PREFER_UDP, query, _found_cb, NULL);
	}
	return ret;
}
