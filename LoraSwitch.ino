#include <SeeedOLED.h>
#include <Wire.h>
#include <math.h>
#define MODE_OTAA

#define SerialDebug Serial
#define SerialLoRa Serial1

#define ADC_AREF 3.3
#define BATVOLTPIN A6
#define BATVOLT_R1 4.7
#define BATVOLT_R2 10

const unsigned long ULDelayLoop = 180000; // ERC7003 868 MHz 1% duty cycle   SF12 5bytes
const unsigned long ULAirTimeDelay = 2100; // class A max air time is 2.1sec
const unsigned long LoRaSerialDelay = 250;
const unsigned long LCDDelay = 250;

const int LED_PCB_PIN = 9;
const int PIN_POWERLINE = A4;
const int RELAY_EXT_PIN = 4;
const int LCD_LINE_NR = 16;
const int LCD_DISPLAY_MOD = 10;

unsigned long elapsedTimeSinceLastSending = -ULDelayLoop;
String lastLink;
int battery;

void setup() {
  Wire.begin();

  initLCD();
  displayOnLCDX(4, "Please Wait...");

  SerialDebug.println("**************** LoraSwitch ***********************");

  // *********** init Debug and LoRa baud rate ***********************************
  SerialDebug.begin(19200);      // the debug baud rate on Hardware Serial Atmega
  SerialLoRa.begin(19200);       // the LoRa baud rate on Software Serial Atmega

  // *********** init digital pins **********************************************
  pinMode(LED_PCB_PIN, OUTPUT);
  digitalWrite(LED_PCB_PIN, LOW);  //init
  pinMode(RELAY_EXT_PIN, OUTPUT);
  digitalWrite(RELAY_EXT_PIN, HIGH);  //init

  SerialDebug.println("LoRa Switch Init...\n");
  blinkLed(3);

  // set ATO & ATM Parameters for Nano N8
  initLoRaModule();

  delay(1000);
  SerialDebug.println("Init Completed...");
  displayOnLCDX(4, " ");
  digitalWrite(RELAY_EXT_PIN, LOW);  //init
}

void loop() {
  static int lcdLoop = 0;
  String result;
  /****** Power status measurement **************************************/
  byte power = digitizePowerValue(readPowerInput());
  updateLedStatus(LED_PCB_PIN, power == 0);

  /**************** Up Link Data ****************************************/
  /*************** Send Message ? ***************/
  unsigned long millisecondsElapsed = millis();
  if (millisecondsElapsed  >= elapsedTimeSinceLastSending + ULDelayLoop) {

    SerialDebug.println("*** TIME TO SEND DATA... ***");
    sendPowerLineValuetoLoRa(power, result);
    //sendDatatoLoRa(String(power) + String(battery), result);

    /********* Down Link Get Serial Data *****************************/
    unsigned long millisecondsDownLink = millis(); // Current millis()
    while (!SerialLoRa.available())
      if ((millis() - millisecondsDownLink) > ULAirTimeDelay) break;

    if (SerialLoRa.available()) {
      // Read the Down Link serial Data:
      lastLink = SerialLoRa.readString(); lastLink.trim();
      SerialDebug.print("Last Link: ");
      SerialDebug.println(lastLink);
      if (lastLink.length() > 2)
      {
        if (lastLink[0] == 0) {
          SerialDebug.println("Turn Relay OFF");
          digitalWrite(RELAY_EXT_PIN, LOW);
        }
        else
        {
          SerialDebug.println("Turn Relay ON");
          digitalWrite(RELAY_EXT_PIN, HIGH);
        }
      }
      else
      {
        SerialDebug.println("No command received...");
      }
    }
    elapsedTimeSinceLastSending = millisecondsElapsed;
  }

  if (lcdLoop % LCD_DISPLAY_MOD == 0)
  {
    lcdLoop = 0;
    displayOnLCDX(0, "LoRa Monitor");

    SerialDebug.print("RSSI Reader");
    sendATCommandToLoRa("ATT09\n", result);
    result.trim();
    displayOnLCDX(2, "RSSI");
    if (result.length() == 7)
    {
      displayOnLCDXY(2, 5, result);
    }
    else
    {
      SerialDebug.println("*" + result + "*");
      displayOnLCDXY(2, 5, "-");
    }

    SerialDebug.print("NETWORK_JOINED");
    sendATCommandToLoRa("ATO201\n", result);
    result.trim();
    if (result.indexOf("01") >= 0)
    {
      displayOnLCDX(3, "NET_JOINED OK");
    }
    else
    {
      displayOnLCDX(3, "NET_JOINED KO");
    }

    /*    SerialDebug.print("Battery Level");
        sendATCommandToLoRa("ATT08\n", result);*/

    battery = getRealBatteryVoltage() * 1000.0;
    SerialDebug.println(battery);
    displayOnLCDX(4, "BAT " + String(battery) + "mv");

    String status = ((power == 0) ? "OFF" : "ON");
    displayOnLCDX(5, "POW " + status);
    SerialDebug.print("POW ");
    SerialDebug.println(status);

    SerialDebug.print("LAST LINK ");
    SerialDebug.println(lastLink);
    if (lastLink.indexOf("OK") >= 0)
    {
      SerialDebug.println("=> UP OK");
      displayOnLCDXY(5, 8, "UP OK");
    }
    else
    {
      SerialDebug.println("=> UP KO");
      displayOnLCDXY(5, 8, "UP KO");
    }

    if (lastLink.length() > 2)
    {
      if (lastLink[0] == 0) {
        SerialDebug.println("=> DOWN OFF");
        displayOnLCDX(6, "LAST DOWN OFF");
      }
      else
      {
        SerialDebug.println("=> DOWN ON");
        displayOnLCDX(6, "LAST DOWN ON");
      }
    }
    else
    {
      SerialDebug.println("=> NO DOWN DATA");
      displayOnLCDX(6, "NO DOWN DATA");
    }
  }

  unsigned long wait = (elapsedTimeSinceLastSending - millisecondsElapsed + ULDelayLoop) / 1000;
  if (wait <= ULDelayLoop)
  {
    SerialDebug.println("Link in " + String(wait) + "s");
    displayOnLCDXY(7, 0, "Link in " + String(wait) + "s   ");
  }

  lcdLoop++;
  delay(1000);
}

void changeRelayState(bool state) {
  digitalWrite(RELAY_EXT_PIN, state);
}

void initLCD() {
  SeeedOled.init();
  SeeedOled.clearDisplay();
  SeeedOled.setNormalDisplay();
  SeeedOled.setPageMode();
}

void displayOnLCDXY(int X, int Y, String string) {
  char lineBuffer[LCD_LINE_NR + 1];
  string.toCharArray(lineBuffer, sizeof(lineBuffer));
  SeeedOled.setTextXY(X, Y);
  SeeedOled.putString(lineBuffer);
}

void displayOnLCDX(int X, String string) {
  char lineBuffer[LCD_LINE_NR + 1];
  memset(lineBuffer, ' ', sizeof(lineBuffer)); lineBuffer[LCD_LINE_NR] = 0;
  displayOnLCDXY(X, 0, String(lineBuffer));
  delay(LCDDelay);
  displayOnLCDXY(X, 0, string);
}

void initLoRaModule() {
  String result;

  // ************ Command Mode *****************
  // set ATO & ATM Parameters (Unconf frame, port com, encoding, rx payload only, Duty Cycle...)
  SerialDebug.print("Enter command mode: +++");
  sendATCommandToLoRa("+++", result);

  SerialDebug.print("LoRa Serial rate 19200 ATM007=06");
  sendATCommandToLoRa("ATM007=06\n", result);

  SerialDebug.print("Debug Mode ON or OFF");
  sendATCommandToLoRa("ATM17=1\n", result); // 1: DEBUG MODE ON, 3: DEBUG MODE OFF

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

  SerialDebug.print("Frame Size ATO084");
  sendATCommandToLoRa("ATO084=0\n", result);

  /***************** JoinMode ****************************************/
  SerialDebug.print("LoRaWan Behaviour ATO083");
  sendATCommandToLoRa("ATO083\n", result);

#ifdef MODE_OTAA
  SerialDebug.println(" --> OTAA ");

  /***************** OTAA ****************************************/
  if (result[7] != '3' || result[8] != 'F') { // Avoid Restarting if Already in Right Mode
    SerialDebug.print("Set to OTAA mode");
    sendATCommandToLoRa("ATO083=3F\n", result);

    SerialDebug.println("Save new configuration");
    sendATCommandToLoRa("ATOS", result);

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

void sendATCommandToLoRa(String command, String &result) {
  delay(LoRaSerialDelay);
  SerialLoRa.print(command);
  result = SerialLoRa.readString();
  SerialDebug.println(result);
}

void blinkLed(int seconds) {
  SerialDebug.print("Led blinking... ");
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

void sendPowerLineValuetoLoRa(byte value, String &result) {
  String string = String(value, HEX);

  SerialDebug.print("Send Data: ");
  SerialDebug.println(value, HEX);
  sendATCommandToLoRa("AT$SB=" + string + "\n", result);
}

void sendDatatoLoRa(String data, String &result) {
  String hexStr = "";
  for (int i = 0; i < data.length(); i++) {
    hexStr += String(data.charAt(i), HEX);
  }

  SerialDebug.print("Send Data: ");
  SerialDebug.println(hexStr);
  sendATCommandToLoRa("AT$SF=" + hexStr + "\n", result);
}

void updateLedStatus(int led, boolean predicate) {
  if (predicate) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

byte digitizePowerValue(int powerValue) {
  SerialDebug.print("Power Value: ");
  SerialDebug.println(powerValue);
  return powerValue > 512 ? 1 : 0;
}

int readPowerInput() {
  int sum = 0, i;
  battery = getRealBatteryVoltage() * 1000.0; // Measure battery here!
  SerialDebug.print("Battery: ");
  SerialDebug.println(battery);

  for (i = 0; i < 3; i++)
  {
    sum += analogRead(PIN_POWERLINE);
  }
  return sum / i;
}

float getRealBatteryVoltage()
{
  uint16_t batteryVoltage = analogRead(BATVOLTPIN);
  return (ADC_AREF / 1023.0) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * batteryVoltage;
}
