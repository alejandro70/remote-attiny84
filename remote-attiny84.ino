/*
   Remote Control secuence generator V2.0
   Generador de secuencias de control remoto para control de dsispositivos
*/

#include <avr/power.h>
#include <avr/sleep.h>
#include <IRremote.h>

#define RM_AMU178_POWER        0xA81
#define RM_AMU178_SLEEP        0x61
#define RM_AMU178_VOLUME_UP    0x481
#define RM_AMU178_VOLUME_DOWN  0xC81
#define RM_AMU178_MEMORY_1    0x001
#define RM_AMU178_MEMORY_2    0x801
#define RM_AMU178_MEMORY_3    0x401
#define RM_AMU178_MEMORY_4    0xC01
#define RM_AMU178_MEMORY_5    0x201
#define RM_AMU178_MEMORY_6    0xA01

/* ---------- DECLARACION DE VARIABLES ---------- */
const byte ledPin = 1;        // led
const byte interruptPin = 8;  // touch sensor (INT0)
const byte sendPin = 6;       // Infra-Red LED (IRremote -> ATtiny84)
IRsend irsend;

void setup() {
  // All pins as inputs, and LOW
  for (byte i = 0; i <= A5; i++)
  {
    pinMode (i, INPUT);    // changed as per below
    digitalWrite (i, LOW);  //     ditto
  }

  // disable ADC
  ADCSRA = 0;

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);  // turn on brown-out enable select
  MCUCR = bit (BODS);        // this must be done within 4 clock cycles of above

  // configure pins
  pinMode(ledPin, OUTPUT);
  pinMode(sendPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
}

void loop() {
  flashLed();
  TurnOn_AMU178();
  sleepNow();
}

void onInt0(void)
{
  // This will bring us back from sleep.
  
  // detach interrupt to stop it from continuously firing while the interrupt pin is low.
  detachInterrupt(0);
}

void sleepNow()
{
    /* Setup pin as an interrupt and attach handler. */
  attachInterrupt(0, onInt0, LOW);  // ATTiny84: pin 8 => INT0 (0)
  delay(100);
  
  // Choose our preferred sleep mode:
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Set sleep enable (SE) bit:
  sleep_enable();

  // Put the device to sleep:
  digitalWrite(ledPin, LOW);   // turn LED off to indicate sleep
  digitalWrite(sendPin, LOW);   // turn Infre-Red LED off
  sleep_mode();

  // Upon waking up, sketch continues from this point.
  sleep_disable();
}

void TurnOn_AMU178(void) {
  int i;
  Send_AMU178(RM_AMU178_POWER);
  delay(7000);
  for (i = 0; i < 10; i++) {
    Send_AMU178(RM_AMU178_SLEEP);
  }
}

void Send_AMU178(unsigned long key) {
  for (int i = 0; i < 3; i++) {
    irsend.sendSony(key, 12);
    delay(40);
  }
  delay(100);
}

void flashLed() {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
}
