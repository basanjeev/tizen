#ifndef _TINNOS_SERVER_H_
#define _TINNOS_SERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>


typedef enum {
	Signal_Body_Lights_IsLowBeamOn,
	Signal_Drivetrain_BatteryManagement_BatteryStatus,
	Signal_Drivetrain_Transmission_Speed,
	Signal_Drivetrain_InternalCombustionEngine_OilGauge,
	Signal_Chassis_Axle_Row1_Wheel_Right_Tire_Pressure,
	Signal_Chassis_Axle_Row1_Wheel_Right_Tire_PressureLow,
	Signal_Chassis_Axle_Row1_Wheel_Right_Tire_Temperature,
	Signal_Chassis_Axle_Row1_Wheel_Left_Tire_Pressure,
	Signal_Chassis_Axle_Row1_Wheel_Left_Tire_PressureLow,
	Signal_Chassis_Axle_Row1_Wheel_Left_Tire_Temperature,
	Signal_Chassis_Axle_Row2_Wheel_Right_Tire_Pressure,
	Signal_Chassis_Axle_Row2_Wheel_Right_Tire_PressureLow,
	Signal_Chassis_Axle_Row2_Wheel_Right_Tire_Temperature,
	Signal_Chassis_Axle_Row2_Wheel_Left_Tire_Pressure,
	Signal_Chassis_Axle_Row2_Wheel_Left_Tire_PressureLow,
	Signal_Chassis_Axle_Row2_Wheel_Left_Tire_Temperature,
	Signal_Drivetrain_FuelSystem_Level,
	Signal_Body_Outside_Temperature,
	Signal_Body_Lights_IsParkingOn,
	Signal_Drivetrain_InternalCombustionEngine_EOP,
	Signal_Max
} tinnos_avn_e;


/*
 *
 * 	bool 	p0;
	bool 	p5;
	bool 	p1;
	int 	p2;
	bool 	p11;
	bool 	p3;
	bool 	p14;
	int 	p16;
	bool 	p19;
	bool 	p8;

	double 	p17;
	double 	p4;
	double 	p6;
	double 	p7;
	double 	p9;
	bool 	p18;
	double 	p10;
	double 	p12;
	double 	p13;
	double 	p15;
 *
 */

typedef enum {
	BOOL,
	DOUBLE,
	INT,
	TYPE_MAX
}tinnos_data_e;

typedef struct {
	tinnos_avn_e s;
	tinnos_data_e t;
	bool ch;

	char n[25]; //label
	char f[25]; //format
	char v[50]; //value
	char l[50]; //long form
	union {
		bool 	b;
		double 	d;
		int 	i;
	};
}tinnos_signal_s;


typedef enum
{
  MSG_TYPE_CLU_TO_IOTIVITY = 1,
  MSG_TYPE_IOTIVITY_TO_CLU
}MESSAGE_TYPE_CLU_IOTIVITY;

typedef enum
{
	MSG_ID_CLU_TO_IOTIVITY_INIT,
	MSG_ID_CLU_TO_IOTIVITY_START,
	MSG_ID_CLU_TO_IOTIVITY_END,
	MSG_ID_CLU_TO_IOTIVITY_SEND_DATA
}MESSAGE_ID_CLU_TO_IOTIVITY;

typedef enum
{
	MSG_ID_IOTIVITY_TO_CLU_INIT,
	MSG_ID_IOTIVITY_TO_CLU_READY,
	MSG_ID_IOTIVITY_TO_CLU_TEXT
}MESSAGE_ID_IOTIVITY_TO_CLU;

typedef struct {
	int 	speed;
	int	fuel;
	int	outTemperature;
}IOTIVITY_GAUGE_INFO;

typedef struct
{
	int 	frontRightTirePressure;
	bool  frontRightTirePressureLow;
	int  	frontRightTirePressureTemperature;
	int 	frontLeftTirePressure;
	bool  frontLeftTirePressureLow;
	int  	frontLeftTirePressureTemperature;
	int 	rearRightTirePressure;
	bool  rearRightTirePressureLow;
	int  	rearRightTirePressureTemperature;
	int 	rearLeftTirePressure;
	bool  rearLeftTirePressureLow;
	int 	rearLeftTirePressureTemperature;
}IOTIVITY_TPMS_INFO;

typedef struct
{
	bool	lowBeam;
	bool	charge;
	bool  parking;
	bool	checkEngine;
	bool	engineOil;
}IOTIVITY_TELLTALE_INFO;

typedef struct
{
    long 														msgType;
    MESSAGE_ID_CLU_TO_IOTIVITY			msgID;
    IOTIVITY_TELLTALE_INFO					telltale;
    IOTIVITY_GAUGE_INFO 						gauge;
    IOTIVITY_TPMS_INFO							tpms;
} MSG_BUF_CLU_TO_IOTIVITY_TELLTALE;

typedef struct
{
    long 														msgType;
    MESSAGE_ID_IOTIVITY_TO_CLU			msgID;
} MSG_BUF_IOTIVITY_TO_CLU;


void sendOCFPayload(MSG_BUF_CLU_TO_IOTIVITY_TELLTALE *);
void sendW3CPayload(MSG_BUF_CLU_TO_IOTIVITY_TELLTALE *);


#endif
