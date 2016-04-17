/*
 * Application.c
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#include "Application.h"
#include "SDCard.h"
#include "LED_G.h"
#include "PORT_PDD.h"
#include "H3LIS331DL.h"
#include "Error.h"
#include "PE_Types.h"
#include "Events.h"

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

bool newDataAvailableFlag = FALSE;
bool dataOverrunFlag = FALSE;

int16_t count_or = 0;

void APP_Run(void){
	/* SD card detection: PTE6 with pull-down! */
	  PORT_PDD_SetPinPullSelect(PORTE_BASE_PTR, 6, PORT_PDD_PULL_DOWN);
	  PORT_PDD_SetPinPullEnable(PORTE_BASE_PTR, 6, PORT_PDD_PULL_ENABLE);

	  if (FAT1_Init()!=ERR_OK) { /* initialize FAT driver */
	      Err();
	   }
	  if (FAT1_mount(&fileSystemObject, (const TCHAR*)"0", 1) != FR_OK) { /* mount file system */
	      Err();
	  }
	  initH3LI(); 			/* init accelerometer */
	  startLog();			/* start the logger */
	  while(counter<(10000-1)){
		  isNewDataAvailable(Z_AXIS_DA, &newDataAvailableFlag);
		  if(newDataAvailableFlag == TRUE){
			  logAccData();
		  }
		  dataOverrun(Z_AXIS_OR, &dataOverrunFlag);
		  if(dataOverrunFlag == TRUE){
			  //Err();
			  count_or++;
		  }

	  }
	  stopLog();			/* stop the logger */
	  TI2_Disable();		/* disable the counter */
	  LED_G_Off();
}

void logAccData(void){
	  z = getAccData();
	  LogToFile(x, y, z);
	  if ((z>337) || (z< -337)){
		  LED_G_On();
	  }
	  else{
		  LED_G_Off();
	  }
}

