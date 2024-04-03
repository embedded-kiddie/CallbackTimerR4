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

  if (n++ % 2) {
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX,  LOW);
  } else {
    digitalWrite(LED_TX,  LOW);
    digitalWrite(LED_RX, HIGH);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);

  CBTimer_t t;
  t.begin(100, callback_func);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;

  while (i < 256) {
    analogWrite(LED_BUILTIN, i++);
    delay(2);
  }

  while (i >= 0) {
    analogWrite(LED_BUILTIN, i--);
    delay(2);
  }
}
