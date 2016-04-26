/*
 * ReadSensor.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */
#include "PE_Types.h"
#include "ReadSensor.h"
#include "H3LIS331DL.h"
#include "Error.h"
#include "Queue.h"
#include "calibH3LI.h"

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

uint16_t count_or;

bool newDataAvailableFlag = FALSE;
bool dataOverrunFlag = FALSE;

bool measureEnabledFlag = FALSE;

void saveInMemory(int16_t value);

void logAccData(void);

void ReadAccelSensorTask(void *pvParameters){
	initH3LI();														/* init accelerometer */
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if(measureEnabledFlag){
			isNewDataAvailable(Z_AXIS_DA, &newDataAvailableFlag); 	/* check if new data available */
			if(newDataAvailableFlag == TRUE){
				WAIT1_Waitus(2);
				logAccData();										/* read sensor and save on SD card */
			}
			WAIT1_Waitus(2);
			dataOverrun(Z_AXIS_OR, &dataOverrunFlag);				/* data overrun? */
			if(dataOverrunFlag == TRUE){
				count_or++;											/* count overruns */
			}
		}
		FRTOS1_vTaskDelayUntil(&xLastWakeTime, 1/portTICK_RATE_MS);
	}
}

void logAccData(void){
	  z = getAccData();
	  DATAQUEUE_SaveValue(z);										/* save in queue */
	  if ((z>gain) || (z< -gain)){									/* greater than 1g? */
		  LED_G_On();
	  }
	  else{
		  LED_G_Off();
	  }
}

void setMeasurementEnabled(bool flag){
	measureEnabledFlag = flag;
}

bool isMeasurementEnabled(void){
	return measureEnabledFlag;
}
