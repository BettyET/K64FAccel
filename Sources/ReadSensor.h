/*
 * ReadSensor.h
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#ifndef SOURCES_READSENSOR_H_
#define SOURCES_READSENSOR_H_


void ReadAccelSensorTask(void *pvParameters);

extern bool measureEnabledFlag;
void setMeasurementEnabled(bool flag);
bool isMeasurementEnabled(void);


#endif /* SOURCES_READSENSOR_H_ */
