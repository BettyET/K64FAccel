/*
 * ReadSensor.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */
#include "PE_Types.h"
#include "ReadSensor.h"
#include "H3LIS331DL.h"

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

bool newDataAvailableFlag = FALSE;
bool dataOverrunFlag = FALSE;

int16_t count_or = 0;

#define MEASDUR 10000 											/* measurement duration in ms */

void logAccData(void);

void ReadAccelSensorTask(void *pvParameters){
	initH3LI();													/* init accelerometer */
	while(1)
	{
		isNewDataAvailable(Z_AXIS_DA, &newDataAvailableFlag); 	/* check if new data available */
		if(newDataAvailableFlag == TRUE){
			logAccData();										/* read sensor and save on SD card */
		}
		dataOverrun(Z_AXIS_OR, &dataOverrunFlag);				/* data overrun? */
		if(dataOverrunFlag == TRUE){
			count_or++;											/* count overruns */
		}
	}
}

void logAccData(void){
	  z = getAccData();
	  //LogToFile(x, y, z);
	  if ((z>337) || (z< -337)){								/* greater than 1g? */
		  LED_G_On();
	  }
	  else{
		  LED_G_Off();
	  }
}
