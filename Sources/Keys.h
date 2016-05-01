/**
 * \file
 * \brief Key/switch driver interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a generic keyboard driver for up to 4 Keys.
 * It is using macros for maximum flexibility with minimal code overhead.
 */

#ifndef KEYS_H_
#define KEYS_H_

void KEY_Scan(void);

  #include "SW2.h"
  #define KEY2_Get()  (!(SW2_GetVal()))
    /*!< Macro which returns TRUE if key is pressed */

#endif /* KEYS_H_ */
