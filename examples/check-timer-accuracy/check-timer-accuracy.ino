/*
  Example of CallbackTimerR4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "Arduino.h"
#include "CBTimer.h"

static volatile uint32_t start, end, lap;
static volatile uint32_t elapsed, interval;

void user_callback(void) {
  // Measure elapsed time and interval time
  end = millis();
  elapsed  = end - start;
  interval = end - lap;

  // Blink LED
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

#define TIMER_DURATION  100  // 100, 500, 1000, 1500, 2000, 3000, 4000, ...[msec]

  // Initializing a periodic timer
  CBTimer_t t;
  t.begin(TIMER_MODE_PERIODIC, TIMER_DURATION, user_callback, false);

  // Start the timer
  start = lap = millis();
  t.start();
}

void loop() {
  // Stop in 1 hour + α (over 3600000 [msec])
  if (end && elapsed < 3600200) {
    lap = millis();
    end = 0;

    Serial.println(String(elapsed) + " (" + String(interval) + ")");
  }
}
