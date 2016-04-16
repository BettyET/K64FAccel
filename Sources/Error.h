/*
 * Error.h
 *
 *  Created on: 16.04.2016
 *      Author: User
 */

#ifndef SOURCES_ERROR_H_
#define SOURCES_ERROR_H_

/* turn LED on, remain in loop */

static void Err(void) {
  LED_G_On();				/* Error */
  for(;;);
}

#endif /* SOURCES_ERROR_H_ */
