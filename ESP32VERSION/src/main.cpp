#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include <SD.h>
#include <RTClib.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FreeRTOSConfig.h>
//Pins
const int CLOCK_PIN  {25};
const int DATA_PIN  {26};
const int BIT_COUNT  {24};
const int SD_PIN {14};
const int BUTTON_PIN {33};
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
long programDelay {};
int convertYear {};
int lastHour{};
int lastMinute{};
//Button
boolean DynamicMode = 0;
unsigned long lastDebounceTime {0};
int buttonState;
long DEBOUNCE_TIME {50};
int lastButtonState = LOW;  // the previous reading from the input pin
boolean dynamicTitle = true;
boolean staticTitle = true;
///#define OLED_RESET     -1 
//#define SCREEN_ADDRESS 0x3C 
//Adafruit_SSD1306 display(OLED_RESET);
//Initialization
String dataMessage;
String title;
String messageDIN;
String timeDIN;
//File Name
String fileNameDAT;
String fileNameDIN;
String fileNameTimeDAT;
String fileNameTimeDIN;


File myFile;
RTC_DS3231 rtc;
//Defined functions
void appendFile(fs::FS &fs, const char * path, const char * message);
void createDir(fs::FS &fs, const char * path);
unsigned long readPosition();
unsigned long shiftIn(const int data_pin, const int clock_pin, const int bit_count);
void createDirectories(const char* path);
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
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime now = rtc.now();
 
  // SD card init
  SD.begin(SD_PIN);
  createDir(SD, "/DAT");
  createDir(SD, "/DIN");
  timeDIN = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) ; 
  convertYear = now.year() - 2000;
  fileNameTimeDAT = String(now.hour()) + String(now.day())  + String(now.month()) + String(convertYear);
  messageDIN = timeDIN + ";100;03.00;01;06" + "\n";
  title = "Date;Time;ID;FW version;Channel;Mode;Value; \n";
  fileNameDAT = "/DAT/" + fileNameTimeDAT + ".csv";
  appendFile(SD,fileNameDAT.c_str(), title.c_str());
}

void loop() {
  //time reading
  DateTime now = rtc.now();
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
// //math
   unsigned long reading = readPosition();
   result2 = reading - formula1;
   result3 = result2 * formula2;

//   dataMessage = String(result3, 6) + ";" + "\n"
  int testHOUR = now.hour();
  int testMIN = now.minute();
 // Serial.println(testHOUR);
  Serial.println(fileNameDAT);

  
  
  if (DynamicMode == false){  
  programDelay = 10000;
  digitalWrite(LED_PIN, LOW);
  dynamicTitle = true;
  if(lastHour != now.hour()){
    fileNameTimeDAT = String(now.hour()) + String(now.day())  + String(now.month()) + String(convertYear);
    lastHour = now.hour();
    fileNameDAT = "/DAT/" + fileNameTimeDAT + ".csv";
    appendFile(SD, fileNameDAT.c_str(), title.c_str());
  }
  String time = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) ;
  dataMessage = time + ";100;03.00;01;V" + String(result3, 6) + ";" + "\n";
    if (millis()- last_time > programDelay){
    last_time = millis();
    Serial.println(time);
    appendFile(SD, fileNameDAT.c_str(), dataMessage.c_str());
    }
  }
  
  if (DynamicMode == true){
  programDelay = 20;
  digitalWrite(LED_PIN, HIGH);
    while (dynamicTitle)
    {
    timeDIN = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) ; 
    appendFile(SD, "/DIN.csv", title.c_str());
    appendFile(SD, "/DIN.csv", messageDIN.c_str());
    dynamicTitle = !dynamicTitle;
    }
  dataMessage = String(result3, 6) + ";" + "\n";
    if (millis()- last_time > programDelay){
    last_time = millis();
    appendFile(SD, "/DIN.csv", dataMessage.c_str());
    }
 // String getTime = String(timer_count[3]) + "-" + String(timer_count[2]) + "-" + String(timer_count[1]) + "-" + String(timer_count[0]);
  }
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

void appendFile(fs::FS &fs, const char * path, const char * message){
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
