#include <glib.h>
#include <w3cocf.h>
#include "view.h"

#define MSGPORT "s3port"

static tinnos_signal_s cache[Signal_Max] = {
		{Signal_Body_Lights_IsLowBeamOn, 								BOOL, 	false, 	"Beam", 	"%s", 			"N/A", "    LowBeam : %s      ", 		{false}},
		{Signal_Drivetrain_BatteryManagement_BatteryStatus,				BOOL, 	false, 	"Btry", 	"%s", 			"N/A", "    BatteryStatus : %s",		{false}},
		{Signal_Drivetrain_Transmission_Speed,							INT, 	false, 	"kmph", 	"%d", 			"N/A", "    Tx Speed %d kmph  ",		{0}},
		{Signal_Drivetrain_InternalCombustionEngine_OilGauge,			BOOL, 	false, 	"Oil", 		"%s", 			"N/A", "    OilGauge %s       ",		{false}},
		{Signal_Chassis_Axle_Row1_Wheel_Right_Tire_Pressure,			DOUBLE, false, 	"psi", 		"%3.2f", 		"N/A", "    Row1->psi %3.2f   ",		{0.00}},
		{Signal_Chassis_Axle_Row1_Wheel_Right_Tire_PressureLow,			BOOL, 	false, 	"1R", 		"%s", 			"N/A", "    Row1->IsLow %s    ",		{false}},
		{Signal_Chassis_Axle_Row1_Wheel_Right_Tire_Temperature,			DOUBLE,	false, 	"°C", 		"%3.2f", 		"N/A", "    Row1->°C %3.2f    ",		{0.00}},
		{Signal_Chassis_Axle_Row1_Wheel_Left_Tire_Pressure,				DOUBLE, false, 	"psi", 		"%3.2f", 		"N/A", "    Row1<-psi %3.2f   ",		{0.00}},
		{Signal_Chassis_Axle_Row1_Wheel_Left_Tire_PressureLow,			BOOL, 	false, 	"1L", 		"%s", 			"N/A", "    Row1<-IsLow %s    ",		{false}},
		{Signal_Chassis_Axle_Row1_Wheel_Left_Tire_Temperature,			DOUBLE,	false, 	"°C", 		"%3.2f", 		"N/A", "    Row1<-°C %3.2f    ",		{0.00}},
		{Signal_Chassis_Axle_Row2_Wheel_Right_Tire_Pressure,			DOUBLE, false, 	"psi", 		"%3.2f", 		"N/A", "    Row2<-psi %3.2f   ",		{0.00}},
		{Signal_Chassis_Axle_Row2_Wheel_Right_Tire_PressureLow,			BOOL, 	false, 	"2R", 		"%s", 			"N/A", "    Row2->IsLow %s    ",		{false}},
		{Signal_Chassis_Axle_Row2_Wheel_Right_Tire_Temperature,			DOUBLE,	false, 	"°C", 		"%3.2f", 		"N/A", "    Row2->°C %3.2f    ",		{0.00}},
		{Signal_Chassis_Axle_Row2_Wheel_Left_Tire_Pressure,				DOUBLE, false, 	"psi", 		"%3.2f", 		"N/A", "    Row2->psi %3.2f   ",		{0.00}},
		{Signal_Chassis_Axle_Row2_Wheel_Left_Tire_PressureLow,			BOOL, 	false, 	"2L", 		"%s", 			"N/A", "    Row2<-IsLow %s    ",		{false}},
		{Signal_Chassis_Axle_Row2_Wheel_Left_Tire_Temperature,			DOUBLE,	false, 	"°C", 		"%3.2f", 		"N/A", "    Row2<-°C %3.2f    ",		{0.00}},
		{Signal_Drivetrain_FuelSystem_Level,							INT, 	false, 	"lts", 		"%d", 			"N/A", "    Fuel Level    %d l",		{0}},
		{Signal_Body_Outside_Temperature,								DOUBLE,	false, 	"°C", 		"%3.2f",		"N/A", "    Out Temp %3.2f    ",		{0.00}},
		{Signal_Body_Lights_IsParkingOn,								BOOL, 	false, 	"Park", 	"%s", 			"N/A", "    Parking        %s ",		{false}},
		{Signal_Drivetrain_InternalCombustionEngine_EOP,				BOOL, 	false, 	"EOP", 		"%s", 			"N/A", "    Engine Oil PSI %s ",		{false}}
};


#ifdef OCF

#include <map>
#include <cstdlib>
#include <functional>
#include <string>
#include <mutex>
#include <condition_variable>
#include "OCPlatform.h"
#include "OCApi.h"
using namespace OC;
namespace PH = std::placeholders;
using namespace std;

static const char* SVR_DB_FILE_NAME = "./oic_svr_db_server.dat";
std::string  platformId = "0A3E0D6F-DBF5-404E-8719-D6880042463A";
std::string  manufacturerName = "Tinnos";
std::string  manufacturerLink = "https://www.iotivity.org";
std::string  modelNumber = "0.1";
std::string  dateOfManufacture = "2016-01-15";
std::string  platformVersion = "1.2.0";
std::string  operatingSystemVersion = "Yocto";
std::string  hardwareVersion = "0.1";
std::string  firmwareVersion = "1.0";
std::string  supportLink = "https://www.iotivity.org";
std::string  systemTime = "2016-01-15T11.01";
std::string  deviceName = "W3C OCF AVN Cluster";
std::string  specVersion = "core.1.1.0";
std::string  dataModelVersions = "res.1.1.0";
OCPlatformInfo platformInfo;
OCDeviceInfo deviceInfo;

class AVNResource
{
public:
    /// Access this property from a TB client
    std::string m_name;
    std::string m_avnUri;

    OCResourceHandle m_resourceHandle;

    AVNResource():m_name("Tinnos AVN"), m_avnUri("/a/w3cocf/avn"), m_resourceHandle(nullptr) {
	}

    OCRepresentation post(OCRepresentation& rep){

    	dlog_print(DLOG_DEBUG, LOG_TAG, "POST received \n");

    	char val[5];
    	bool refresh = false;

    	for(int i = 0; i<Signal_Max; i++){
    		sprintf(val, "p%d", i);
    		tinnos_signal_s sig;
			switch(cache[i].t){
   				case BOOL:
   					rep.getValue(val, sig.b);
   					if(cache[i].b!=sig.b){
   						cache[i].b = sig.b;
   						cache[i].ch = true;
   	   					dlog_print(DLOG_DEBUG, LOG_TAG, "Received %s = %s = %s", val, cache[i].n, (cache[i].b)?"On" : "Off");
   					}
   					break;
   				case DOUBLE:
   					rep.getValue(val, sig.d);
   					if(cache[i].d!=sig.d){
   						cache[i].d = sig.d;
   						cache[i].ch = true;
   	   					dlog_print(DLOG_DEBUG, LOG_TAG, "Received %s = %s = %f", val, cache[i].n, cache[i].d);
   					}
   					break;
   				case INT:
   					rep.getValue(val, sig.i);
   					if(cache[i].i!=sig.i){
   						cache[i].i = sig.i;
   						cache[i].ch = true;
   	   					dlog_print(DLOG_DEBUG, LOG_TAG, "Received %s = %s = %d", val, cache[i].n, cache[i].i);
   					}
   					break;
   				default:
   					dlog_print(DLOG_DEBUG, LOG_TAG, "Unsupported Data Type");
   					break;

    		}
			refresh |= cache[i].ch;
    	}


    	send_message(refresh);
    	dlog_print(DLOG_DEBUG, LOG_TAG, "Refresh : %d", refresh);
		return rep;

    }

    void createResource()
    {
        std::string resourceURI = m_avnUri;
        std::string resourceTypeName = "oic.d.avn";
        std::string resourceInterface = DEFAULT_INTERFACE;
        uint8_t resourceProperty;
        resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
        EntityHandler cb = std::bind(&AVNResource::entityHandler, this,PH::_1);
        OCStackResult result = OCPlatform::registerResource(
                                    m_resourceHandle, resourceURI, resourceTypeName,
                                    resourceInterface, cb, resourceProperty);

        if (OC_STACK_OK != result)
        {
            dlog_print(DLOG_DEBUG, LOG_TAG, "Resource creation was unsuccessful\n");
		}
    }
    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
	}

    void addType(const std::string& type) const
    {
        OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle, type);
        if (OC_STACK_OK != result)
        {
            dlog_print(DLOG_DEBUG, LOG_TAG,  "Binding TypeName to Resource was unsuccessful\n");
		}
    }

    void addInterface(const std::string& iface) const
    {
        OCStackResult result = OCPlatform::bindInterfaceToResource(m_resourceHandle, iface);
        if (OC_STACK_OK != result)
        {
            dlog_print(DLOG_DEBUG, LOG_TAG,  "Binding TypeName to Resource was unsuccessful\n");
        }
    }

private:
	OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG,  "\tIn Server CPP entity handler:\n");
		OCEntityHandlerResult ehResult = OC_EH_ERROR;
		if(request)
		{
			// Get the request type and request flag
			std::string requestType = request->getRequestType();
			int requestFlag = request->getRequestHandlerFlag();

			if(requestFlag & RequestHandlerFlag::RequestFlag)
			{
				dlog_print(DLOG_DEBUG, LOG_TAG,  "\t\trequestFlag : Request\n");
				auto pResponse = std::make_shared<OC::OCResourceResponse>();
				pResponse->setRequestHandle(request->getRequestHandle());
				pResponse->setResourceHandle(request->getResourceHandle());

				if(requestType == "POST")
				{
					dlog_print(DLOG_DEBUG, LOG_TAG,  "\t\t\trequestType : POST\n");

					OCRepresentation rep = request->getResourceRepresentation();
					OCRepresentation rep_post = post(rep);
					//pResponse->setResourceRepresentation(rep_post);
					pResponse->setErrorCode(200);
					if(rep_post.hasAttribute("createduri"))
					{
						pResponse->setResponseResult(OC_EH_RESOURCE_CREATED);
						pResponse->setNewResourceUri(rep_post.getValue<std::string>("createduri"));
						dlog_print(DLOG_DEBUG, LOG_TAG,  "\t\t\trequestType : POST\n");
					}
					else
					{
						pResponse->setResponseResult(OC_EH_OK);
						dlog_print(DLOG_DEBUG, LOG_TAG,  "\t\t\retype : POST\n");
					}

					if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
					{
						ehResult = OC_EH_OK;
						dlog_print(DLOG_DEBUG, LOG_TAG,  "POST response sent");
					}else{
						dlog_print(DLOG_DEBUG, LOG_TAG,  "POST response NOT sent");
					}
				}
			}
		}
		else
		{
			dlog_print(DLOG_DEBUG, LOG_TAG, "Request invalid" );;
		}
		return ehResult;
	}

};

void DeleteDeviceInfo()
{
    delete[] deviceInfo.deviceName;
    delete[] deviceInfo.specVersion;
    //OCFreeOCStringLL(deviceInfo.dataModelVersions);
}

void DuplicateString(char ** targetString, std::string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

OCStackResult SetPlatformInfo(std::string platformID, std::string manufacturerName,
        std::string manufacturerUrl, std::string modelNumber, std::string dateOfManufacture,
        std::string platformVersion, std::string operatingSystemVersion,
        std::string hardwareVersion, std::string firmwareVersion, std::string supportUrl,
        std::string systemTime)
{
    DuplicateString(&platformInfo.platformID, platformID);
    DuplicateString(&platformInfo.manufacturerName, manufacturerName);
    DuplicateString(&platformInfo.manufacturerUrl, manufacturerUrl);
    DuplicateString(&platformInfo.modelNumber, modelNumber);
    DuplicateString(&platformInfo.dateOfManufacture, dateOfManufacture);
    DuplicateString(&platformInfo.platformVersion, platformVersion);
    DuplicateString(&platformInfo.operatingSystemVersion, operatingSystemVersion);
    DuplicateString(&platformInfo.hardwareVersion, hardwareVersion);
    DuplicateString(&platformInfo.firmwareVersion, firmwareVersion);
    DuplicateString(&platformInfo.supportUrl, supportUrl);
    DuplicateString(&platformInfo.systemTime, systemTime);

    return OC_STACK_OK;
}

OCStackResult SetDeviceInfo(std::string deviceName, std::string specVersion, std::string dataModelVersions)
{
    DuplicateString(&deviceInfo.deviceName, deviceName);

    if (!specVersion.empty())
    {
        DuplicateString(&deviceInfo.specVersion, specVersion);
    }

    if (!dataModelVersions.empty())
    {
        //           OCResourcePayloadAddStringLL(&deviceInfo.dataModelVersions, dataModelVersions.c_str());
    }

    return OC_STACK_OK;
}


void DeletePlatformInfo()
{
    delete[] platformInfo.platformID;
    delete[] platformInfo.manufacturerName;
    delete[] platformInfo.manufacturerUrl;
    delete[] platformInfo.modelNumber;
    delete[] platformInfo.dateOfManufacture;
    delete[] platformInfo.platformVersion;
    delete[] platformInfo.operatingSystemVersion;
    delete[] platformInfo.hardwareVersion;
    delete[] platformInfo.firmwareVersion;
    delete[] platformInfo.supportUrl;
    delete[] platformInfo.systemTime;
}

static FILE* client_open(const char* /*path*/, const char *mode)
{
    return fopen(SVR_DB_FILE_NAME, mode);
}

pthread_t g_thread;


void *GMainLoopThread(void *param)
{
    OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "0.0.0.0",
        0,
        OC::QualityOfService::HighQos,
        &ps
    };
    OCPlatform::Configure(cfg);
    OCStackResult result = SetPlatformInfo(platformId, manufacturerName, manufacturerLink,
            modelNumber, dateOfManufacture, platformVersion, operatingSystemVersion,
            hardwareVersion, firmwareVersion, supportLink, systemTime);
    result = OCPlatform::registerPlatformInfo(platformInfo);
    if (result != OC_STACK_OK)
    {
        dlog_print(DLOG_DEBUG, LOG_TAG, "Platform Registration failed");
        return NULL;
    }
    dlog_print(DLOG_DEBUG, LOG_TAG, "Platform Registration succeeded");

    result = SetDeviceInfo(deviceName, specVersion, dataModelVersions);
    //OCResourcePayloadAddStringLL(&deviceInfo.types, "oic.wk.d");
    result = OCPlatform::registerDeviceInfo(deviceInfo);
    if (result != OC_STACK_OK)
    {
        dlog_print(DLOG_DEBUG, LOG_TAG, "Device  Registration failed");
        return NULL;
    }
    dlog_print(DLOG_DEBUG, LOG_TAG, "Device Registration succeeded");

    try
    {
        AVNResource myAVN;
        myAVN.createResource();
        dlog_print(DLOG_DEBUG, LOG_TAG, "Created resource." );;
        myAVN.addType(std::string("oic.d.avn"));
        myAVN.addInterface(std::string(LINK_INTERFACE));
        dlog_print(DLOG_DEBUG, LOG_TAG, "Added Interface and Type" );;
//        DeletePlatformInfo();
//        DeleteDeviceInfo();
        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        dlog_print(DLOG_DEBUG, LOG_TAG, "Waiting for Events" );
        cv.wait(lock, []{return false;});
    }
	catch(OCException &e)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "OCException in main : ");
	}
    dlog_print(DLOG_DEBUG, LOG_TAG, "Done Processing" );
    return NULL;
}

#endif


char *get_signal_name(int i){
	if(i>=0 && i<Signal_Max){
		return cache[i].n;
	}
	else
		return (char *)"N/A";
}

tinnos_signal_s *get_tinnos_signal(int i){
	if(i>=0 && i<Signal_Max){
		return &cache[i];
	}
	return NULL;
}

char *get_signal_value(int i, bool l){
	if(i>=0 && i<Signal_Max){
		dlog_print(DLOG_DEBUG, LOG_TAG, "format = %d", l);
		char *ptr = (l)?cache[i].l:cache[i].f;
// FIXME : final fiz
//		if(!cache[i].ch)
//			return cache[i].v;

		if(l)dlog_print(DLOG_DEBUG, LOG_TAG, "Signal %s has changed %d format = %s", cache[i].n, cache[i].ch, ptr);

		switch(cache[i].t){
			case BOOL:
				sprintf(cache[i].v, ptr, (cache[i].b)?"On":"Off");
				break;
			case INT:
				sprintf(cache[i].v, ptr, cache[i].i);
				break;
			case DOUBLE:
				sprintf(cache[i].v, ptr, cache[i].d);
				break;
			default:
				break;
		}
		//dlog_print(DLOG_DEBUG, LOG_TAG, "Getting Signal %s Value = %s", cache[i].n, cache[i].v);
		cache[i].ch = false;
		return cache[i].v;
	}
	return (char *)"N/A";
}

void  send_message(bool refresh)
{
	bundle *b = bundle_create();
	bundle_add_byte(b, "w3cocfmsg", (void *)&refresh, sizeof(bool));
	int ret = message_port_send_message(PACKAGE, MSGPORT, b);
	dlog_print(DLOG_INFO, LOG_TAG, "Message port send status = %d", ret);
}

static void message_port_cb(int id, const char *remote_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *data)
{
	size_t size;
	void *ptr;
	bundle_get_byte(message, "w3cocfmsg", &ptr, &size);
	dlog_print(DLOG_INFO, LOG_TAG, "message_port_cb size received = %d refresh=%d", size, *(bool *)ptr);

	if(*(bool *)ptr){
		refresh_dial((appdata_s*)data);
		dlog_print(DLOG_INFO, LOG_TAG, "dial_refresh=%d", *(bool *)ptr);

	}
}

static bool app_create(void *user_data)
{
	appdata_s *ad = (appdata_s *)user_data;
	create_base_gui(ad);
	return true;
}

static void app_control(app_control_h app_control, void *user_data)
{
}

static void app_pause(void *user_data)
{
}

static void app_resume(void *user_data)
{
}

static void app_terminate(void *user_data)
{
	view_destroy(user_data);
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);

	if (locale != NULL) {
		elm_language_set(locale);
		free(locale);
	}
	return;
}

int main(int argc, char *argv[])
{
	int ret;

	appdata_s ad;
	ad.mouse_down_dial_num = -1;
	ui_app_lifecycle_callback_s event_callback = {0, };
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);

	//thread_blocker_cb(NULL, NULL);

    int port = message_port_register_local_port(MSGPORT, &message_port_cb, &ad);
    dlog_print(DLOG_INFO, LOG_TAG, "Local Message Port = %d", port);


#ifdef OCF
	int result = pthread_create(&g_thread, NULL, GMainLoopThread, (void *)&ad);
	if (result < 0)
	{
		dlog_print(DLOG_INFO, LOG_TAG, "pthread_create failed in initialize\n");
		return 0;
	}
#endif

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
	}

	return ret;
}


