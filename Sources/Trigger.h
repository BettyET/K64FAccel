/**
 * \file
 * \brief Trigger module interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements Trigger module.
 * Triggers are used to callback functions or hooks with a given relative time delay.
 */

#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <stdint.h>
#include "Timer.h"

/*! \brief Triggers which can be used from the application */
typedef enum {
  TRG_KEYPRESS,    /*!< key debounce */
  TRG_NOF_TRIGGERS /*!< Must be last! */
} TRG_TriggerKind;


#define TRG_TICKS_MS  TMR_TICK_MS
  /*!< Defines the period at which TRG_IncTick gets called */

/*! \brief Type for the data pointer used by the callback */
typedef void *TRG_CallBackDataPtr;

/*! \brief Function callback prototype, with having an optional parameter pointer */
typedef void (*TRG_Callback)(TRG_CallBackDataPtr);

/*! \brief Type to hold the trigger ticks */
typedef uint16_t TRG_TriggerTime;

/*!
 * \brief Adds a new trigger
 * \param trigger Trigger to be added
 * \param ticks Trigger time in ticks. The time is relative from the current time.
 * \param callback Callback to be called when the trigger fires
 * \param data Optional pointer to data
 * \return error code, ERR_OK if everything is fine
 */
uint8_t TRG_SetTrigger(TRG_TriggerKind trigger, TRG_TriggerTime ticks, TRG_Callback callback, TRG_CallBackDataPtr data);

/*! \brief Called from interrupt service routine with a period of TRG_TICKS_MS. */
void TRG_IncTick(void);

/*!\brief De-initializes the module. */
void TRG_Deinit(void);

/*!\brief Initializes the module. */
void TRG_Init(void);

#endif /* TRIGGER_H_ */