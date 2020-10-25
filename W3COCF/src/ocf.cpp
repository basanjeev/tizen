#include "livingroom.h"
using namespace std;

using namespace OC;
namespace PH = std::placeholders;
typedef std::map<std::string, std::shared_ptr<OCResource>> DiscoveredResourceMap;
DiscoveredResourceMap lrmap;

extern int intialized;

static char devnames[][10]{
		"TV",
		"FAMILYHUB",
		"AC",
		"RVC",
		"SURFACE"
};

static lr_resources_s globres[] = {
//Samsung TV Resources
	{TVPOWER,		"TV Power", 			"/sec/tv/switch/binary", 				ICON_DIR"/gen/power.png", 	TV,		true},
	{VOLMUTE,		"TV Volume", 				"/sec/tv/audio", 						ICON_DIR"/tv/vol.png", 		TV, 	false},
	{PICMODE,		"TV Picture Mode", 				"/sec/tv/mode/picture", 				ICON_DIR"/tv/pm.png", 		TV, 	false},
	{SOUNDMODE, 	"TV Sound Mode", 				"/sec/tv/mode/sound", 					ICON_DIR"/tv/sm.png", 		TV, 	false},
	{SOURCE, 		"TV Source", 				"/sec/tv/mediaInput", 					ICON_DIR"/tv/src.png", 		TV, 	false},

//Samsung Family Hub Resources
	{FHDCV, 		"CVRoom Door",			"/door/cvroom/0", 						ICON_DIR"/fh/dcv.png", 		FAMILYHUB, 	false},
	{FHDF,	 		"Freezer Door",			"/door/freezer/0", 						ICON_DIR"/fh/df.png", 		FAMILYHUB, 	false},
	{FHDC, 			"Cooler Door",			"/door/cooler/0", 						ICON_DIR"/fh/dc.png", 		FAMILYHUB, 	false},
	{FHFREEZER,		"Freezer",				"/refrigeration/0", 					ICON_DIR"/fh/rf.png", 		FAMILYHUB, 	false},
	{FHCCT,			"Current Cooler",		"/temperature/current/cooler/0", 		ICON_DIR"/fh/cc.png", 		FAMILYHUB, 	false},
	{FHCDT,			"Desired Cooler",		"/temperature/desired/cooler/0", 		ICON_DIR"/fh/cd.png", 		FAMILYHUB, 	false},
	{FHFCT,			"Current Freezer",		"/temperature/current/freezer/0", 		ICON_DIR"/fh/fc.png", 		FAMILYHUB, 	false},
	{FHFDT,			"Desired Freezer",		"/temperature/desired/freezer/0", 		ICON_DIR"/fh/fd.png", 		FAMILYHUB, 	false},
	{FHCVCT,		"Current CVRoom",		"/temperature/current/cvroom/0", 		ICON_DIR"/fh/cvc.png", 		FAMILYHUB, 	false},
	{FHCVDT,		"Desired CVRoom",		"/temperature/desired/cvroom/0", 		ICON_DIR"/fh/cvd.png", 		FAMILYHUB, 	false},
	{FHMODE,		"FamilyHub Mode",		"/mode/0",								ICON_DIR"/fh/fhmode.png",	FAMILYHUB, 	false},

//Samsung A/C Resources
	{ACPOWER, 		"On/Off", 				"/power/0", 							ICON_DIR"/gen/power.png", 	AC, 	false},
	{AIRFLOW, 		"AirFlow", 				"/airflow/0", 							ICON_DIR"/ac/airflow.png",	AC, 	false},
	{ACMODE, 		"Mode", 				"/mode/0", 								ICON_DIR"/ac/mode.png", 		AC, 	false},
	{ACTEMP, 		"Temperature",			"/temperature/desired/0", 				ICON_DIR"/ac/temp.png", 		AC, 	false},
	{ACSENSORS, 	"Sensors",				"/sensors/0", 							ICON_DIR"/ac/sensor.png", 	AC, 	false},

//Samsung RVC Resources
	{RVCPOWER, 		"On/Off", 				"/power/0", 							ICON_DIR"/gen/power.png", 	RVC, 	false},
	{RVCMODE, 		"Mode", 				"/mode/0", 								ICON_DIR"/rvc/mode.png", 		RVC, 	false},

#if 0
//Alpha Nodus Thermostat Resources
	{AN_AMB,		"Ambient", 				"/AN/org/orgID/dev/devID/ambtemp", 		ICON_DIR"/an/lt.png", 		DUMMY_DEVID, 	false},
	{AN_AMB_UNIT,	"Ambient Unit",			"/AN/org/orgID/dev/devID/ambtemp", 		ICON_DIR"/gen/celcius.png", 	DUMMY_DEVID, 	false},
	{AN_SETPOINT,	"SetPoint", 			"/AN/org/orgID/dev/devID/sptemp", 		ICON_DIR"/an/rt.png", 		DUMMY_DEVID, 	false},
	{AN_SP_UNIT,	"SetPoint Unit",		"/AN/org/orgID/dev/devID/sptemp", 		ICON_DIR"/gen/farenheit.png", 	DUMMY_DEVID, 	false},
	{AN_SPNAME,		"SetPoint Mode Name",	"/AN/org/orgID/dev/devID/spmode", 		ICON_DIR"/an/mute.png", 		DUMMY_DEVID, 	false},
	{AN_SPMODE,		"SetPoint Mode",		"/AN/org/orgID/dev/devID/spmode", 		ICON_DIR"/an/mute.png", 		DUMMY_DEVID, 	false},
	{AN_HVACNAME,	"HVAC Mode Name",		"/AN/org/orgID/dev/devID/hvacmode", 	ICON_DIR"/an/hvac.png", 		DUMMY_DEVID, 	false},
	{AN_HVACMODE,	"HVAC Mode",			"/AN/org/orgID/dev/devID/hvacmode", 	ICON_DIR"/an/hvac.png", 		DUMMY_DEVID, 	false},
#endif

//Surface Server Resources
	{MSP4, 			"Scene Control",		"/a/scenecontrol", 						ICON_DIR"/scenes/s%d.png", 		SURFACE, 		false}

};

lr_resources_s *get_res_by_uri(const char *ruri){
	for(int i = TVPOWER; i<MAX_RESOURCES; i++){
		if(!strcmp(globres[i].ruri, ruri))
			return &globres[i];
	}
	return NULL;
}

char *get_device_name(devtype_e devtype){
	if(devtype>=TV && devtype<MAX_DEVICES)
		return devnames[devtype];
	else
		return "OCF Device";
}

lr_resources_s *get_res_by_devtype(devtype_e devtype){
	for(int i = TVPOWER; i<MAX_RESOURCES; i++){
		if(devtype == globres[i].devtype)
			return &globres[i];
	}
	return NULL;
}

lr_resources_s *get_res_by_name(const char *name){
	for(int i = TVPOWER; i<MAX_RESOURCES; i++){
		if(!strcmp(globres[i].name, name))
			return &globres[i];
	}
	return NULL;
}

lr_resources_s *get_res_by_restype(ocfrestype_e restype){
	if(restype>=TVPOWER && restype <MAX_RESOURCES){
		return &globres[restype];
	}
	return NULL;
}

lr_resources_s *get_livingroom_resource_value(ocfrestype_e res){
	if(res>=TVPOWER && res <MAX_RESOURCES){
		return &globres[res];
	}
	return NULL;
}

static FILE* client_open(const char* /*path*/, const char *mode)
{
//	/return fopen("./oic_svr_db_client.json", mode);
	return NULL;
}

void init_global_resources(appdata_s *ad){
	ad->master.data.tv.count = 5;
	ad->master.data.fh.count = 11;
	ad->master.data.ac.count = 5;
	ad->master.data.rvc.count = 2;
	ad->master.data.surface.count = 1;
}

void print_glob(lr_resources_s *data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "Details of %d", data->restype);
	dlog_print(DLOG_INFO, LOG_TAG, "Name %s", data->name);
	dlog_print(DLOG_INFO, LOG_TAG, "URI  %s", data->ruri);
	dlog_print(DLOG_INFO, LOG_TAG, "URI  %s", data->ruri);
}

void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
                    const int& eCode, const int& sequenceNumber)
{
    try
    {
//        if(eCode == OC_STACK_OK)
//        {
//            if(sequenceNumber == OC_OBSERVE_REGISTER)
//            {
//            	dlog_print(DLOG_INFO, LOG_TAG, "Observe registration action is successful");
//            }
//
//            int i = 0;
//            dlog_print(DLOG_INFO, LOG_TAG, "OBSERVE RESULT:");
//            rep.getValue("scene", i);
//            lr_resources_s temp;
//            temp.state = MSP4;
//            temp.data.i = i;
//            send_message(&temp);
//            dlog_print(DLOG_INFO, LOG_TAG, "OBSERVE RESULT: %d", temp.data.i);
//        }
//        else
//        {
//        	dlog_print(DLOG_INFO, LOG_TAG, "Observe registration action is unsuccessful");
//        }
    }
    catch(std::exception& e)
    {
    	dlog_print(DLOG_INFO, LOG_TAG, "Observe registration action has exception");
    }

}

void foundResource(std::shared_ptr<OCResource> resource)
{
#if 0
	std::string resourceURI;
	std::string hostAddress;
	try
	{
		resourceURI = resource->uri();
		if (lrmap.find(resourceURI) == lrmap.end())
		{

			for(int i = 0; i<MAX_RESOURCES; i++){
				if (resourceURI.compare(globres[i].ruri) == 0){

					lrmap[resourceURI] = resource;
					globres[i].found = true;

					if(i==MSP4){
						dlog_print(DLOG_INFO, LOG_TAG,  "Resource detected %s", resourceURI.c_str());
						send_message(&globres[MSP4]);
						resource->observe(ObserveType::ObserveAll, QueryParamsMap(), &onObserve);
					}
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

void initOCFStack(appdata_s *ad)
{
#ifdef OCF
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
		requestURI << OC_RSRVD_WELL_KNOWN_URI;
		OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResource);
		dlog_print(DLOG_INFO, LOG_TAG,  "Finding Resource... ");
	}catch(OCException& e)
	{
		oclog() << "Exception in main: "<<e.what();
	}
#endif
}

void onPost(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
	dlog_print(DLOG_INFO, LOG_TAG,  "PUT request response code = %d ", eCode);
}

void onGet(const HeaderOptions&, const OCRepresentation& rep, const int eCode)
{
	const char *resUri = "";
	if(eCode == OC_STACK_OK)
	{
		dlog_print(DLOG_INFO, LOG_TAG,  "GET request was successful %d", eCode);
#ifdef OCF
		resUri = rep.getUri().c_str();
#endif
		lr_resources_s *s = get_res_by_uri(resUri);
		if(s!=NULL){
			s->found = true;
		}
	}
	else
	{
		dlog_print(DLOG_INFO, LOG_TAG,  "GET request was UNsuccessful %d", eCode);
	}
}

void setRepresentation(OCRepresentation &rep, lr_s *s){
	dlog_print(DLOG_INFO, LOG_TAG,  "Setting Representation for %s", s->res->name);
	switch(s->res->restype){
		case MSP4:
		{
			rep.setValue("scene", s->data.surface.scene.i);
			dlog_print(DLOG_INFO, LOG_TAG,  "Setting Scene %d", s->data.surface.scene.i);
		}
		break;
		//TV
	    TVPOWER:{
	    	rep.setValue("value", s->data.tv.power);
	    }
	    break;
	    VOLMUTE:
	    PICMODE:
	    SOUNDMODE:
		SOURCE:
		//Family Hub
		FHDCV:
		FHDF:
		FHDC:
		FHFREEZER:
		FHCCT:
		FHCDT:
		FHFCT:
		FHFDT:
		FHCVCT:
		FHCVDT:
		FHXXX:
		//AC
		ACPOWER:
		AIRFLOW:
		ACMODE:
		ACTEMP:
		ACSENSORS:
		//RVC
		RVCPOWER:
		RVCMODE:
		default:
			dlog_print(DLOG_INFO, LOG_TAG,  "Unsupported %s", s->res->name);
	}
}



void postOCF(lr_s *s)
{
	string key;
	OCRepresentation rep;
	dlog_print(DLOG_INFO, LOG_TAG,  "POSTING %s", globres[s->res->restype].ruri);
	std::shared_ptr<OCResource> resource = lrmap[s->res->ruri];
	if(resource){
		setRepresentation(rep, s);
#ifdef OCF
		resource->post(rep, QueryParamsMap(), &onPost);
#endif
	}

}


void putOCF(lr_s *s)
{
	string key;
	OCRepresentation rep;
	std::shared_ptr<OCResource> resource = lrmap[s->res->ruri];
	if(resource){
		if(s->res->restype ==MSP4){
			rep.setValue("scene", s->data.surface.scene.i);
			dlog_print(DLOG_INFO, LOG_TAG,  "Posting %d", s->data.surface.scene.i);
		}
		dlog_print(DLOG_INFO, LOG_TAG,  "POSTING %s", globres[s->res->restype].ruri);
#ifdef OCF
		resource->put(rep, QueryParamsMap(), &onPost);
#endif
	}

}

void getOCF(char *ruri)
{
	QueryParamsMap test;
	std::shared_ptr<OCResource> resource = lrmap[ruri];
#ifdef OCF
	if(resource)
		resource->get(test, &onGet);
#endif
}

void findDeviceResource(lr_s *s)
{
	int start = 0;
	int end = 0;

	switch(s->res->devtype)
	{
		case TV :{
			start = TVPOWER;
			end =SOURCE;
		}
		break;
		case FAMILYHUB :{
			start = FHDCV;
			end =FHMODE;
		}
		break;
		case AC:{
			start = ACPOWER;
			end =ACSENSORS;
		}
		break;
		case RVC:{
			start = RVCPOWER;
			end =RVCMODE;
		}
		break;
		default:
			return;
	}

	for(int i = start; i<=end; i++){
		dlog_print(DLOG_INFO, LOG_TAG,  "Getting Resource  %s", globres[i].ruri);
		if(!globres[i].found)
			getOCF(globres[i].ruri);
	}
}
