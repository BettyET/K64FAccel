/*
 * Application.c
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#include "Application.h"
#include "PE_Types.h"
#include "RTOS.h"
#include "Queue.h"



void APP_Init(void){
	DATAQUEUE_Init();
	RTOS_Run();
}



