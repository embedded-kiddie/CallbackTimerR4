/*
  CBTimer.cpp - Callback Timer using FSP Timer for Arduino UNO R4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "Arduino.h"
#include "CBTimer.h"

int CBTimer_t::channel = -1;
int CBTimer_t::duration_max = LIMIT_DURATION_GPT;
int volatile CBTimer_t::duration_ms = 0;
int volatile CBTimer_t::remain_ms = 0;
timer_mode_t CBTimer_t::timer_mode = TIMER_MODE_PERIODIC;  // or TIMER_MODE_ONE_SHOT defined in `variants/MINIMA/includes/ra/fsp/inc/api/r_timer_api.h`
FspTimer CBTimer_t::fsp_timer;

void (*CBTimer_t::user_callback)(void);
bool CBTimer_t::begin(timer_mode_t mode, int duration, void (*callback)(), bool start) {
  CBTimer_t::user_callback = callback;
  timer_mode = mode;
  duration_ms = remain_ms = duration;
  return timer_config(timer_mode, duration_ms, start);
}

bool CBTimer_t::begin(int duration, void (*callback)(), bool start) {
  return begin(TIMER_MODE_PERIODIC, duration, callback, start);
}

void CBTimer_t::cbtimer_callback(timer_callback_args_t __attribute__((unused)) * args) {
  debug_println("cbtimer_callback");
  if (remain_ms > duration_max) {
    remain_ms -= duration_max;
    timer_config(TIMER_MODE_ONE_SHOT, remain_ms);
  } else {
    timer_config(timer_mode, remain_ms = duration_ms);
    user_callback();
  }
}
