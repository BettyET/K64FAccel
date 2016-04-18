/*
 * SDCard.h
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#ifndef SOURCES_SDCARD_H_
#define SOURCES_SDCARD_H_

#include "FAT1.h"

void startLog(void);
void setLoggingEnabled(bool flag);
bool isLoggingEnabled(void);

static FAT1_FATFS fileSystemObject;
static FIL fp;

void SaveValuesSDTask(void *pvParameters);

#endif /* SOURCES_SDCARD_H_ */
