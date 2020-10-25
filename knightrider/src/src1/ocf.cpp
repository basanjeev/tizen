#include "geniviocf.h"
using namespace std;

#include <map>
#include <string>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
namespace PH = std::placeholders;
typedef std::map<std::string, std::shared_ptr<OCResource>> DiscoveredResourceMap;
DiscoveredResourceMap lrmap;

static int deviceFound = 0;

static char devnames[][10]{
		"GENIVI",
		"FAMILYHUB"
};

static char gdevrestypes[][100]{
		"oic.d.ivi",
		"oic.d.refrigerator"
};
ocfrestype_e restype;
devtype_e devtype;
char sid[500];
char name[100];
char ruri[100];
char icon[1024];
bool found;
string host;
static lr_resources_s globres[] = {
	{VEHICLECONTROL,	GENIVI_VEHICLE, "", "Vehicle User Profile", 	"/a/vehicle/control", 		ICON_DIR"/user.png", 	true},
	{FHNOTI, 			FH, 			"", "FamilyHub Notifications",	"/doors/vs/0", 				ICON_DIR"/fh/dcv.png", 	true}
};

lr_resources_s *get_res_by_uri(const char *ruri){
	for(int i = VEHICLECONTROL; i<MAX_RESOURCES; i++){
		if(!strcmp(globres[i].ruri, ruri))
			return &globres[i];
	}
	return NULL;
}

char *get_device_name(devtype_e devtype){
	if(devtype>=GENIVI_VEHICLE && devtype<MAX_DEVICES)
		return devnames[devtype];
	else
		return (char *)"OCF Device";
}

lr_resources_s *get_res_by_devtype(devtype_e devtype){
	for(int i = VEHICLECONTROL; i<MAX_RESOURCES; i++){
		if(devtype == globres[i].devtype)
			return &globres[i];
	}
	return NULL;
}

lr_resources_s *get_res_by_name(const char *name){
	for(int i = VEHICLECONTROL; i<MAX_RESOURCES; i++){
		if(!strcmp(globres[i].name, name))
			return &globres[i];
	}
	return NULL;
}

lr_resources_s *get_res_by_restype(ocfrestype_e restype){
	if(restype>=VEHICLECONTROL && restype <MAX_RESOURCES){
		return &globres[restype];
	}
	return NULL;
}

lr_resources_s *get_livingroom_resource_value(ocfrestype_e res){
	if(res>=VEHICLECONTROL && res <MAX_RESOURCES){
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

		for(int i = 0; i<MAX_RESOURCES; i++){
			if ((sid.compare(globres[i].sid) == 0) && (resourceURI.compare(globres[i].ruri) == 0) && !globres[i].found){
				dlog_print(DLOG_INFO, LOG_TAG,  "*****Found %s Assigned %s", resourceURI.c_str(), resource->sid().c_str());
				lrmap[resourceURI + sid] = resource;
				string temp = resourceURI + sid;
				dlog_print(DLOG_INFO, LOG_TAG,  "*********************************Assigned for   %s", temp.c_str());
				globres[i].found = true;
				deviceFound--;
				if(deviceFound==0){
					lr_resources_s temp;
					temp.restype = MAX_RESOURCES;
					strcpy(temp.sid, "Found Resources!");
					send_message(&temp);
				}

				if(globres[i].devtype==FH){
					observeOCF(&globres[i]);
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

		for(int i = 0; i<MAX_RESOURCES; i++){
			if (resourceFor == globres[i].devtype){
				strncpy(globres[i].sid, sid.c_str(), 499);
				globres[i].host = resource->host();
				dlog_print(DLOG_INFO, LOG_TAG,  "Found %s URI %s SID %s", devnames[resourceFor], resource->uri().c_str() , sid.c_str());
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
		dlog_print(DLOG_INFO, LOG_TAG,  "Exception in findDevice");
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
			OC::QualityOfService::LowQos,
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
	dlog_print(DLOG_INFO, LOG_TAG,  "Setting user %d and scene = %d", s->user, s->scene);
	switch(s->res->restype){
		case VEHICLECONTROL:{
	    	rep.setValue<int>("scene", s->scene);
	    	rep.setValue<int>("user",  s->user);
	    	rep.setValue<int>("gesture",  s->gesture);
	    }
	    break;
		default:
			dlog_print(DLOG_INFO, LOG_TAG,  "MAX_RESOURCES ???");
			break;
	}
	dlog_print(DLOG_INFO, LOG_TAG,  "Representation Set");
}

void postOCF(geniviocf_s *s)
{
	string key;
	OCRepresentation rep;

	string temp = s->res->ruri;
	temp += s->res->sid;
	dlog_print(DLOG_INFO, LOG_TAG,  "-----------------------Looking for   %s", temp.c_str());
	std::shared_ptr<OCResource> resource = lrmap[temp];
	if(resource){
		setRepresentation(rep, s);
		dlog_print(DLOG_INFO, LOG_TAG,  "POST %s", globres[s->res->restype].ruri);
#ifdef OCF
		resource->post(rep, QueryParamsMap(), &onPost);
#endif
	}else{
		dlog_print(DLOG_INFO, LOG_TAG,  "Resource Unavailable %s", globres[s->res->restype].ruri);
	}
}


void findDevices(appdata_s *ad){
#ifdef OCF

	for(int i = VEHICLECONTROL; i<MAX_RESOURCES; i++){
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
	start = VEHICLECONTROL;
	end =VEHICLECONTROL;
	for(int i = VEHICLECONTROL; i<MAX_RESOURCES; i++){
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

void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
                    const int& eCode, const int& sequenceNumber)
{
    try
    {
        if(eCode == OC_STACK_OK)
        {
            if(sequenceNumber == OC_OBSERVE_REGISTER)
            {

            	dlog_print(DLOG_INFO, LOG_TAG, "Observe registration action is successful");
            }
            else
            {
				lr_resources_s temp;
				dlog_print(DLOG_INFO, LOG_TAG,  "onObserve %s", rep.getUri().c_str());
				if(rep.getUri().compare("/doors/vs/0")==0)
				{
					vector<OCRepresentation> items;
					string doorstate;
					string result;
					int id;

					char doornames[][30] = {
							"Cooler Door is ",
							"Freezer Door is ",
							"CVRoom Door is "
					};

					rep.getValue("x.com.samsung.da.items", items);
					for(int i = 0; i<3; i++){
						items[i].getValue("x.com.samsung.da.id", id);
						items[i].getValue("x.com.samsung.da.openState", doorstate);
						result += doornames[id];
						result += doorstate;
						result += " : ";
					}
					temp.restype = FHNOTI;
					strcpy(temp.sid, result.c_str());
					send_message(&temp);
				}
            }
        }
		else
		{
			dlog_print(DLOG_INFO, LOG_TAG, "Observe registration action is unsuccessful");
		}
    }
    catch(std::exception& e)
    {
    	dlog_print(DLOG_INFO, LOG_TAG, "Observe registration action has exception");
    }

}


void observeOCF(lr_resources_s *s)
{
	dlog_print(DLOG_INFO, LOG_TAG,  "Can we observe %s found = %d", s->ruri, s->found);
	if(s->found)
	{
		string key = s->ruri;
		dlog_print(DLOG_INFO, LOG_TAG,  "Observing %s", s->ruri);
		key += s->sid;
		key += s->name;
		std::shared_ptr<OCResource> res = lrmap[key];
		if(res)
		{
			res->observe(ObserveType::ObserveAll, QueryParamsMap(), &onObserve);
		}
	}
}

