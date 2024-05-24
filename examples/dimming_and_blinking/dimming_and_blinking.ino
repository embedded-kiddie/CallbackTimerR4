/*
  Example of CallbackTimerR4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "Arduino.h"
#include "CBTimer.h"

void callback_func(void) {
  static int n = 0;

#if defined(ARDUINO_MINIMA)

  if (n++ % 2) {
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX,  LOW);
  } else {
    digitalWrite(LED_TX,  LOW);
    digitalWrite(LED_RX, HIGH);
  }

#elif defined(ARDUINO_UNOWIFIR4)

  if (n++ % 2) {
    R_PORT1->PODR_b.PODR9 = 1;
  } else {
    R_PORT1->PODR_b.PODR9 = 0;
  }

#endif
}

void setup() {
  // put your setup code here, to run once:
#if defined(ARDUINO_MINIMA)

  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);

#elif defined(ARDUINO_UNOWIFIR4)

  R_PFS->PORT[1].PIN[9].PmnPFS_b.PMR = 0;
  R_PORT1->PDR_b.PDR9 = 1;

#endif

  static CBTimer timer;
  timer.begin(350, callback_func);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 256; i++) {
    analogWrite(LED_BUILTIN, i);
    delay(3);
  }

  for (int i = 255; i >= 0; i--) {
    analogWrite(LED_BUILTIN, i);
    delay(3);
  }
}
