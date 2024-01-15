#include <libraries.h> // Libraries defined
#include <config.h> // Pins 

File myFile;
RTC_DS3231 rtc;
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
 
  lastMinute =now.minute();
  lastHour = now.hour();
  DynamicMode = false;

  convertYear = now.year() - 2000;
  fileNameTimeDAT = String(now.hour()) + String(now.day())  + String(now.month()) + String(convertYear);
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
  if(now.second()<10){
    convertSecond = "0" + String(now.second());
  }else{
    convertSecond = String(now.second());
  }
  if(now.minute()<10){
    convertMinute = "0" + String(now.minute());
  }else{
    convertMinute = String(now.minute());
  }
  if (DynamicMode == false){  
  programDelay = 10000;
  digitalWrite(LED_PIN, LOW);
  if(lastHour != now.hour()){
    fileNameTimeDAT = String(now.hour()) + String(now.day())  + String(now.month()) + String(convertYear);
    lastHour = now.hour();
    fileNameDAT = "/DAT/" + fileNameTimeDAT + ".csv";
    appendFile(SD, fileNameDAT.c_str(), title.c_str());
  }
  String time = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(convertMinute) + ":" + String(convertSecond);
  dataMessage = time + ";100;03.00;01;V;" + String(result3, 6) + ";" + "\n";
    if (millis()- last_time > programDelay){
    last_time = millis();
    appendFile(SD, fileNameDAT.c_str(), dataMessage.c_str());
    }
  }

  if (DynamicMode == true){
  programDelay = 20;
  digitalWrite(LED_PIN, HIGH);
  if(lastMinute != now.minute()){
    fileNameTimeDIN = String(now.hour()) + convertMinute  + convertSecond + "01";
    fileNameDIN = "/DIN/" + fileNameTimeDIN + ".csv"; 
    lastMinute = now.minute();
    timeDIN = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(convertMinute) + ":" + String(convertSecond); 
    messageDIN = timeDIN + ";100;03.00;01;06" + "\n";
    appendFile(SD, fileNameDIN.c_str(), title.c_str());
    appendFile(SD, fileNameDIN.c_str(), messageDIN.c_str());
  }
  dataMessage = String(result3, 6) + ";" + "\n";
    if (millis()- last_time > programDelay){
    last_time = millis();
    appendFile(SD, fileNameDIN.c_str(), dataMessage.c_str());
    }
 
  }

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
