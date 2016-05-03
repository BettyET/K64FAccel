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
#include "SDCard.h"

#define MEMORY_SIZE 10				/* number of calibrating measurements */

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

uint16_t count_or;

static int16_t accelMemory[MEMORY_SIZE]={0};

int16_t zerGOff = 295;				/* zero g offset, substract from raw value */
int16_t gain = 173;					/* number of digits corresponding to 1g */

static int16_t accelPos;
static int16_t accelNeg;


bool newDataAvailableFlag = FALSE;
bool dataOverrunFlag = FALSE;
bool measureEnabledFlag = FALSE;

static xSemaphoreHandle dataQueueSem = NULL;
static xSemaphoreHandle startStopSensCalibSem = NULL;

/* Prototypes */
void saveInMemory(int16_t value);
void accelSensInit(void);
void logAccData(void);

void blinkRed(void);
void blinkGreen(void);
void blinkBlue(void);

int16_t getAccData(void);

SensStateType sensState = SENS_STATE_STARTUP; 		/* state machine state */

void ReadAccelSensorTask(void *pvParameters){
	int32_t accelSum =0;
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
			case SENS_START_CALIB:
				blinkRed();													/* ready to calibrate */
				sensState = SENS_READ_POS_DIR;
				break;
			case SENS_READ_POS_DIR:											/* read acceleration positive direction */
				if(FRTOS1_xSemaphoreTake(startStopSensCalibSem, 0)==pdTRUE){
					accelSum = 0;
					WAIT1_WaitOSms(2000);
					blinkGreen();
					for (int i = MEMORY_SIZE; i>0;i--){
						WAIT1_Waitms(10);
						accelMemory[i-1] = getRawData();						/* save values*/
						accelSum += (int32_t)accelMemory[i-1];					/* add values */
					}
					accelPos = (uint16_t)(accelSum / MEMORY_SIZE);				/* calculate average */
					LED_G_On();													/* turn the device */
					WAIT1_WaitOSms(2000);
					LED_G_Off();
					sensState = SENS_READ_NEG_DIR;
				}
				break;
			case SENS_READ_NEG_DIR:											/* read acceleration negative direction */
				if(FRTOS1_xSemaphoreTake(startStopSensCalibSem, 0)==pdTRUE){
					accelSum = 0;
					WAIT1_WaitOSms(2000);
					blinkBlue();
					for (int i=MEMORY_SIZE; i>0;i--){
						WAIT1_Waitms(10);
						accelMemory[i-1] = getRawData();						/* save values*/
						accelSum += (int32_t)accelMemory[i-1];					/* add values */
					}
					accelNeg = (int16_t)(accelSum / MEMORY_SIZE);				/* calculate average */
					LED_B_On();
					WAIT1_WaitOSms(2000);
					LED_B_Off();												/* end of calibration */
					gain = (accelPos- accelNeg)/2;				 				/* sensitivity */
					zerGOff = accelPos-gain;					 				/* zero g offset */
					sensState = SENS_STATE_READY;
				}
				break;
		}
		FRTOS1_vTaskDelayUntil(&xLastWakeTime, 1/portTICK_RATE_MS);
	}
}

void blinkRed(void){
	for(int i=8; i>0;i--){	/* start calibration modus */
		LED_R_Neg();
		WAIT1_WaitOSms(100);
	}
}

void blinkGreen(void){
	for(int i=8; i>0;i--){	/* start calibration modus */
		LED_G_Neg();
		WAIT1_WaitOSms(100);
	}
}

void blinkBlue(void){
	for(int i=8; i>0;i--){	/* start calibration modus */
		LED_B_Neg();
		WAIT1_WaitOSms(100);
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
	return (FRTOS1_xSemaphoreTake(dataQueueSem, 0)==pdTRUE);
}

void setSensState(SensStateType state){
	sensState = state;
}

SensStateType getSensState(void){
	return sensState;
}

void doCalibrationStep(void){
	(void)xSemaphoreGive(startStopSensCalibSem);
}

int16_t getAccData(void){
	int16_t accel = getRawData();
	return (accel-zerGOff);
}

void ReadSensor_Init(void){
	/* create semaphore to manage sensor data */
	dataQueueSem  = FRTOS1_xSemaphoreCreateCounting(DATAQUEUE_LENGTH, 0);
	if(dataQueueSem == NULL){
		for(;;);													/* error */
	}
	FRTOS1_vQueueAddToRegistry(dataQueueSem, "dataQueueSem");

	/* create sempaphore to manage calibration */
	startStopSensCalibSem  = FRTOS1_xSemaphoreCreateBinary();
	if(startStopSensCalibSem == NULL){
		for(;;);													/* error */
	}
	FRTOS1_vQueueAddToRegistry(startStopSensCalibSem, "startStopSensCalibSem");

	/* create sensor reading task */
	if (FRTOS1_xTaskCreate(ReadAccelSensorTask, (signed portCHAR *)"ReadSensor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
		for(;;){} /* error */
	}
}
