//orange=2
//blue=1

int blueLightPin = 0;  //define a pin for Photo resistor
int orangeLightPin = 1;

byte blueScoreCount = 0;
byte orangeScoreCount = 0;

int blueThreshold = 450;
int orangeThreshold = 900;

#include <SPI.h>

const int LASERTIMEOUT = 1000;
const int BUTTONTIMEOUT = 300;
const int SPI_CS_PIN = 53;
const byte IODIRA = 0x00;
const byte IODIRB = 0x01;
const byte GPPUA = 0x0C;
const byte IOCON = 0x0A;
const byte IOEXTENDERADDRESS = 0x40;
const byte SPIIOCHIPBLUE = 0x00;
const byte SPIIOCHIPORANGE = 0x02;
const byte SPI_ACTION_READ = 0x01;
const byte SPI_ACTION_WRITE = 0x00;
const byte PORTA_OUTPUT_VALUE = 0x14;
const byte PORTB_OUTPUT_VALUE = 0x15;
const byte PORTA_INPUT_VALUE = 0x12;
const byte PORTB_INPUT_VALUE = 0x13;
const byte PORTA_INPUT_POLARITY = 0x02;
const byte PORTB_INPUT_POLARITY = 0x03;

/*
Clock: 52
MOSI: 51
MISO: 50
*/

bool flip = false;

void setup() {
  // put your setup code here, to run once:

  // Set SPI_CS pin to output and default HIGH
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);

  SPI.begin();

  // At the start BLUE and ORANGE listen to every command!

  // Set IO direction for pin GPA6 & GPA7 to input. All other port A pins to output
  WriteSPIValue(SPIIOCHIPBLUE, IODIRA, 0xC0);

  // Set input polarity for pin GPA6 & GPA7
  WriteSPIValue(SPIIOCHIPBLUE, PORTA_INPUT_POLARITY, 0xC0);
  
  // Set IO direction for all port B pins to output
  WriteSPIValue(SPIIOCHIPBLUE,IODIRB, 0x00);

  // Configure pull up for pin GPA6 & GPA7
  WriteSPIValue(SPIIOCHIPBLUE,GPPUA, 0xC0);

  // Enable hardware address pins: after this blue and orange are different!
  WriteSPIValue(SPIIOCHIPBLUE, IOCON, 0x08);

  Serial.begin(9600);
  Serial.println("Ready!");
}

void WriteSPIValue(byte chip, byte port, byte value) {
  digitalWrite(SPI_CS_PIN, LOW);
  SPI.transfer(IOEXTENDERADDRESS + chip + SPI_ACTION_WRITE);
  SPI.transfer(port);  
  SPI.transfer(value);
  digitalWrite(SPI_CS_PIN, HIGH);
}

byte ReadSPIValue(byte chip, byte port) {
  digitalWrite(SPI_CS_PIN, LOW);
  SPI.transfer(IOEXTENDERADDRESS + chip + SPI_ACTION_READ);  
  SPI.transfer(port);
  byte result = SPI.transfer(0x00);
  digitalWrite(SPI_CS_PIN, HIGH);
  return result;
}


void tafelCheck(){
  if(analogRead(blueLightPin) < blueThreshold){
    if(blueScoreCount < 10) {
      blueScoreCount++;
      DisplayScore(SPIIOCHIPBLUE, blueScoreCount);
    }
    delay(LASERTIMEOUT);
  }
  if(analogRead(orangeLightPin) < orangeThreshold){
    if(orangeScoreCount < 10) {
      orangeScoreCount++;
      DisplayScore(SPIIOCHIPORANGE, orangeScoreCount);
    }
    delay(LASERTIMEOUT);
  }
}

static void DisplayScore(byte chip, byte score)
{
  int intScore = 0;

  for(int i = 0; i <= score; i++) {
    intScore += score == 0x00 ? 0x00 : 0x01 << (i - 1);
  }

  WriteSPIValue(chip, PORTA_OUTPUT_VALUE, intScore <= 0x00FF ? 0x00 : intScore >> 8);
  WriteSPIValue(chip, PORTB_OUTPUT_VALUE, intScore);  
}

void loop()
{
  tafelCheck();

  byte switchValue = ReadSPIValue(SPIIOCHIPORANGE, PORTA_INPUT_VALUE);
  
  if(switchValue & 0x40)
  {
    if(orangeScoreCount < 10) {
      orangeScoreCount++;
      DisplayScore(SPIIOCHIPORANGE, orangeScoreCount);
    }
    delay(BUTTONTIMEOUT);
  }
   
  if(switchValue & 0x80)
  {
    if(orangeScoreCount > 0) {
      orangeScoreCount--;
      DisplayScore(SPIIOCHIPORANGE, orangeScoreCount);
    }
    delay(BUTTONTIMEOUT);
  }  

  switchValue = ReadSPIValue(SPIIOCHIPBLUE, PORTA_INPUT_VALUE);
  
  if(switchValue & 0x40)
  {
    if(blueScoreCount < 10) {
      blueScoreCount++;
      DisplayScore(SPIIOCHIPBLUE, blueScoreCount);
    }
    delay(BUTTONTIMEOUT);
  }
   
  if(switchValue & 0x80)
  {
    if(blueScoreCount > 0) {
      blueScoreCount--;
      DisplayScore(SPIIOCHIPBLUE, blueScoreCount);
    }
    delay(BUTTONTIMEOUT);
  }  
}
