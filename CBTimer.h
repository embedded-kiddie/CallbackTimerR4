/*
  CBTimer.h - Callback Timer using FSP Timer for Arduino UNO R4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#ifndef CBTIMER_H
#define CBTIMER_H

#include "FspTimer.h"

#ifndef debug_begin
#define debug_begin(...)
#endif
#ifndef debug_print
#define debug_print(...)
#endif
#ifndef debug_println
#define debug_println(...)
#endif

// To start playing music immediately you should not specify `0` as `duration_ms`.
// You can specify the following symbols as `begin(CBTIMER_START_NOW, ...)`:
#define CBTIMER_START_NOW (1)

// We can not get 32bit timer because they are already used.
// The limit of duration for GPT and AGT 16bit timer:
// - GPT: 1/(48MHz ÷ 1024) × 65535 = 1398.08[msec]
// - AGT: 1/(24MHz ÷ 8   ) × 65535 ≒   21.85[msec]
#define DIVISION_RATIO_GPT  1024
#define DIVISION_RATIO_AGT  8
#define LIMIT_DURATION_GPT  1398
#define LIMIT_DURATION_AGT  21

class CBTimer_t {
private:
  static int channel;
  static int duration_max;
  static volatile int duration_ms;
  static volatile int remain_ms;
  static timer_mode_t timer_mode; // TIMER_MODE_PERIODIC or TIMER_MODE_ONE_SHOT defined in `variants/MINIMA/includes/ra/fsp/inc/api/r_timer_api.h`
  static FspTimer fsp_timer;

  static void (*user_callback)(void);
  static void cbtimer_callback(timer_callback_args_t __attribute((unused)) * p_args);

public:
  bool begin(int duration_ms, void (*callback)(void), bool start = true);
  bool begin(timer_mode_t timer_mode, int duration_ms, void (*callback)(void), bool start = true);

  static void end(void) {
    fsp_timer.end();
  }

  static bool start(void) {
    debug_println("start()");
    if ((duration_ms != 0) && (channel != -1)) {
      return fsp_timer.start();
    } else {
      return false;
    }
  }

  static bool stop(void) {
    debug_println("stop()");
    if ((duration_ms != 0) && (channel != -1)) {
      return fsp_timer.stop();
    } else {
      return false;
    }
  }

  static bool timer_config(timer_mode_t mode, int period_ms, bool start = true) {
    if (fsp_timer.is_opened()) {
      period_ms = min(period_ms, duration_max);
      fsp_timer.set_period_ms(period_ms);

      // show FSP Timer infomation
      debug_print("is_opened = "); debug_println(period_ms);
      debug_print("get_period_raw = "); debug_println(fsp_timer.get_period_raw());
      debug_print("get_counter = "); debug_println(fsp_timer.get_counter());
      debug_print("get_freq_hz = "); debug_println(fsp_timer.get_freq_hz());

      return true;
    }

    else {
      // type is determined by get_available_timer(&type) as GPT_TIMER or AGT_TIMER
      uint8_t type = AGT_TIMER;
      channel = FspTimer::get_available_timer(type);

      debug_print("type    = "); debug_println(type);
      debug_print("channel = "); debug_println(channel);
      debug_print("period  = "); debug_println(period_ms);

      // calculate maximum limit of duration for each type of timer
      uint32_t freq_hz;
      if (type == GPT_TIMER) {
        freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
        duration_max = (int)(65535000.0 * DIVISION_RATIO_GPT / freq_hz); 
      } else {
        freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB);
        duration_max = (int)(65535000.0 * DIVISION_RATIO_AGT / freq_hz); 
      }

      debug_print("freq_hz = "); debug_println(freq_hz);
      debug_print("duration_max = "); debug_println(duration_max);

      // set limit of duration
      period_ms = min(period_ms, duration_max);

      if (channel != -1) {
        fsp_timer.begin(timer_mode, type, channel, 1000.0 / period_ms, 100.0, cbtimer_callback, nullptr);
        fsp_timer.setup_overflow_irq();
        fsp_timer.open();
        if (start) {
          fsp_timer.start();
        }

        // show FSP Timer infomation
        debug_print("get_period_raw = "); debug_println(fsp_timer.get_period_raw());
        debug_print("get_counter = "); debug_println(fsp_timer.get_counter());
        debug_print("get_freq_hz = "); debug_println(fsp_timer.get_freq_hz());

        return true;
      }
    }
    return false;
  }
};

#endif /* CBTIMER_H */
