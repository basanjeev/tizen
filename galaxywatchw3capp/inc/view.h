/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

#if !defined(_VIEW_H)
#define _VIEW_H

#include <Elementary.h>
#include <efl_extension.h>
#include <app_preference.h>
#define KEY_END "XF86Stop"

#define RES_DIR "/opt/usr/apps/org.w3c.galaxywatchw3capp/res/"
#define ICON_DIR RES_DIR"images"

#define EDJ_DASH_FILE "edje/dash.edj"
#define EDJ_WHEEL_FILE "edje/wheel.edj"
#define GRP_MAIN "main"

#define RADIUS_MIN 82
#define CENTER_REF_X 180
#define CENTER_REF_Y 180

/* Slope value for circular touch layer */
#define SLOPE_36_DEGREE 0.7265
#define SLOPE_72_DEGREE 3.0776
#define SLOPE_108_DEGREE -3.0776
#define SLOPE_144_DEGREE -0.7265
#define SLOPE_180_DEGREE 0

#define DAIL_INPUT_MAX 128

/* Text Style for Dial Entry */
#define DIAL_TEXT_STYLE_NORMAL "DEFAULT='font=Tizen:style=Light font_size=34 align=center'"
#define DIAL_TEXT_STYLE_SMALL "DEFAULT='font=Tizen:style=Light font_size=28 align=center'"
#define DIAL_TEXT_STYLE_SMALLER "DEFAULT='font=Tizen:style=Regular font_size=23 align=center'"

#define ENTRY_TEXT_MAX DAIL_INPUT_MAX

/* Entry text Control Option */
#define ENTRY_TEXT_ADD_TEXT 1

#define LONGPRESS_TIME 0.5f

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
} w3c_avn_e;

typedef enum {
	BOOL,
	DOUBLE,
	INT,
	TYPE_MAX
}w3c_data_e;

typedef struct {
	w3c_avn_e s;
	char path[128];
	w3c_data_e t;
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
}w3c_signal_s;

void view_destroy(void *data);
void create_base_gui(void *data);
char *get_signal_name(int i);
char *get_signal_value(int i, bool l);

#endif
