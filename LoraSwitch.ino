#include <math.h>
#define MODE_OTAA

#define SerialDebug Serial
#define SerialLoRa Serial1

const unsigned long ULDelayLoop = 180000; // ERC7003 868 MHz 1% duty cycle   SF12 5bytes
const unsigned long ULAirTimeDelay = 2100; // class A max air time is 2.1sec
const unsigned long LoRaSerialDelay = 500;

const int LED_PCB_PIN = 9;
const int LED_EXT_PIN = 8;

const int PIN_POWERLINE = 0;

unsigned long elapsedTimeSinceLastSending = -ULDelayLoop;

void setup() {
  SerialDebug.println("**************** LoraSwitch ***********************");

  // *********** init Debug and LoRa baud rate ***********************************
  SerialDebug.begin(19200);      // the debug baud rate on Hardware Serial Atmega
  SerialLoRa.begin(19200);       // the LoRa baud rate on Software Serial Atmega

  // *********** init digital pins **********************************************
  pinMode(LED_PCB_PIN, OUTPUT);
  digitalWrite(LED_PCB_PIN, LOW);  //init
  pinMode(LED_EXT_PIN, OUTPUT);
  digitalWrite(LED_EXT_PIN, LOW);  //init

  SerialDebug.println("LoRa Switch Init...\n");
  ledBlinking(3);

  // set ATO & ATM Parameters for Nano N8
  initLoRaModule();

  delay(1000);
  SerialDebug.println("Init Completed...");
}

void loop() {
  String result;

  /**************** Up Link Data ****************************************/
  /****** Power status measurement **************************************/
  byte power = digitizePowerValue(analogRead(PIN_POWERLINE));
  updateLedStatus(LED_PCB_PIN, power == 0);

  /*************** Send Message ? ***************/
  unsigned long millisecondsElapsed = millis();
  if (millisecondsElapsed  >= elapsedTimeSinceLastSending + ULDelayLoop) {
    sendPowerLineValuetoLoRa(power);

    /********* Down Link Get Serial Data *****************************/
    unsigned long millisecondsDownLink = millis(); // Current millis()
    while (!SerialLoRa.available())
      if ((millis() - millisecondsDownLink) > ULAirTimeDelay) break;

    if (SerialLoRa.available()) {
      // Read the Down Link serial Data:
      result = SerialLoRa.readString();
      SerialDebug.print("Command received : ");
      SerialDebug.println(result);
      while (SerialLoRa.read() > -1); // Empty Buffer
    }
    elapsedTimeSinceLastSending = millisecondsElapsed;
  }

  SerialDebug.print("RSSI Reader");
  sendATCommandToLoRa("ATT09\n", result);

  delay(10000);
}

void initLoRaModule() {
  String result;

  // ************ Command Mode *****************
  // set ATO & ATM Parameters (Unconf frame, port com, encoding, rx payload only, Duty Cycle...)
  SerialDebug.print("Enter command mode: +++");
  sendATCommandToLoRa("+++", result);

  SerialDebug.print("LoRa Serial rate 19200 ATM007=06");
  sendATCommandToLoRa("ATM007=06\n", result);

  SerialDebug.print("Debug Mode");
  sendATCommandToLoRa("ATM17=3\n", result);

  SerialDebug.print("Battery Level");
  sendATCommandToLoRa("ATT08\n", result);

  SerialDebug.print("RSSI Reader");
  sendATCommandToLoRa("ATT09\n", result);

  SerialDebug.print("ATIM Module version & information");
  sendATCommandToLoRa("ATV\n", result);

  SerialDebug.print("Get DevAddr (LSB F) ATO069");
  sendATCommandToLoRa("ATO069\n", result);

  SerialDebug.print("Get DevEUI (LSB F) ATO070");
  sendATCommandToLoRa("ATO070\n", result);

  SerialDebug.print("Get AppEUI (LSB F) ATO071");
  sendATCommandToLoRa("ATO071\n", result);

  SerialDebug.print("Get AppKey (LSB F) ATO072");
  sendATCommandToLoRa("ATO072\n", result);

  SerialDebug.print("Get NwkSKey (LSB F) ATO073");
  sendATCommandToLoRa("ATO073\n", result);

  SerialDebug.print("Get AppSKey (LSB F) ATO074");
  sendATCommandToLoRa("ATO074\n", result);

  SerialDebug.print("NETWORK_JOINED");
  sendATCommandToLoRa("ATO201\n", result);

  /***************** JoinMode ****************************************/
  SerialDebug.print("LoRaWan Behaviouir ATO083");
  sendATCommandToLoRa("ATO083\n", result);

#ifdef MODE_OTAA
  SerialDebug.println(" --> OTAA ");

  /***************** OTAA ****************************************/
  if (result[7] != '3' || result[8] != 'F') { // Avoid Restarting if Already in Right Mode
    SerialDebug.print("Set to OTAA mode");
    sendATCommandToLoRa("ATO083=3F\n", result);

    SerialDebug.println("Save new configuration");
    sendATCommandToLoRa("ATOS", result);
    while (SerialLoRa.read() > -1); // Empty buffer

    SerialDebug.print("Restart the module");
    sendATCommandToLoRa("ATR\n", result);
    delay(3 * LoRaSerialDelay);
  }
#else
  SerialDebug.println(" --> ABP ");

  /***************** ABP ****************************************/
  if (result[7] != '3' || result[8] != 'E') { // Avoid Restarting if Already in Right Mode
    SerialDebug.print("Set to ABP mode");
    sendATCommandToLoRa("ATO083=3E\n", result);

    SerialDebug.println("Save new configuration");
    sendATCommandToLoRa("ATOS", result);
    while (SerialLoRa.read() > -1); // Empty buffer

    SerialDebug.print("Restart the module");
    sendATCommandToLoRa("ATR\n", result);
    delay(3 * LoRaSerialDelay);
  }
#endif

  //  /***************** ATF ****************************************/
  //  SerialLoRa.print("ATOS\n");
  //  dummy = SerialLoRa.readString();
  //  SerialDebug.print(" return to factory config ");
  //  SerialDebug.println(dummy);
  //  delay(500);
  //
  //  /***************** reboot ****************************************/
  //  SerialLoRa.print("ATR\n");
  //  dummy = SerialLoRa.readString();
  //  SerialDebug.print(" reboot module ");
  //  SerialDebug.println(dummy);
  //  delay(500);

  /***********************Quit COMMAND MODE ********************/
  //  SerialDebug.println("Quit command mode: ATQ");
  //  sendATCommandToLoRa("ATQ\n", result);
}

void sendATCommandToLoRa(String command, String &result)
{
  SerialLoRa.print(command);
  result = SerialLoRa.readString();
  SerialDebug.println(result);
  delay(LoRaSerialDelay);
}

void ledBlinking(int seconds)
{
  SerialDebug.print(" --> led blinking... ");
  SerialDebug.print(seconds);
  SerialDebug.print(" sec");

  int counter = 0;
  while (counter++ < seconds)
  {
    // blink
    digitalWrite(LED_PCB_PIN, HIGH);
    delay(350);

    digitalWrite(LED_PCB_PIN, LOW);
    delay(450);
  }

  SerialDebug.println("... end blinking");
}

void sendPowerLineValuetoLoRa(byte value)
{
  String result;
  String string = String(value, HEX);

  SerialDebug.print("--> Send frame: ");
  SerialDebug.println(value, HEX);
  sendATCommandToLoRa("AT$SB=" + string + "\n", result);
}

void updateLedStatus(int led, boolean predicate) {
  if (predicate) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

byte digitizePowerValue(int powerValue) {
  return powerValue > 512 ? 1 : 0;
}



