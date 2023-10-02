#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include <SD.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <FreeRTOSConfig.h>

const int CLOCK_PIN  {25};
const int DATA_PIN  {26};
const int BIT_COUNT  {24};
const int SD_PIN {22};

const float formula2  {0.0000153547881599881};
const long formula1  {8000000};
unsigned long result2 {};
float result3 {};

int timing {12};
int Btime {0};
int timer_count[4] = {0, 0, 0, 0};
unsigned long last_time {};
long programDelay  {5000};

///#define OLED_RESET     -1 
//#define SCREEN_ADDRESS 0x3C 
//Adafruit_SSD1306 display(OLED_RESET);
String dataMessage;
File myFile;
void appendFile(fs::FS &fs, const char * path, const char * message);
unsigned long readPosition();
unsigned long shiftIn(const int data_pin, const int clock_pin, const int bit_count);
//void displayPrint(unsigned long displayValue);

void setup() {
  //setup our pins
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);
  Serial.begin(115200);
  Wire.begin();
  
  // SD card init
  if(SD.begin(SD_PIN)){
    Serial.print("SD is ready for use");
  }else{
    Serial.print("Check SD card connetion");
  }
  //display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); 

}

void loop() {
   unsigned long reading = readPosition();
   result2 = reading - formula1;
   result3 = result2 * formula2;
   if (millis() - timing >= 1000){
    timer_count[0]++; //sec
    Serial.println("Atime in first if before reset: " + String(timing));
    timing = millis();
    Serial.println("Atime in first if after reset: " + String(timing));
    Serial.println("array[0]: in first if " + String(timer_count[0]));
   }
    if (timer_count[0] == 60){ //min
      timer_count[1]++;
      Serial.println("min array : " + String(timer_count[1]));
      timer_count[0] = 0;
      Serial.println("sec array should be 0: " + String(timer_count[0]));
    }
      if(timer_count[1] == 60){
        timer_count[2]++;
        timer_count[1] = 0;
      }
        if(timer_count[2] == 24){
          timer_count[3]++;
          timer_count[2] = 0;
        }
  String getTime = String(timer_count[3]) + "-" + String(timer_count[2]) + "-" + String(timer_count[1]) + "-" + String(timer_count[0]);
  dataMessage = getTime + ";" + String(result3, 6) + "\n";
  appendFile(SD, "/mySCV.txt", dataMessage.c_str());
//  delay(5000);
  // if (millis()- last_time > programDelay){
  //   last_time = millis();
  //   Serial.print("$");
  //   Serial.print(result3, 6);
  //   Serial.println(";");
  //   appendFile(SD, "/mySCV.txt", dataMessage.c_str());
  // }
}


//read the current angular position
unsigned long readPosition() {
  unsigned long sample1 = shiftIn(DATA_PIN, CLOCK_PIN, BIT_COUNT);
  return sample1;
}

//read in a byte of data from the digital input of the board.
unsigned long shiftIn(const int data_pin, const int clock_pin, const int bit_count) {
  unsigned long data = 0;
  for (int i=0; i<bit_count; i++) {
    data <<= 1;
    digitalWrite(clock_pin, LOW);
    delayMicroseconds(9);
    digitalWrite(clock_pin, HIGH);
    delayMicroseconds(9);

    data |= digitalRead(data_pin);
    //Serial.println(data);
  }

  return data;
}

// void displayPrint(unsigned long displayValue){
//   display.clearDisplay();
//   display.setTextColor(WHITE);
//   display.setTextSize(1);
//   display.setCursor(0, 0);
//   display.print("Result:");
//   display.setTextSize(2);
//   display.setCursor(0, 8);
//   display.print(displayValue);

// }
// Append_inf to a file 
void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  file.print(message);
  file.close();
}






