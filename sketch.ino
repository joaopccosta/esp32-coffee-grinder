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
const int pregrind_time_in_ms = 200;

int dispense_time_in_ms = 5000;
String state = "Ready";
String sdata = "";

BluetoothSerial SerialBT;
OneButton grinderButton(PIN_INPUT, true); 

void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  pinMode (GRINDER_PIN, OUTPUT);

  grinderButton.attachClick(grinderHandler);
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
         case 'g':
            getCommand();
            break;
         case 's':
            setCommand(sdata);
            break;
          case 'd':
            dispenseCommand();
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
  Serial.println(dispense_time_in_ms);
  SerialBT.print(state);
  SerialBT.print("_");
  SerialBT.println(dispense_time_in_ms);
}

void setCommand(String sdata) {
   if (sdata.length()>1){
     String valStr = sdata.substring(1);
     dispense_time_in_ms = valStr.toInt();
  }
  Serial.print("Set: ");
  Serial.println(dispense_time_in_ms);
}

void dispenseCommand() {
  grindForDuration(dispense_time_in_ms);
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
  delay(pregrind_time_in_ms);
  grindForDuration(dispense_time_in_ms);
}

void longPressGrinderHandler(){
  grind();
}

void longPressGrinderStopHandler(){
  stopGrind();
}