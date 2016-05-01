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
#include "SDCard.h"

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

uint16_t count_or;

bool newDataAvailableFlag = FALSE;
bool dataOverrunFlag = FALSE;
bool measureEnabledFlag = FALSE;

static xSemaphoreHandle  dataQueueSem = NULL;

void saveInMemory(int16_t value);
void accelSensInit(void);
void logAccData(void);

typedef enum {
  SENS_STATE_STARTUP,
  SENS_STATE_READY,
} SensStateType;
static volatile SensStateType sensState = SENS_STATE_STARTUP; /* state machine state */

void ReadAccelSensorTask(void *pvParameters){
	initH3LI();														/* init accelerometer */
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		switch(sensState){
			case SENS_STATE_STARTUP:
				if(isFileSystemMounted()){							/* wait until file system mounted */
					sensState = SENS_STATE_READY;
				}
				break;
			case SENS_STATE_READY:
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
				break;
		}
		FRTOS1_vTaskDelayUntil(&xLastWakeTime, 1/portTICK_RATE_MS);
	}
}

void logAccData(void){
	  z = getAccData();
	  DATAQUEUE_SaveValue(z);										/* save in queue */
	  (void)xSemaphoreGive(dataQueueSem);
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

bool isDataInQueue(void){
	if(FRTOS1_xSemaphoreTake(dataQueueSem, 0)==pdTRUE){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

void ReadSensor_Init(void){
	/* create semaphore to manage sensor data */
	dataQueueSem  = FRTOS1_xSemaphoreCreateCounting(DATAQUEUE_LENGTH, 0);
	if(dataQueueSem == NULL){
		for(;;);													/* error */
	}
	FRTOS1_vQueueAddToRegistry(dataQueueSem, "dataQueueSem");

	/* create sensor reading task */
	if (FRTOS1_xTaskCreate(ReadAccelSensorTask, (signed portCHAR *)"ReadSensor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
		for(;;){} /* error */
	}
}
