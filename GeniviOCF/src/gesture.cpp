/*
 * gesture.cpp
 *
 *  Created on: Dec 11, 2016
 *      Author: sanjeev
 */


#include <gesture_recognition.h>
#include <geniviocf.h>



void list_all_gestures()
{

	for(int i = 1 ; i<=GESTURE_WRIST_UP; i++){
		bool supported = false;
		gesture_is_supported((gesture_type_e)i, &supported);

		if (!supported) {
			dlog_print(DLOG_INFO, LOG_TAG,  "%d UN suported = %d\n", i, supported);
		}else{
			dlog_print(DLOG_INFO, LOG_TAG,  "%d suported = %d\n", i, supported);
		}
	}
}
