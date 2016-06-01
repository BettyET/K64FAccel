/*
 * SDCard.c
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#include "WAIT1.h"
#include "FAT1.h"
#include "UTIL1.h"
#include "PORT_PDD.h"
#include "SDCard.h"
#include "LED_G.h"
#include "H3LIS331DL.h"
#include "Events.h"
#include "Error.h"
#include "ReadSensor.h"
#include "Queue.h"


static FAT1_FATFS fileSystemObject;
static FIL fp;

SDStateType sdState = SD_STATE_STARTUP;

static xSemaphoreHandle  startSensorReadingSem = NULL;

static TaskHandle_t xHandlingSDTask;

#define START_STOP_MEAS_BIT	0x01

void SaveValuesSDTask(void *pvParameters){
	uint8_t write_buf[512];
	int16_t z;
	UINT bw;
	BaseType_t xResult;
	uint32_t ulNotifiedValue;

	/* write data */
	write_buf[0] = '\0';

	while(1)
	{
		switch(sdState){
			case SD_STATE_STARTUP:
				/* SD card detection: PTE6 with pull-down! */
				PORT_PDD_SetPinPullSelect(PORTE_BASE_PTR, 6, PORT_PDD_PULL_DOWN);
				PORT_PDD_SetPinPullEnable(PORTE_BASE_PTR, 6, PORT_PDD_PULL_ENABLE);

				if (FAT1_Init()!=ERR_OK) { 											/* initialize FAT driver */
					for(;;);
				}
				if (FAT1_mount(&fileSystemObject, (const TCHAR*)"0", 1) != FR_OK) { /* mount file system */
					for(;;);
				}
				fileSystemReady();													/* file system mounted */
				sdState = SD_STATE_IDLE;
				break;
			case SD_STATE_IDLE:
				xResult = xTaskNotifyWait( pdFALSE,    	 /* Don't clear bits on entry. */
									   START_STOP_MEAS_BIT,  	/* Clear all bits on exit. */
									   &ulNotifiedValue, /* Stores the notified value. */
									   0);				 /* Don't wait */

				  if( xResult == pdPASS )
				  {
					 /* A notification was received.  See which bits were set. */
					 if( ( ulNotifiedValue & START_STOP_MEAS_BIT ) != 0 )
					 {
						 sdState = SD_STATE_OPENFILE;
					 }
				  }
				break;
			case SD_STATE_OPENFILE:
				/* open file */
				if (FAT1_open(&fp, "./log.txt", FA_OPEN_ALWAYS|FA_WRITE)!=FR_OK) {
					for(;;);
				}
				/* move to the end of the file */
				if (FAT1_lseek(&fp, fp.fsize) != FR_OK || fp.fptr != fp.fsize) {
					for(;;);
				}
				sdState = SD_STATE_BUFFER;
			case SD_STATE_BUFFER:
				/* buffer data */
				while((strlen(write_buf)<500) && (DATAQUEUE_NofElements())){
					if(DATAQUEUE_NofElements()) {
						  z = DATAQUEUE_ReadValue();
						  UTIL1_strcatNum16s(write_buf, sizeof(write_buf), z);
						  UTIL1_strcat(write_buf, sizeof(write_buf), (unsigned char*)"\r\n");
					  }
				  }
				sdState = SD_STATE_SAVE;
				break;
			case SD_STATE_SAVE:
				/* write data down */
				if (FAT1_write(&fp, write_buf, UTIL1_strlen((char*)write_buf), &bw)!=FR_OK) {
					(void)FAT1_close(&fp);
					for(;;);
				}
				FAT1_sync(&fp);
				if(!DATAQUEUE_NofElements()){
					xResult = xTaskNotifyWait( pdFALSE,    	 /* Don't clear bits on entry. */
										   START_STOP_MEAS_BIT,  	/* Clear all bits on exit. */
										   &ulNotifiedValue, /* Stores the notified value. */
										   0);				 /* Don't wait */

					  if( xResult == pdPASS )
					  {
						 /* A notification was received.  See which bits were set. */
						 if( ( ulNotifiedValue & START_STOP_MEAS_BIT ) != 0 )
						 {
							 sdState = SD_STATE_CLOSEFILE;
						 }
					  }
					  else{
						sdState = SD_STATE_BUFFER;
					  }
				}
				else{
					sdState = SD_STATE_BUFFER;
				}
				write_buf[0] = '\0';				/* reset buffer */
				break;
			case SD_STATE_CLOSEFILE:
				 /* closing file */
				(void)FAT1_close(&fp);
				LED_G_Off();
				sdState = SD_STATE_IDLE;
		}
		vTaskDelay(1/portTICK_RATE_MS);
	}
}

bool isFileSystemMounted(void){
	return (FRTOS1_xSemaphoreTake(startSensorReadingSem, 0)==pdTRUE);
}

void startStopMeas(void){
	FRTOS1_xTaskNotify(xHandlingSDTask, START_STOP_MEAS_BIT, eSetBits);
}


void SDCard_Init(void){
	/* create semaphore which is given after file system is mounted */
	startSensorReadingSem  = FRTOS1_xSemaphoreCreateBinary();
	if(startSensorReadingSem == NULL){
		for(;;);													/* error */
	}
	FRTOS1_vQueueAddToRegistry(startSensorReadingSem, "startSensorReadingSem");

	/* create task */
	if (FRTOS1_xTaskCreate(SaveValuesSDTask, (signed portCHAR *)"SaveOnSDCard", configMINIMAL_STACK_SIZE+600, NULL, tskIDLE_PRIORITY+1, &xHandlingSDTask) != pdPASS) {
	      for(;;){} /* error */
	}
}
