#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "wiring_private.h"
TwoWire myWire(&sercom2, 22, 38);

#define Serial SerialUSB
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

Adafruit_SSD1306 display(128,32, &myWire,-1);

void setup() {
  Serial.begin(115200);
  pinPeripheral(22, PIO_SERCOM);
  pinPeripheral(38, PIO_SERCOM);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false,false); 
  display.clearDisplay();
  
  //Display Text
  //Firmware Optimization Done
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Welcome!");
  display.display(); 
}

void loop() {
  Serial.println(".");
}
