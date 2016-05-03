/*
 * RTOS.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#include "SDCard.h"
#include "ReadSensor.h"
#include "Event.h"
#include "KeyDebounce.h"

void RTOS_Init(void);

void RTOS_Run(void) {
	SDCard_Init();					/* start SDCard task */
	ReadSensor_Init();				/* start sensor reading task */
	RTOS_Init();					/* start main task */
	FRTOS1_vTaskStartScheduler();  	/* does usually not return! */
}

void MainTask(void *pvParameters);
void myEvents(EVNT_Handle event);

void RTOS_Init(void) {
  if (FRTOS1_xTaskCreate(MainTask, (signed portCHAR *)"Main", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
      for(;;){} /* error */
    }
}


void MainTask(void *pvParameters){
	while(1)
	{
		KEYDBNC_Process();
		EVNT_HandleEvent(myEvents);
		FRTOS1_vTaskDelay(50/portTICK_RATE_MS);
	}
}

void myEvents(EVNT_Handle event)
{
	switch(event)
	{
	case EVENT_BUTTON_2_PRESSED:
		if(isLoggingEnabled() && isMeasurementEnabled()){
			setMeasurementEnabled(FALSE);
		}
		else if ((!isLoggingEnabled()) && (!isMeasurementEnabled())){
			startLog();
			setMeasurementEnabled(TRUE);
			setLoggingEnabled(TRUE);
		}
		break;
	case EVENT_BUTTON_2_LPRESSED:
		setSensState(SENS_START_CALIB);
		break;
	case EVENT_BUTTON_3_PRESSED:
		if((getSensState() == SENS_READ_POS_DIR) || (getSensState() == SENS_READ_NEG_DIR)){
			doCalibrationStep();
		}
		break;
	}

}
