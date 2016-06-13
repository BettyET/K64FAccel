/*
 * ReadSensor.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */
#include "PE_Types.h"
#include "ReadSensor.h"
#include "H3LIS331DL.h"
#include "Queue.h"
#include "SDCard.h"
#include "Trigger.h"
#include "Buzzer.h"

#define MEMORY_SIZE 10									/* number of calibrating measurements */
static int16_t accelMemory[MEMORY_SIZE]={0}; 			/* memory for raw data */
static int16_t accelPos;								/* corresponds to 1g */
static int16_t accelNeg;								/* corresponds to -1g*/

static int16_t zerGOff = 295;							/* zero g offset, substract from raw value */
static int16_t gain = 173;								/* number of digits corresponding to 1g */

static uint16_t count_or;								/* number of overruns */

bool detected;											/* high if shock event detected */

static SensStateType sensState = SENS_STATE_INITACCEL; 	/* initial state */

static TaskHandle_t xHandlingSensorTask;

#define BTN2_BIT			0x01
#define BTN2_L_BIT  		0x02
#define BTN3_BIT			0x04

#define FILE_SYS_RDY_BIT 	0x08

/* Prototypes */
void saveInMemory(int16_t value);
void accelSensInit(void);
void logAccData(void);

void buzzerBeep(void);
void buzzerClear(TRG_CallBackDataPtr data);

void detectClear(TRG_CallBackDataPtr data);

void blinkRed(void);
void blinkGreen(void);
void blinkBlue(void);

int16_t getAccData(void);


void ReadAccelSensorTask(void *pvParameters){
	int32_t accelSum =0;
	BaseType_t xResult;
	uint32_t ulNotifiedValue;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		switch(sensState){
			case SENS_STATE_INITACCEL:
				initH3LI();														/* init accelerometer */
				sensState = SENS_STATE_STARTUP;
			case SENS_STATE_STARTUP:
				xResult = xTaskNotifyWait( pdFALSE,    	 						/* Don't clear bits on entry. */
									   FILE_SYS_RDY_BIT,  						/* Clear bit on exit. */
									   &ulNotifiedValue, 						/* Stores the notified value. */
									   pdMS_TO_TICKS( 10 ));					/* Wait 10ms */

				  if( xResult == pdPASS )
				  {
					 /* A notification was received.  See which bits were set. */
					 if( ( ulNotifiedValue & FILE_SYS_RDY_BIT ) != 0 )
					 {
						sensState = SENS_STATE_IDLE;
					 }
				  }
				  break;
			case SENS_STATE_IDLE:
			  xResult = xTaskNotifyWait( pdFALSE,    	 						/* Don't clear bits on entry. */
					  	  	  	  	   BTN2_BIT|BTN2_L_BIT,  					/* Clear bits on exit. */
									   &ulNotifiedValue, 						/* Stores the notified value. */
									   0);				 						/* Don't wait */

				  if( xResult == pdPASS )
				  {
					 /* A notification was received.  See which bits were set. */
					 if( ( ulNotifiedValue & BTN2_BIT ) != 0 )
					 {
						sensState = SENS_STATE_MEASURE;
						readInt1Source();										/* clear interrupts */
						WAIT1_Waitus(2);										/* strange sensor communication problem: need to wait */
						LED_G_On();
						startStopMeas();
					 }

					 if( ( ulNotifiedValue & BTN2_L_BIT ) != 0 )
					 {
						sensState = SENS_START_CALIB;
					 }
				  }
				break;
			case SENS_STATE_MEASURE:
				if(isNewDataAvailable(Z_AXIS_DA)){								/* check if new data available */
					WAIT1_Waitus(2);											/* strange sensor communication problem: need to wait */
					int16_t z = getAccData();
					WAIT1_Waitus(2);											/* strange sensor communication problem: need to wait */
					if(readInt1Source()){
						if(!detected){
							detected = TRUE;
							TRG_SetTrigger(TRG_DEL_DETECT, 200, &detectClear, NULL);
							buzzerBeep();
							z=z+10000;											/* shock detected: simply add 10'000 */
						}
					}
					DATAQUEUE_SaveValue(z);										/* save in queue */
				}
				WAIT1_Waitus(2);												/* strange sensor communication problem: need to wait */
				if(dataOverrun(Z_AXIS_OR)){										/* data overrun? */
					count_or++;													/* count overruns */
				}
				WAIT1_Waitus(2);												/* strange sensor communication problem: need to wait */
				xResult = xTaskNotifyWait( pdFALSE,    	 						/* Don't clear bits on entry. */
									   BTN2_BIT,  		 						/* Clear all bits on exit. */
									   &ulNotifiedValue, 						/* Stores the notified value. */
									   0);				 						/* Don't wait */
				  if( xResult == pdPASS )
				  {
					 /* A notification was received.  See which bits were set. */
					 if( ( ulNotifiedValue & BTN2_BIT ) != 0 )
					 {
						sensState = SENS_STATE_IDLE;
						LED_G_Off();
						startStopMeas();
					 }
				  }
				break;
			case SENS_START_CALIB:
				blinkRed();														/* ready to calibrate */
				sensState = SENS_READ_POS_DIR;
				break;
			case SENS_READ_POS_DIR:												/* read acceleration positive direction */
				xResult = xTaskNotifyWait( pdFALSE,    	 						/* Don't clear bits on entry. */
									   BTN3_BIT,  		 						/* Clear bit on exit. */
									   &ulNotifiedValue, 						/* Stores the notified value. */
									   pdMS_TO_TICKS( 60000 )); 				/* Wait 60 s*/
				  if( xResult == pdPASS )
				  {
					 /* A notification was received.  See which bits were set. */
					 if( ( ulNotifiedValue & BTN3_BIT ) != 0 )
					 {
							accelSum = 0;
							WAIT1_WaitOSms(2000);
							blinkGreen();
							for (int i = MEMORY_SIZE; i>0;i--){
								WAIT1_WaitOSms(10);
								accelMemory[i-1] = getRawData();				/* save values*/
								accelSum += (int32_t)accelMemory[i-1];			/* add values */
							}
							accelPos = (uint16_t)(accelSum / MEMORY_SIZE);		/* calculate average */
							LED_G_On();											/* turn the device */
							WAIT1_WaitOSms(2000);
							LED_G_Off();
							sensState = SENS_READ_NEG_DIR;
					 }
				  }
				  else{															/* timeout */
					  blinkRed();
					  sensState = SENS_STATE_IDLE;								/* back to idle */
				  }


				break;
			case SENS_READ_NEG_DIR:												/* read acceleration negative direction */
				xResult = xTaskNotifyWait( pdFALSE,    	 						/* Don't clear bits on entry. */
									   BTN3_BIT,  		 						/* Clear bit on exit. */
									   &ulNotifiedValue, 						/* Stores the notified value. */
									   pdMS_TO_TICKS( 60000 )); 				/* Wait 60 s*/
				  if( xResult == pdPASS )
				  {
					 /* A notification was received.  See which bits were set. */
					 if( ( ulNotifiedValue & BTN3_BIT ) != 0 )
					 {
							accelSum = 0;
							WAIT1_WaitOSms(2000);
							blinkBlue();
							for (int i=MEMORY_SIZE; i>0;i--){
								WAIT1_WaitOSms(10);
								accelMemory[i-1] = getRawData();				/* save values*/
								accelSum += (int32_t)accelMemory[i-1];			/* add values */
							}
							accelNeg = (int16_t)(accelSum / MEMORY_SIZE);		/* calculate average */
							LED_B_On();
							WAIT1_WaitOSms(2000);
							LED_B_Off();										/* end of calibration */
							gain = (accelPos- accelNeg)/2;				 		/* sensitivity */
							zerGOff = accelPos-gain;					 		/* zero g offset */
							sensState = SENS_STATE_IDLE;
					 }
				  }
				  else{															/* timeout */
					  blinkRed();
					  sensState = SENS_STATE_IDLE;								/* back to idle */
				  }
				break;
		}
		FRTOS1_vTaskDelayUntil(&xLastWakeTime, 1/portTICK_RATE_MS);
	}
}

void blinkRed(void){
	for(int i=8; i>0;i--){
		LED_R_Neg();
		WAIT1_WaitOSms(100);
	}
}

void blinkGreen(void){
	for(int i=8; i>0;i--){
		LED_G_Neg();
		WAIT1_WaitOSms(100);
	}
}

void blinkBlue(void){
	for(int i=8; i>0;i--){
		LED_B_Neg();
		WAIT1_WaitOSms(100);
	}
}

void logAccData(void){
	  int16_t z = getAccData();
	  DATAQUEUE_SaveValue(z);													/* save in queue */
	  if ((z>gain) || (z< -gain)){												/* greater than 1g? */
		  LED_G_On();
	  }
	  else{
		  LED_G_Off();
	  }
}

void button2Pressed(void){
	FRTOS1_xTaskNotify(xHandlingSensorTask, BTN2_BIT, eSetBits);
}

void button2LongPressed(void){
	FRTOS1_xTaskNotify(xHandlingSensorTask, BTN2_L_BIT, eSetBits);
}

void button3Pressed(void){
	FRTOS1_xTaskNotify(xHandlingSensorTask, BTN3_BIT, eSetBits);
}

void fileSystemReady(void){
	FRTOS1_xTaskNotify(xHandlingSensorTask, FILE_SYS_RDY_BIT, eSetBits);
}

int16_t getAccData(void){
	int16_t accel = getRawData();
	return (accel-zerGOff);
}

void ReadSensor_Init(void){
	/* create sensor reading task */
	if (FRTOS1_xTaskCreate(ReadAccelSensorTask, (signed portCHAR *)"ReadSensor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &xHandlingSensorTask) != pdPASS) {
		for(;;){} /* error */
	}
}
void buzzerClear(TRG_CallBackDataPtr data){
	Buzzer_ClrVal();
}

void buzzerBeep(void){
	Buzzer_SetVal();
	TRG_SetTrigger(TRG_BUZ_BEEP, 1000, &buzzerClear, NULL);
}

void detectClear(TRG_CallBackDataPtr data){
	detected = FALSE;
}

