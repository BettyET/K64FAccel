/*
 * SDCard.h
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#ifndef SOURCES_SDCARD_H_
#define SOURCES_SDCARD_H_

#include "FAT1.h"

typedef enum {
  SD_STATE_STARTUP,
  SD_STATE_IDLE,
  SD_STATE_OPENFILE,
  SD_STATE_BUFFER,
  SD_STATE_SAVE,
  SD_STATE_CLOSEFILE,
} SDStateType;

void startStopMeas(void);

bool isFileSystemMounted(void);

void SDCard_Init(void);

void SaveValuesSDTask(void *pvParameters);

#endif /* SOURCES_SDCARD_H_ */
