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

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

uint16_t count_or;

bool newDataAvailableFlag = FALSE;
bool dataOverrunFlag = FALSE;

bool loggingEnabledFlag = TRUE;

void saveInMemory(int16_t value);

#define MEASDUR 10000 												/* measurement duration in ms */

void logAccData(void);

void ReadAccelSensorTask(void *pvParameters){
	FRTOS1_vTaskDelay(10000);										/* time for SDCardTask to open a file */
	initH3LI();														/* init accelerometer */
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if(loggingEnabledFlag){
			isNewDataAvailable(Z_AXIS_DA, &newDataAvailableFlag); 	/* check if new data available */
			if(newDataAvailableFlag == TRUE){
				logAccData();										/* read sensor and save on SD card */
			}
			dataOverrun(Z_AXIS_OR, &dataOverrunFlag);				/* data overrun? */
			if(dataOverrunFlag == TRUE){
				count_or++;											/* count overruns */
			}
			FRTOS1_vTaskDelayUntil(&xLastWakeTime, 10);
		}
	}
}

void logAccData(void){
	  z = getAccData();
	  DATAQUEUE_SaveValue(z);										/* save in queue */
	  if ((z>337) || (z< -337)){									/* greater than 1g? */
		  LED_G_On();
	  }
	  else{
		  LED_G_Off();
	  }
}

