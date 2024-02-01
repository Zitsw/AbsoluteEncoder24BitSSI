#include <libraries.h>

// That file include all varibles used for encoder program

// Pins
const int CLOCK_PIN  {25}; // RX pin
const int DATA_PIN  {26}; //TX pin
const int BIT_COUNT  {24}; //amount of bits encoder have 
const int SD_PIN {14};  // pin for sd card
const int BUTTON_PIN {33}; // pin for button
const int LED_PIN {32}; // pin for LED

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
String convertMinute {};
String convertSecond{};
int lastHour{};
int lastMinute{};

//Button
boolean DynamicMode;
unsigned long lastDebounceTime {0};
int buttonState;
long DEBOUNCE_TIME {50};
int lastButtonState = LOW;  // the previous reading from the input pin
boolean staticTitle = true;

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

//Functions
void appendFile(fs::FS &fs, const char * path, const char * message);
void createDir(fs::FS &fs, const char * path);
unsigned long readPosition();
unsigned long shiftIn(const int data_pin, const int clock_pin, const int bit_count);
void createDirectories(const char* path);
