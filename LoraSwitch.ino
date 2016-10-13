#include <math.h>
#define MODE_OTAA

#define SerialDebug Serial
#define SerialLORA Serial1

const unsigned long ULDelayLoop = 180000; // ERC7003 868 MHz 1% duty cycle   SF12 5bytes
const unsigned long xbeeSerialDelay = 500;

const int LED_PCB_PIN = 9;         //Connect the PCB LED L0 to Pin13, Digital 13
const int LED_EXT_PIN = 8;         //Connect the external Green LED to Pin 12, Digital 12

const int PIN_POWERLINE = 0;

unsigned long elapsedTimeSinceLastSending = -ULDelayLoop;

void setup() {
  SerialDebug.println("**************** LoraSwitch ***********************.\n ");

  // *********** init Debug and Bee baud rate ***********************************
  SerialDebug.begin(19200);      // the debug baud rate on Hardware Serial Atmega
  SerialLORA.begin(19200);       // the Bee baud rate on Software Serial Atmega

  // *********** init digital pins **********************************************
  pinMode(LED_PCB_PIN, OUTPUT);     //Set the LED on Digital 12 as an OUTPUT
  digitalWrite(LED_PCB_PIN, LOW);  //init
  pinMode(LED_EXT_PIN, OUTPUT);     //Set the External Led on Digital 13 as an OUTPUT
  digitalWrite(LED_EXT_PIN, LOW);  //init

  SerialDebug.println("- Check LED Blinking on Stalker...\n");
  led_blinking(2);  // blinking for ten seconds

  // set ATO & ATM Parameters for Nano N8
  initXbeeNanoN8();

  SerialDebug.println("\n*************** Sensor Light Value & Led Activity on Stalker: ************\n");

  delay(1000);

}

void loop() {
  /****************Up Link data ****************************************/

  /****** Power status measurement **************************************/
  int powerStatus = analogRead(PIN_POWERLINE); //read light from sensor
  byte powerValue = digitizePowerValue(powerStatus);
  visualCheckWithLed(LED_PCB_PIN, powerValue == 0);
  /***************send light ***************/
  unsigned long millisecondsElapsed = millis();
  if (millisecondsElapsed  >= elapsedTimeSinceLastSending + ULDelayLoop) {
    sendPowerLineSensorValuetoNanoN8(powerStatus);
    elapsedTimeSinceLastSending = millisecondsElapsed;
  }
  delay(1000);

  /********* Down Link get serial data  *****************************/
  /*unsigned long l_milli = millis(); //save current time for timeout
  while (!SerialLORA.available()) //wait for valid data
    if ((millis() - l_milli) > 2100) break; //timeout at 2.1s (class A max air time is 2.1sec)*/

  /*if(SerialLORA.available()) {
    // read the down link serial Data:
    by_CdeLed = SerialLORA.read();

    SerialDebug.print("<-- Cde Led received = ");
    SerialDebug.println(by_CdeLed);

    while(SerialLORA.read() > -1); //empty XBee buffer

    switch (by_CdeLed) {
       case 0:
        digitalWrite(LED_PCB_PIN, LOW);
        break;
       case 1:
        digitalWrite(LED_PCB_PIN, HIGH);
        break;
       case 2:
        led_blinking(30);  // both leds blink for 30 sec
        break;
    }
    }*/
}

void initXbeeNanoN8() {
  String str_dummy;

  // ************ Command Mode *****************
  // set ATO & ATM Parameters (Unconf frame, port com, encoding, rx payload only, Duty Cycle...)
  SerialLORA.print("+++");          // Enter command mode
  str_dummy = SerialLORA.readString();
  SerialDebug.println("\n- Enter command mode: +++");
  delay(xbeeSerialDelay);

  SerialLORA.print("ATV\n");    // Return module version, DevEUI (LSB first), Stack version.
  str_dummy = SerialLORA.readString();
  SerialDebug.print("\n ATIM Module version & information:");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

  /***************** DevEUI ****************************************/
  SerialLORA.print("ATO070\n");    // DevEUI (4 Bytes all at once)
  str_dummy = SerialLORA.readString();
  SerialDebug.print(" get DevEUI (LSB F) ATO070: ");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

  /***************** AppEUI ****************************************/
  SerialLORA.print("ATO071\n");    // AppEUI (4 Bytes all at once)
  str_dummy = SerialLORA.readString();
  SerialDebug.print(" get AppEUI (LSB F) ATO071: ");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

  /***************** Serial Rx ****************************************/
  SerialLORA.print("ATM007=06\n");    // Baud rate 19200
  str_dummy = SerialLORA.readString();
  SerialDebug.print(" --> Xbee Serial rate 19200 ATM007=06: ");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

  // /***************** JoinMode ****************************************/
  SerialLORA.print("ATO083\n"); //get actual join mode
  str_dummy = SerialLORA.readString();
  delay(xbeeSerialDelay);

#ifdef MODE_OTAA

  SerialDebug.print(" --> OTAA ");
  SerialDebug.println(str_dummy);

  /***************** OTAA ****************************************/
  if (str_dummy[7] != '3' || str_dummy[8] != 'F') { //avoid to restart if already in the right mode
    SerialLORA.print("ATO083=3F\n");    // OTAA
    str_dummy = SerialLORA.readString();
    SerialDebug.println(" set to OTAA mode: ");
    SerialDebug.println(str_dummy);
    delay(xbeeSerialDelay);

    SerialDebug.println(" Save new configuration");
    SerialLORA.print("ATOS");    // save param to EEPROM
    while (SerialLORA.read() > -1); //empty buffer
    delay(xbeeSerialDelay);

    SerialDebug.println(" Restart the module");
    SerialLORA.print("ATR\n");    // restart the module
    delay(1500);
  }

  /***************** AppKey ****************************************/
  SerialLORA.print("ATO072\n");    // AppKey (16 Bytes all at once)
  str_dummy = SerialLORA.readString();
  SerialDebug.print(" get AppKey (LSB F) ATO072: ");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

#else
  SerialDebug.print(" --> ABP ");
  SerialDebug.println(str_dummy);

  /***************** ABP ****************************************/
  if (str_dummy[7] != '3' || str_dummy[8] != 'E') { //avoid to restart if already in the right mode
    SerialLORA.print("ATO083=3E\n");    // ABP
    str_dummy = SerialLORA.readString();
    SerialDebug.println(" set to OTAA mode: ");
    SerialDebug.println(str_dummy);
    delay(xbeeSerialDelay);

    SerialDebug.println(" Save new configuration");
    SerialLORA.print("ATOS");    // save param to EEPROM
    while (SerialLORA.read() > -1); //empty buffer
    delay(xbeeSerialDelay);

    SerialDebug.println(" Restart the module");
    SerialLORA.print("ATR\n");    // restart the module
    delay(1500);
  }

  /***************** DevAddr ****************************************/
  SerialLORA.print("ATO069\n");    // AppSKey (16 Bytes all at once)
  str_dummy = SerialLORA.readString();
  SerialDebug.print(" get DevAddr (LSB F) ATO069: ");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

  /***************** AppSKey ****************************************/
  SerialLORA.print("ATO074\n");    // AppSKey (16 Bytes all at once)
  str_dummy = SerialLORA.readString();
  SerialDebug.print(" get AppSKey (LSB F) ATO074: ");
  SerialDebug.println(str_dummy);
  delay(xbeeSerialDelay);

#endif


  //  /***************** ATF ****************************************/
  //  SerialLORA.print("ATOS\n");    // ATO configuration saving
  //  str_dummy = SerialLORA.readString();
  //  SerialDebug.print(" return to factory config ");
  //  SerialDebug.println(str_dummy);
  //  delay(500);
  //
  //  /***************** reboot ****************************************/
  //  SerialLORA.print("ATR\n");    // ATR ro reboot module
  //  str_dummy = SerialLORA.readString();
  //  SerialDebug.print(" reboot module ");
  //  SerialDebug.println(str_dummy);
  //  delay(500);

  /***********************Quit COMMAND MODE ********************/
  SerialLORA.print("ATQ\n");        // Quit command mode
  str_dummy = SerialLORA.readString();
  SerialDebug.println("\n- Quit command mode: ATQ");
  delay(500);
}

void led_blinking(int sec)
{
  SerialDebug.print(" --> led blinking... ");
  SerialDebug.print(sec);
  SerialDebug.print("sec ");

  int i_cmp = 0;
  while (i_cmp++ < sec)
  {
    // blink
    digitalWrite(LED_PCB_PIN, HIGH);
    delay(450); // wait for a second

    digitalWrite(LED_PCB_PIN, LOW);
    delay(450); // wait for a second
  }

  SerialDebug.println("...end blinking");
}

void sendPowerLineSensorValuetoNanoN8(int l_value)
{
  SerialDebug.print("--> Send frame:  ");
  byte powerValue = digitizePowerValue(l_value);
  SerialLORA.write(powerValue);
  SerialDebug.print(powerValue, HEX);
}

void visualCheckWithLed(int led, boolean predicate) {
  if(predicate) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

byte digitizePowerValue(int powerValue) {
  return powerValue > 512 ? 1 : 0;
}



