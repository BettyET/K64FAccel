/**
 * \file
 * \brief Key debouncing implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements debouncing of up to 4 Keys.
 */

#include "Keys.h"
#include "KeyDebounce.h"
#include "Debounce.h"
#include "Trigger.h"
#include "Event.h"


/*!
 * \brief Returns the state of the keys. This directly reflects the value of the port
 * \return Port bits
 */
static DBNC_KeySet KEYDBNC_GetKeys(void) {
  DBNC_KeySet keys = 0;

  if (KEY2_Get())
  {
    keys |= (1<<1);
  }
  if (KEY3_Get())
  {
    keys |= (1<<2);
  }
  return keys;
}

/*!
 * \brief Event called in the event of a long key press detected.
 * \param keys The keyboard port content
 * \param event The event kind
 */
static void KEYDBNC_OnDebounceEvent(DBNC_EventKinds event, DBNC_KeySet keys) {
  switch(event) {
    /* pressed */
    case DBNC_EVENT_PRESSED:
      if (keys==(1<<0)) {
      }
      if (keys==(1<<1)) {
        EVNT_SetEvent(EVENT_BUTTON_2_PRESSED);
      }
      if (keys==(1<<2)) {
        EVNT_SetEvent(EVENT_BUTTON_3_PRESSED);
      }
      break;

    /* long pressed */
    case DBNC_EVENT_LONG_PRESSED:
      if (keys==(1<<0)) {
      }
      if (keys==(1<<1)) {

      }EVNT_SetEvent(EVENT_BUTTON_2_LPRESSED);
     if (keys==(1<<2)) {
        EVNT_SetEvent(EVENT_BUTTON_3_LPRESSED);
      }
     break;

     /* released */
    case DBNC_EVENT_RELEASED:
      if (keys==(1<<0)) {
      }
      if (keys==(1<<1)) {
        EVNT_SetEvent(EVENT_BUTTON_2_RELEASED);
      }
      if (keys==(1<<2)) {
        EVNT_SetEvent(EVENT_BUTTON_3_RELEASED);
      }
      break;
    case DBNC_EVENT_END:
      /*! \todo Implement what you have to do at the end of the debouncing. Need to re-enable interrupts? */
#if PL_CONFIG_HAS_KBI
      KEY_EnableInterrupts();
#endif
      break;
  } /* switch */
}

/*! \brief This struct contains all the data and pointers needed to have
 *         our debounce FSM reentrant and usable for multiple ISR's.
 */
static DBNC_FSMData KEYDBNC_FSMdata = {
  /* callbacks: */
  KEYDBNC_GetKeys, /* returns bit set of pressed keys */
  KEYDBNC_OnDebounceEvent, /* event callback */
  /* data: */
  DBNC_KEY_IDLE, /* initial state machine state, here the state is stored */
  0, /* key scan value */
  0, /* long key count */
  TRG_KEYPRESS, /* trigger to be used */
  (50/TRG_TICKS_MS), /* debounceTicks for 50 ms */
  (1000/TRG_TICKS_MS), /* longKeyTicks for 1 s */
};

void KEYDBNC_Process(void) {
  /** \todo check/call DBNC_Process(&KEYDBNC_FSMdata);
   * But be careful: only if we are not debouncing, and if we have a key press if we are polling.
   * And you will need to disable the keyboard interrupts too!
   */
  if (KEYDBNC_FSMdata.state==DBNC_KEY_IDLE && KEYDBNC_GetKeys()!=0) { /* a key is pressed and we are not debouncing */
  #if PL_CONFIG_HAS_KBI
    KEY_DisableInterrupts(); /* disable interrupts for all keys */
  #endif
    DBNC_Process(&KEYDBNC_FSMdata); /* starts the state machine */
  }
}

void KEYDBNC_Init(void) {
  KEYDBNC_FSMdata.state = DBNC_KEY_IDLE;
}

void KEYDBNC_Deinit(void) {
  /* nothing needed */
}

