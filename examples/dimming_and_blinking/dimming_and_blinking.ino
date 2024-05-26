/*
  Example of CallbackTimerR4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "CBTimer.h"

void callback_func(void) {
  static bool flag;

#if defined(ARDUINO_MINIMA)

  if (flag = !flag) {
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX,  LOW);
  } else {
    digitalWrite(LED_TX,  LOW);
    digitalWrite(LED_RX, HIGH);
  }

#elif defined(ARDUINO_UNOWIFIR4)

  // P109 Port Output Data
  if (flag = !flag) {
    R_BSP_PinWrite(BSP_IO_PORT_01_PIN_09, BSP_IO_LEVEL_HIGH);
  } else {
    R_BSP_PinWrite(BSP_IO_PORT_01_PIN_09, BSP_IO_LEVEL_LOW);
  }

#endif
}

void setup() {
  // put your setup code here, to run once:
#if defined(ARDUINO_MINIMA)

  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);

#elif defined(ARDUINO_UNOWIFIR4)

  // setup P109 functions as GPIO output pin (PDR:1, PMR:0) for TX LED
  R_BSP_PinCfg (BSP_IO_PORT_01_PIN_09, BSP_IO_DIRECTION_OUTPUT);

#endif

  static CBTimer timer;
  timer.begin(350 /* msec cycle */, callback_func);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 256; i++) {
    analogWrite(LED_BUILTIN, i);
    delay(2);
  }

  for (int i = 255; i >= 0; i--) {
    analogWrite(LED_BUILTIN, i);
    delay(2);
  }

  delay(100);
}
