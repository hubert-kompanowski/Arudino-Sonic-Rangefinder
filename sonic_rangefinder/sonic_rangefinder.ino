#define trigPin 2
#define echoPin 3
#define strobePin 7
#define dataPin 5
#define clockPin 6
 
 
 
const int digits[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
const int digitsWithComma[10] = {0xBF, 0x86, 0xDB, 0xcf, 0xe6, 0xED, 0xFD, 0x87, 0xff, 0xEF};
const int idleLine = 0x40;
const int emptyDisplay = 0x00;
 
const int firstDisplay = 0xc0;
const int lastDisplay = 0xce;
const int numberDisplays[4] = {0xc0, 0xc2, 0xc4, 0xc6};
 
const int numberOfMeasurements = 8;
const int durationDivider = 29.1 * 2;
 
const long interval = 1000;
const int trigPinInHighDuration = 10;
 
unsigned int flag = 1;
unsigned long previousMillis = 0;
 
 
 
void sendCommand(uint8_t value)
{
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(strobePin, HIGH);
}
 
void setup()
{
 
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  sendCommand(0x8f);
  reset();
}
 
void idleLines()
{
  for (uint8_t i = firstDisplay; i <= lastDisplay; i+=2)
  {
    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, i);
    shiftOut(dataPin, clockPin, LSBFIRST, idleLine);
    digitalWrite(strobePin, HIGH);
  }
}
 
void empty()
{
  for (uint8_t i = firstDisplay; i <= lastDisplay; i+=2)
  {
    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, i);
    shiftOut(dataPin, clockPin, LSBFIRST, emptyDisplay);
    digitalWrite(strobePin, HIGH);
  }
}
 
void reset()
{
  sendCommand(0x40);
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, firstDisplay);
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOut(dataPin, clockPin, LSBFIRST, emptyDisplay);
  }
  digitalWrite(strobePin, HIGH);
}
 
uint8_t readButtons(void)
{
  uint8_t pressed = 0;
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0x42);
 
  pinMode(dataPin, INPUT);
 
  for (uint8_t i = 0; i < 2; i++)
  {
    uint8_t v = shiftIn(dataPin, clockPin, LSBFIRST) << i;
    if (v) { pressed = i + 1; }
  }
 
  pinMode(dataPin, OUTPUT);
  digitalWrite(strobePin, HIGH);
  return pressed;
}
 
void screenWrite(int distance){
 
  int numberArray[4] = {0};
  int z = 3;
  while (distance!=0){
    numberArray[z] = distance%10;
    distance=distance/10;
    z--;
  }
 
     for (int i=0; i<4; i++){
      digitalWrite(strobePin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, numberDisplays[i]);
      switch(i){
        case 2:
          shiftOut(dataPin, clockPin, LSBFIRST, digitsWithComma[numberArray[i]]);
          break;
        default:
          shiftOut(dataPin, clockPin, LSBFIRST, digits[numberArray[i]]);
      }
      digitalWrite(strobePin, HIGH);
     }
 
}
long mean (long a[], int n)
{
    long sum = 0;
    for (int i = 0; i < n; i++)  
        sum += round(a[i]);
     
    long x = (long)sum/(float)n;
    long y = round(x);
    return y;
   
}
 
 
 
void measurementStart(){
  long duration;
  long distance;
  long measurements[numberOfMeasurements] = {0};
  for (int i=0; i<numberOfMeasurements; i++){
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(trigPinInHighDuration);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance =  (duration/durationDivider) * 10; 
    measurements[i] = distance;
    delay(100);
    for (int i=0; i<numberOfMeasurements; i++){
    }
  }

   
  long result = mean(measurements, numberOfMeasurements);
  if( result <= 1000){
    result = result + 10;
  } else if(result >1000 && result <= 1650){
    result = result + 20;
  }else if(result >1650){
    result = result + 29;
  }
  screenWrite(result);
 
 
}
 
 
void loop()
{
 
  uint8_t buttons = readButtons();
  unsigned long currentMillis = millis();
 
  switch(buttons){
    case 1:
      empty();
      flag = 0;
      measurementStart();
      break;
    case 2:
      flag = 2;
      break;
  }
 
 
  if (currentMillis - previousMillis >= interval && flag != 0 ) {
    previousMillis = currentMillis;
    switch (flag){
      case 1:
        empty();
        flag = 2;
        break;
      case 2:
        idleLines();
        flag = 1;
        break;
    }
  }
 
}
