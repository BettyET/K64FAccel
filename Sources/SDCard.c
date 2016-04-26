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
#include "calibH3LI.h"

bool loggingEnabledFlag = FALSE;
bool keyPressed = FALSE;

static FAT1_FATFS fileSystemObject;
static FIL fp;
#if 0

#endif

void startLog(void){
  /* open file */
  if (FAT1_open(&fp, "./log.txt", FA_OPEN_ALWAYS|FA_WRITE)!=FR_OK) {
	for(;;);
  }
  /* move to the end of the file */
  if (FAT1_lseek(&fp, fp.fsize) != FR_OK || fp.fptr != fp.fsize) {
	  for(;;);
  }
}
void closeFile(void){
	  /* closing file */
	(void)FAT1_close(&fp);
}

void setLoggingEnabled(bool flag){
	loggingEnabledFlag = flag;
}

bool isLoggingEnabled(void){
	return loggingEnabledFlag;
}

void SaveValuesSDTask(void *pvParameters){
	uint8_t write_buf[512];
	static int i=0;
	int16_t z;
	UINT bw;

	/* write data */
	write_buf[0] = '\0';

	/* SD card detection: PTE6 with pull-down! */
	PORT_PDD_SetPinPullSelect(PORTE_BASE_PTR, 6, PORT_PDD_PULL_DOWN);
	PORT_PDD_SetPinPullEnable(PORTE_BASE_PTR, 6, PORT_PDD_PULL_ENABLE);

	if (FAT1_Init()!=ERR_OK) { 											/* initialize FAT driver */
		for(;;);
	}
	if (FAT1_mount(&fileSystemObject, (const TCHAR*)"0", 1) != FR_OK) { /* mount file system */
		for(;;);
	}

	if (FRTOS1_xTaskCreate(ReadAccelSensorTask, (signed portCHAR *)"ReadSensor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
	    for(;;){} /* error */
	  }

	while(1)
	{
		if(keyPressed){
			vTaskDelay(10/portTICK_RATE_MS);			/* debouncing */
			if(EInt1_GetVal()== 0){						/* still pressed? */
					if(isLoggingEnabled() && isMeasurementEnabled){
						setMeasurementEnabled(FALSE);
					}
					else if ((!isLoggingEnabled()) && (!isMeasurementEnabled())){
						setMeasurementEnabled(TRUE);
						setLoggingEnabled(TRUE);
						startLog();
					}
				//calibrateH3LI();
			}
			keyPressed = FALSE;

		}
		if(loggingEnabledFlag){
			/* buffer data */
			while(i<128){
				  if(z=DATAQUEUE_ReadValue()){
					  UTIL1_strcatNum16s(write_buf, sizeof(write_buf), z);
					  UTIL1_strcat(write_buf, sizeof(write_buf), (unsigned char*)"\r\n");
					  i++;
				  }
				  else if(measureEnabledFlag == FALSE){		/* measurement disabled */
					  break;								/* leafe */
				  }
			  }
			/* write data down */
			if (FAT1_write(&fp, write_buf, UTIL1_strlen((char*)write_buf), &bw)!=FR_OK) {
				(void)FAT1_close(&fp);
				for(;;);
			}
			FAT1_sync(&fp);
			if((!DATAQUEUE_NofElements()) && (measureEnabledFlag == FALSE)){
				(void)FAT1_close(&fp);
				loggingEnabledFlag = FALSE;
				LED_G_Off();
			}
			i=0;								/* set counter to 0 */
			write_buf[0] = '\0';				/* reset buffer */
		}
		//vTaskDelay(1/portTICK_RATE_MS);
	}
}
