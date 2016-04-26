/*
 * RTOS.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#include "SDCard.h"
#include "ReadSensor.h"

void RTOS_Init(void);

void RTOS_Run(void) {
	RTOS_Init();
	FRTOS1_vTaskStartScheduler();  /* does usually not return! */
}

void RTOS_Init(void) {
  /*! \todo Add tasks here */
#if 1

#endif
  if (FRTOS1_xTaskCreate(SaveValuesSDTask, (signed portCHAR *)"SaveOnSDCard", configMINIMAL_STACK_SIZE+600, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
      for(;;){} /* error */
    }
}
