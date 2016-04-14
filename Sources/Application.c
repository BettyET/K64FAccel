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

/* Prototype */
static void Err(void);

int16_t z = 0;
int16_t x = 0;
int16_t y = 0;

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
	  for(;;){
		  z = getAccData();
		  LogToFile(x, y, z);
		  if ((z>337) || (z< -337)){
			  LED_G_On();
		  }
		  else{
			  LED_G_Off();
		  }
		  WAIT1_Waitms(10);
	  }
}

static void Err(void) {
  LED_G_On();				/* Error */
}
