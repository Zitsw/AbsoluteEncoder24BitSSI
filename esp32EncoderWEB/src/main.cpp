//#include <libraries.h> // Libraries defined
#include <config.h> // all configuration values and libraries
#include <webConfig.h>

File myFile;
RTC_DS3231 rtc;
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 5, /* dc=*/ 13, /* reset=*/ 12);	// Enable U8G2_16BIT in u8g2.h
Encoder enc(encoders2clock,encoders1data,encoderbutton);
GButton ModeButton (BUTTON_PIN, HIGH_PULL, NORM_OPEN); 
GButton ResetButton (resetBUTTON_PIN, HIGH_PULL, NORM_OPEN); 
CRGB leds[1];


void setup() {
  //Web
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  server.on("text/html", handle_encoder); 
  server.begin();


  //setup our pins
  pinMode(progibomerDATA_PIN, INPUT);
  pinMode(progibomerCLOCK_PIN, OUTPUT);
  //encoder setup
  enc.setType(TYPE2);
  attachInterrupt(encoders2clock, isr, CHANGE);
  //button setup
  ModeButton.setTickMode(AUTO);
  ResetButton.setTickMode(AUTO);
  ResetButton.setType(HIGH_PULL);
  //LED setup

  FastLED.addLeds<PL9823, LED_PIN , RGB>(leds, 1);

  //give some default values
  digitalWrite(progibomerCLOCK_PIN, HIGH);
  Serial.begin(115200);
  u8g2.begin();  
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
 // appendFile(SD,fileNameDAT.c_str(), title.c_str());
 
}

void loop() {
  server.handleClient();
  //time reading
  DateTime now = rtc.now();
  enc.tick();
  analogWrite(LED_PIN,200);
 //math 
  unsigned long reading = readPosition();
  // result2 = reading - formula1; 24 bit ?
  //  result3 = result2 * formula2;
  step3 = reading * formula2;
  result = step3 - zerovalue;
 // Serial.println(result3);
  if(result > maxvalue){
    maxvalue = result;
  }
  if(result < minvalue){
    minvalue = result;
  }

  if (ModeButton.isClick()) DynamicMode = !DynamicMode;
  if (ResetButton.isClick()) {
  zerovalue = step3 ;
  maxvalue = 0;
  minvalue = 0;
  }
  //limits
   if(limMode == 0 && enc.isClick()){
    limMode = 1;
  }
    if(limMode == 1){
      if (enc.isRight()) highlim = highlim + 0.01;     	
      if (enc.isLeft()) highlim = highlim - 0.01;	    
      if (enc.isRightH()) highlim = highlim + 0.1; 	
      if (enc.isLeftH()) highlim = highlim - 0.1;
        if(enc.isClick()){
          limMode = 2;
        }
  
    }if(limMode == 2){
      if (enc.isRight()) lowlim = lowlim + 0.01;     	
      if (enc.isLeft()) lowlim = lowlim - 0.01;	    
      if (enc.isRightH()) lowlim = lowlim + 0.1; 	
      if (enc.isLeftH()) lowlim = lowlim - 0.1;
        if(enc.isClick()){
          limMode = 0;
        }
    }
    if(result >= highlim || result <= lowlim ){
      leds[0] = CRGB::Red;
      FastLED.show();
    }else if(result >= highlim-(highlim*0.10) && result < highlim){
      leds[0] = CRGB::Yellow;
      FastLED.show(); 
    }else if(result <= lowlim-(lowlim *0.10) && result > lowlim){
      leds[0] = CRGB::Yellow;
      FastLED.show();    
    }else{
      leds[0] = CRGB::Green;
      FastLED.show();
    }
  
  // if(now.second()<10){
  //   convertSecond = "0" + String(now.second());
  // }else{
  //   convertSecond = String(now.second());
  // }
  // if(now.minute()<10){
  //   convertMinute = "0" + String(now.minute());
  // }else{
  //   convertMinute = String(now.minute());
  // }
  // if (DynamicMode == false){  
  // programDelay = 10000;
  // digitalWrite(LED_PIN, LOW);
  // if(lastHour != now.hour()){
  //   fileNameTimeDAT = String(now.hour()) + String(now.day())  + String(now.month()) + String(convertYear);
  //   lastHour = now.hour();
  //   fileNameDAT = "/DAT/" + fileNameTimeDAT + ".csv";
  //   appendFile(SD, fileNameDAT.c_str(), title.c_str());
  // }
  // String time = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(convertMinute) + ":" + String(convertSecond);
  // dataMessage = time + ";100;03.00;01;V;" + String(result3, 6) + ";" + "\n";
  //   if (millis()- last_time > programDelay){
  //   last_time = millis();
  //   appendFile(SD, fileNameDAT.c_str(), dataMessage.c_str());
  //   }
  // }

  //  if (DynamicMode == true){
  // programDelay = 20;
  // digitalWrite(LED_PIN, HIGH);
  // if(lastMinute != now.minute()){
  //   fileNameTimeDIN = String(now.hour()) + convertMinute  + convertSecond + "01";
  //   fileNameDIN = "/DIN/" + fileNameTimeDIN + ".csv"; 
  //   lastMinute = now.minute();
  //   timeDIN = String(now.day()) + "." + String(now.month()) + "." + String(now.year()) + ";" + String(now.hour()) + ":" + String(convertMinute) + ":" + String(convertSecond); 
  //   messageDIN = timeDIN + ";100;03.00;01;06" + "\n";
  //   appendFile(SD, fileNameDIN.c_str(), title.c_str());
  //   appendFile(SD, fileNameDIN.c_str(), messageDIN.c_str());
  // }
  // dataMessage = String(result3, 6) + ";" + "\n";
  //   if (millis()- last_time > programDelay){
  //   last_time = millis();
  //   appendFile(SD, fileNameDIN.c_str(), dataMessage.c_str());
  //   }
 
  // }

  u8g2.firstPage();
  u8g2.clearBuffer();
  u8g2.setFont( u8g2_font_8x13_mf); 
  u8g2.setCursor(5,10); 
  u8g2.print(highlim);
  if(limMode == 1){
    u8g2.print("<");
  }
  u8g2.setCursor(10,70); 
  u8g2.drawXBMP(0,10,90,40,epd_bitmap_Science_Gauge);
  u8g2.setCursor(5,62);
  u8g2.print(lowlim);
  if(limMode == 2){
    u8g2.print("<");  
  }
  u8g2.setCursor(110,10); 
  if(!DynamicMode){
    u8g2.print("STAT");
  }else{
    u8g2.print("DIN");
  }
  u8g2.setCursor(170,10); 
  u8g2.print(maxvalue);
  u8g2.setCursor(170,64); 
  u8g2.print(minvalue);
  u8g2.setFont( u8g2_font_timB24_tr);         
  u8g2.setCursor(95,45);
  u8g2.print(result);
  u8g2.sendBuffer();


}


//read the current angular position
unsigned long readPosition() {
  unsigned long sample1 = shiftIn(progibomerDATA_PIN, progibomerCLOCK_PIN, BIT_COUNT);
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
//11111
// void displayPrint(unsigned long displayValue){
//   display.clearDisplay();
//   display.setTextColor(WHITE);
//   display.setTextSize(1);
//   display.setCursor(0, 0);
//   display.print("Result:");
//   display.setTextSize(2);
//   display.setCursor(0, 8);
//   display.print(displayValue);
//11111
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


// encoder
void isr() {
  enc.tick(); 
}
