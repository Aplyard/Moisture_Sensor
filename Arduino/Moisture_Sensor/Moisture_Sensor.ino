#include <TFT_eSPI.h> 
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();
#include <Wire.h>  
#include <TJpg_Decoder.h>
#include "SPIFFS.h"
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define WHITE 0xFFFF

int sensor_pin=12;
int moisture;
int x_moisture;
int moisture_length;
String moisture_percent;
String logo= "/logo.jpg";

void listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

void setup(){
  
  Serial.begin(115200); 
  Serial.println("\n\n Testing TJpg_Decoder library");

  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  listSPIFFS();

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);       // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setCallback(tft_output);  // The decoder must be given the exact name of the rendering function above
  
  pinMode(sensor_pin,INPUT);
  
  tft.init();
  tft.setRotation(4);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  TJpgDec.drawFsJpg(0, 0, logo);

}

void loop(){

  uint16_t x = tft.width()/2;
  uint16_t y = tft.height()/2;
  
  tft.fillRect(0, 150, 135, 90, BLACK);
  
//  for (int i=0; i<=100; i++){
//    moisture = moisture + analogRead(sensor_pin);
//    delay(1);
//  }    
//  moisture = moisture/100;
  moisture = analogRead(sensor_pin);
  Serial.println(moisture);
  moisture = map(moisture ,0 , 2300, 0, 100);
  Serial.print(moisture);
  Serial.println("%");
  moisture_percent = "";
  moisture_percent += moisture;
  moisture_percent += "%";


  moisture_length= moisture_percent.length();
  
  tft.setFreeFont(&FreeSansBold24pt7b);
  
  if(moisture_length == 4){
    tft.setCursor(5,200); 
  }
  else if (moisture_length == 3){
    tft.setCursor(20,200);
  }
  else if (moisture_length == 2){
    tft.setCursor(35,200);
  }
  
  tft.print(moisture_percent);

  delay(2500);
}
