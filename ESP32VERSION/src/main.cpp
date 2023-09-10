const int CLOCK_PIN = 25;
const int DATA_PIN = 26;
const int BIT_COUNT = 24;


const float formula2 = 0.0000153547881599881;
const long formula1 = 8000000;


unsigned long result2;
float result3;


unsigned long last_time;
long displayDelay = 500;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(OLED_RESET);

//read the current angular position
float readPosition() {
  unsigned long sample1 = shiftIn(DATA_PIN, CLOCK_PIN, BIT_COUNT);
  return sample1;
}

//read in a byte of data from the digital input of the board.
float shiftIn(const int data_pin, const int clock_pin, const int bit_count) {
  unsigned long data = 0;
  for (int i=0; i<bit_count; i++) {
    data <<= 1;
    digitalWrite(clock_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(clock_pin, HIGH);
    delayMicroseconds(1);

    data |= digitalRead(data_pin);
    //Serial.println(data);
  }

  return data;
}

void displayPrint(unsigned long displayValue){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Result:");
  display.setTextSize(2);
  display.setCursor(0, 8);
  display.print(displayValue);

}

void setup() {
  //setup our pins
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); 
  Serial.begin(9600);

}


void loop() {
  float reading = readPosition();
 // result2 = reading - formula1;
 // result3 = result2 * formula2;
//  Serial.print("$");
  Serial.println(reading,4);
//  Serial.println(";");
  if (millis()- last_time > displayDelay){
    last_time = millis();
   // displayPrint(result3);
    display.display();
   // delay(500);
  }
}


