/*
  CBTimer.cpp - Callback Timer using FSP Timer for Arduino UNO R4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "Arduino.h"

#if 0
#define debug_begin(...)  { Serial.begin(__VA_ARGS__); while(!Serial); }
#define debug_print(...)    Serial.print(__VA_ARGS__)
#define debug_println(...)  Serial.println(__VA_ARGS__)
#endif

#include "CBTimer.h"

int CBTimer_t::duration_max = LIMIT_DURATION_GPT;
int volatile CBTimer_t::duration_ms = 0;
int volatile CBTimer_t::remain_ms = 0;
uint32_t volatile CBTimer_t::start_ms = 0;
FspTimer CBTimer_t::fsp_timer;

// TIMER_MODE_PERIODIC or TIMER_MODE_ONE_SHOT (variants/MINIMA/includes/ra/fsp/inc/api/r_timer_api.h)
timer_mode_t CBTimer_t::timer_mode = TIMER_MODE_PERIODIC;

void (*CBTimer_t::user_callback)(void);

bool CBTimer_t::begin(timer_mode_t mode, int duration, void (*callback)(), bool start) {
  timer_mode = mode;
  duration_ms = remain_ms = duration;
  CBTimer_t::user_callback = callback;
  return timer_config(timer_mode, duration_ms, start);
}

bool CBTimer_t::begin(int duration, void (*callback)(), bool start) {
  return begin(TIMER_MODE_PERIODIC, duration, callback, start);
}

void CBTimer_t::cbtimer_callback(timer_callback_args_t __attribute__((unused)) * args) {
  debug_println("cbtimer_callback");

  if (duration_ms <= duration_max) {
    // execute user callback immediately
    user_callback();
  }

  else {
    // actual elapsed time
    int t = (int)(millis() - start_ms);
    start_ms += t;

    if (remain_ms > duration_max) {
      // still needs to be split
      timer_config(timer_mode, remain_ms -= t);
    } 

    else {
      if (timer_mode == TIMER_MODE_PERIODIC) {
        // rerun periodic timer before executing user callback
        timer_config(timer_mode, remain_ms = duration_ms);
      }

      // execute user callback
      user_callback();
    }
  }
}
