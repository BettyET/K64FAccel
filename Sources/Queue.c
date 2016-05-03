/*
 * Queue.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#include "FRTOS1.h"
#include "Error.h"
#include "SDCard.h"
#include "Queue.h"

static xQueueHandle DATAQUEUE_Queue;


#define DATAQUEUE_ITEM_SIZE   sizeof(int16_t)

void DATAQUEUE_SaveValue(const int16_t value) {
	//xQueueSendToBack(SQUEUE_Queue,&str,0)
	  if (FRTOS1_xQueueSendToBack(DATAQUEUE_Queue, &value, 0)!=pdPASS) {
		  for(;;){} /* ups? */
	  }
}

int16_t DATAQUEUE_ReadValue(void) {
  int16_t ptr;
  portBASE_TYPE res;

  res = FRTOS1_xQueueReceive(DATAQUEUE_Queue, &ptr, 0);
  if (res==errQUEUE_EMPTY) {
    return 0;
  } else {
    return ptr;
  }
}

unsigned short DATAQUEUE_NofElements(void) {
  return (unsigned short)FRTOS1_uxQueueMessagesWaiting(DATAQUEUE_Queue);
}

void DATAQUEUE_Deinit(void) {
  FRTOS1_vQueueUnregisterQueue(DATAQUEUE_Queue);
  FRTOS1_vQueueDelete(DATAQUEUE_Queue);
  DATAQUEUE_Queue = NULL;
}

void DATAQUEUE_Init(void) {
  DATAQUEUE_Queue = FRTOS1_xQueueCreate(DATAQUEUE_LENGTH, DATAQUEUE_ITEM_SIZE);
  if (DATAQUEUE_Queue==NULL) {
    for(;;){} /* out of memory? */
  }
  FRTOS1_vQueueAddToRegistry(DATAQUEUE_Queue, "DataQueue");
}


