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

 void LogToFile(int16_t x, int16_t y, int16_t z) {
  uint8_t write_buf[48];
  UINT bw;

  /* write data */
  write_buf[0] = '\0';

//  UTIL1_strcatNum32u(write_buf, sizeof(write_buf), counter);
//  UTIL1_chcat(write_buf, sizeof(write_buf), '\t');


  UTIL1_strcatNum16s(write_buf, sizeof(write_buf), z);
  UTIL1_strcat(write_buf, sizeof(write_buf), (unsigned char*)"\r\n");

  if (FAT1_write(&fp, write_buf, UTIL1_strlen((char*)write_buf), &bw)!=FR_OK) {
    (void)FAT1_close(&fp);
    Err();
  }
}

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

void stopLog(void){
  /* closing file */
  (void)FAT1_close(&fp);
}

void SaveValuesSDTask(void *pvParameters){

	/* SD card detection: PTE6 with pull-down! */
	PORT_PDD_SetPinPullSelect(PORTE_BASE_PTR, 6, PORT_PDD_PULL_DOWN);
	PORT_PDD_SetPinPullEnable(PORTE_BASE_PTR, 6, PORT_PDD_PULL_ENABLE);

	if (FAT1_Init()!=ERR_OK) { 								/* initialize FAT driver */
	  Err();
	}
	if (FAT1_mount(&fileSystemObject, (const TCHAR*)"0", 1) != FR_OK) { /* mount file system */
	  Err();
	}

	while(1)
	{

	}
}
