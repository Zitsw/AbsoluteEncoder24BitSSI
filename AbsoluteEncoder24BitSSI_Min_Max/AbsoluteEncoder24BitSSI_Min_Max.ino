const int CLOCK_PIN = 5;
const int DATA_PIN = 6;
const int BIT_COUNT = 24;
 
unsigned long diffb;
unsigned long diffmin = 13000000;
unsigned long diffmax;

void setup() {
  //setup our pins
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);

  Serial.begin(19200);
}


void loop() {
  unsigned long reading = readPosition();

  //min
  //1000
  diffb = reading;
    //1000>0
  if(diffb > diffmax ) {
    diffmax = diffb;
    
  }
  if (diffb<diffmin) {
     diffmin=diffb;
  }
  
  Serial.print("Reading:");
  Serial.print(reading);
  Serial.print("Min:");
  Serial.print(diffmin);
  Serial.print("Max:");
  Serial.println(diffmax);

//  delay(100);
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
    delayMicroseconds(1);
    digitalWrite(clock_pin, HIGH);
    delayMicroseconds(1);

    data |= digitalRead(data_pin);
    //Serial.println(data);
  }

  return data;
}