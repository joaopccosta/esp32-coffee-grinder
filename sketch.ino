// Based off of these two tutorials
// https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/
// https://pastebin.com/xEr5kV5j

#include "BluetoothSerial.h"
#include "OneButton.h"

#define PIN_INPUT 0 // This is the BOOT button on the ESP32 board. Change it to your liking

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

const String MODULE_NAME="Coffee Grinder";
const int LED_BUILTIN = 2;
const int GRINDER_PIN = 33;
const int PREGRIND_TIME_IN_MS = 200;
const char GET_COMMAND = 'g';
const char SET_DISPENSE_SINGLE_TIME_COMMAND = 's';
const char SET_DISPENSE_DOUBLE_TIME_COMMAND = 'x';
const char DISPENSE_SINGLE_COMMAND = 'd';
const char DISPENSE_DOUBLE_COMMAND = 'z';

int dispense_single_time_in_ms = 5000;
int dispense_double_time_in_ms = 10000;
String state = "Ready";
String sdata = "";

BluetoothSerial SerialBT;
OneButton grinderButton(PIN_INPUT, true); 

void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  pinMode (GRINDER_PIN, OUTPUT);

  grinderButton.attachClick(grinderHandler);
  grinderButton.attachDoubleClick(grinderDoubleHandler);
  grinderButton.attachDuringLongPress(longPressGrinderHandler);
  grinderButton.attachLongPressStop(longPressGrinderStopHandler);
  
  Serial.begin(9600);
  SerialBT.begin(MODULE_NAME);
  logMessage(MODULE_NAME+": "+state);
}

void loop() {
  grinderButton.tick();
  if(SerialBT.available()) {
      byte ch = SerialBT.read();
      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
         sdata.trim();

         // Process command in sdata.
         switch( sdata.charAt(0) ) {
         case GET_COMMAND:
            getCommand();
            break;
         case SET_DISPENSE_SINGLE_TIME_COMMAND:
            setCommand("single",sdata);
            break;
         case SET_DISPENSE_DOUBLE_TIME_COMMAND:
            setCommand("double",sdata);
            break;
          case DISPENSE_SINGLE_COMMAND:
            dispenseSingleCommand();
            break;
          case DISPENSE_DOUBLE_COMMAND:
            dispenseDoubleCommand();
            break;
         default: Serial.println(sdata);
         } 
         sdata = ""; // Clear the string ready for the next command.
      }  
    delay(20);
  }
}

void getCommand() {
  Serial.print(state);
  Serial.print("_");
  Serial.print(dispense_single_time_in_ms);
  Serial.print("_");
  Serial.println(dispense_double_time_in_ms);
  SerialBT.print(state);
  SerialBT.print("_");
  SerialBT.print(dispense_single_time_in_ms);
  SerialBT.print("_");
  SerialBT.println(dispense_double_time_in_ms);
}

void setCommand(String type, String sdata) {
   if (sdata.length()>1){
     if (type == "single"){
      String valStr = sdata.substring(1);
      dispense_single_time_in_ms = valStr.toInt();
      Serial.print("Set single: ");
      Serial.println(dispense_single_time_in_ms);  
    }
    else if (type == "double"){
      String valStr = sdata.substring(1);
      dispense_double_time_in_ms = valStr.toInt();
      Serial.print("Set double: ");
      Serial.println(dispense_double_time_in_ms);  
    }
  }
}

void dispenseSingleCommand() {
  grindForDuration(dispense_single_time_in_ms);
}

void dispenseDoubleCommand() {
  logMessage("Double");
  grindForDuration(dispense_double_time_in_ms);
}


void grindForDuration(int duration) {
  grind();
  delay(duration); 
  stopGrind();
}

void grind() {
  state="Dispensing";
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(GRINDER_PIN, HIGH);
  logMessage(state);
}

void logMessage(String message){
  Serial.println(message);
  SerialBT.println(message);
}


void stopGrind() {
  state="Ready";
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(GRINDER_PIN, LOW);
  logMessage(state);
}

void grinderHandler() {
  delay(PREGRIND_TIME_IN_MS);
  grindForDuration(dispense_single_time_in_ms);
}

void grinderDoubleHandler() {
    delay(PREGRIND_TIME_IN_MS);
    grindForDuration(dispense_double_time_in_ms);
}

void longPressGrinderHandler(){
  grind();
}

void longPressGrinderStopHandler(){
  stopGrind();
}
