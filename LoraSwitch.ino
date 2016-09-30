#include <math.h>
#define MODE_OTAA

#define SerialDebug Serial
#define SerialLORA Serial1

const unsigned long ULDefaultDelay = 130600; // <=> 1% duty cycle, SF12, 5 bytes
const unsigned long ULDelayLoop = ul_default_delay; // ERC7003 868 MHz 1% duty cycle   
const unsigned long xbeeSerialDelay = 500;

// ************** const temp / light ***********************
const int PIN_LIGHTSENSOR = A5;   // Grove - Temperature Sensor connect to A5
const int LIGHT_THRESHOLD_VALUE = 10;    //The threshold for which the LED should turn on.
const int LED_PCB_PIN = 9;         //Connect the PCB LED L0 to Pin13, Digital 13
const int LED_EXT_PIN = 8;         //Connect the external Green LED to Pin 12, Digital 12


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
