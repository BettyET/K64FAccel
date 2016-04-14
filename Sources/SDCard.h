/*
 * SDCard.h
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#ifndef SOURCES_SDCARD_H_
#define SOURCES_SDCARD_H_

#include "FAT1.h"

void LogToFile(int16_t x, int16_t y, int16_t z);

static FAT1_FATFS fileSystemObject;
static FIL fp;



#endif /* SOURCES_SDCARD_H_ */
