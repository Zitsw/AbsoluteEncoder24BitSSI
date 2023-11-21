#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include <SD.h>
#include <RTClib.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <FreeRTOSConfig.h>
//Pins
const int CLOCK_PIN  {25};
const int DATA_PIN  {26};
const int BIT_COUNT  {24};
const int SD_PIN {22};
const int BUTTON_PIN {21};
const int LED_PIN {32};
// Math
const float formula2  {0.0000153547881599881};
const long formula1  {8000000};
unsigned long result2 {};
float result3 {};
//Time
int timing {12};
int Btime {0};
int timer_count[4] = {0, 0, 0, 0};
unsigned long last_time {};
long programDelay {20};
//Button
boolean DynamicMode = false;
unsigned long lastDebounceTime {0};
int buttonState;
long DEBOUNCE_TIME {50};
int lastButtonState = LOW;  // the previous reading from the input pin

///#define OLED_RESET     -1 
//#define SCREEN_ADDRESS 0x3C 
//Adafruit_SSD1306 display(OLED_RESET);
//Initialization
String titleDataMessage;
String dataMessage;
File myFile;
RTC_DS3231 rtc;
void appendFile(fs::FS &fs, const char * path, const char * message);
unsigned long readPosition();
unsigned long shiftIn(const int data_pin, const int clock_pin, const int bit_count);
//void displayPrint(unsigned long displayValue);

void setup() {
  //setup our pins
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);
  Serial.begin(115200);
  Wire.begin();
  
  // SD card init
  if(SD.begin(SD_PIN,SPI,40000000)){
    Serial.print("SD is ready for use");
  }else{
    Serial.print("Check SD card connetion");
  }
  createDir(SD, "/DAT");
  //display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); 
  titleDataMessage = "Время;Дата;ID Регистратора;Версия ПО;Канал;Тип\n10.02.2023;4:58:14;100;03.00;01;06\n";
  appendFile(SD, "/mySCV.txt", titleDataMessage.c_str());
}

void loop() {
  //Button Reading
  int ButtonReading = digitalRead(BUTTON_PIN);
  if (ButtonReading != lastButtonState) {
    lastDebounceTime = millis();
  }
    if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
    if (ButtonReading != buttonState) {
      buttonState = ButtonReading;
      if (buttonState == HIGH) {
        DynamicMode = !DynamicMode;
      }
    }
  }
  lastButtonState = ButtonReading;
//math
  unsigned long reading = readPosition();
  result2 = reading - formula1;
  result3 = result2 * formula2;
  // if (millis() - timing >= 1000){
  //   timer_count[0]++; //sec
  //   timing = millis();
  // }
  //   if (timer_count[0] == 60){ //min
  //     timer_count[1]++;
  //     timer_count[0] = 0;
  //   }
  //     if(timer_count[1] == 60){
  //       timer_count[2]++;
  //       timer_count[1] = 0;
  //     }
  //       if(timer_count[2] == 24){
  //         timer_count[3]++;
  //         timer_count[2] = 0;
  //       }
  dataMessage = String(result3, 6) + ";" + "\n";

  if (DynamicMode == false){
    programDelay = 5000;
    digitalWrite(LED_PIN, LOW);
    if (millis()- last_time > programDelay){
    last_time = millis();
    appendFile(SD, "/mySCV.txt", dataMessage.c_str());
    }
  }

  if (DynamicMode == true){
    programDelay = 20;
    digitalWrite(LED_PIN, HIGH);
  }
 // String getTime = String(timer_count[3]) + "-" + String(timer_count[2]) + "-" + String(timer_count[1]) + "-" + String(timer_count[0]);
  
  //dataMessage = String(result3, 6) + ";\n";
  //Check if button was pressed
//  delay(1500);
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

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}





