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

bool loggingEnabledFlag = FALSE;

void startLog(void){
  /* open file */
  if (FAT1_open(&fp, "./log.txt", FA_OPEN_ALWAYS|FA_WRITE)!=FR_OK) {
	Err();
  }
  /* move to the end of the file */
  if (FAT1_lseek(&fp, fp.fsize) != FR_OK || fp.fptr != fp.fsize) {
	Err();
  }
}

void setLoggingEnabled(bool flag){
  /* closing file */
	loggingEnabledFlag = flag;
}

bool isLoggingEnabled(void){
	return loggingEnabledFlag;
}

void SaveValuesSDTask(void *pvParameters){
	uint8_t write_buf[2048];
	static int i=0;
	int16_t z;
	UINT bw;

	/* write data */
	write_buf[0] = '\0';

	/* SD card detection: PTE6 with pull-down! */
	PORT_PDD_SetPinPullSelect(PORTE_BASE_PTR, 6, PORT_PDD_PULL_DOWN);
	PORT_PDD_SetPinPullEnable(PORTE_BASE_PTR, 6, PORT_PDD_PULL_ENABLE);

	if (FAT1_Init()!=ERR_OK) { 											/* initialize FAT driver */
	  Err();
	}
	if (FAT1_mount(&fileSystemObject, (const TCHAR*)"0", 1) != FR_OK) { /* mount file system */
	  Err();
	}

	while(1)
	{
		if(loggingEnabledFlag){
			/* buffer data */
			while(i<400){
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
				Err();
			}
			if((!DATAQUEUE_NofElements()) && (measureEnabledFlag == FALSE)){
				(void)FAT1_close(&fp);
				loggingEnabledFlag = FALSE;
				LED_G_Off();
			}
			i=0;								/* set counter to 0 */
			write_buf[0] = '\0';				/* reset buffer */
		}
	}
}
