#include "avr/sleep.h"

void send_rf_data(Payload _payload)
{
  rf12_sleep(RF12_WAKEUP);
  // if ready to send + exit loop if it gets stuck as it seems too
  int i = 0; while (!rf12_canSend() && i<10) {rf12_recvDone(); i++;}
  rf12_sendNow(0, &_payload, sizeof _payload);
  // set the sync mode to 2 if the fuses are still the Arduino default
  // mode 3 (full powerdown) can only be used with 258 CK startup fuses
  rf12_sendWait(0);
  rf12_sleep(RF12_SLEEP);
}

static byte readBatt() {
  byte count = 4;
  int value;
  while (count-- > 0) {
    value = battPort.anaRead();
  }
    return  (byte) map(value,0,1013,0,330);
}

void mySleep(long delayMS) {
  setPrescaler(6); // prescaler 64
  long limit = millis() + delayMS/64;
  while (millis() < limit) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_mode();
  }

  setPrescaler(0); // prescaler off
}

void setPrescaler (uint8_t mode) {
  cli();
  CLKPR = bit(CLKPCE);
  CLKPR = mode;
  sei();
}
