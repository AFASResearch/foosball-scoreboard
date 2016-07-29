#include <SPI.h>

const int SPI_CS_PIN = 2;
const byte IODIRA = 0x00;
const byte IODIRB = 0x01;
const byte GPPUA = 0x0C;
const byte IOEXTENDERADDRESS = 0x40;
const byte SPIIOCHIP1 = 0x00;
const byte SPIIOCHIP2 = 0x01;
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

byte pattern[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02 };
byte patternCount = 0;
int delayMs = 50;
bool flip = false;
bool kittScanMode = true;
byte score = 0;
byte pressTimer = 0;

void setup() {
  // put your setup code here, to run once:

  // Set SPI_CS pin to output and default HIGH
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);
  
  SPI.begin();

  // Set IO direction for pin GPA0 & GPA2 to input. All other port A pins to output
  WriteSPIValue(SPIIOCHIP1, IODIRA, 0x05);

  // Set input polarity for pin GPA0 & GPA2
  WriteSPIValue(SPIIOCHIP1, PORTA_INPUT_POLARITY, 0x05);
  
  // Set IO direction for all port B pins to output
  WriteSPIValue(SPIIOCHIP1,IODIRB, 0x00);

  // Configure pull up for pin GPA0 & GPA2
  WriteSPIValue(SPIIOCHIP1,GPPUA, 0x05);

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

void loop() {
  // put your main code here, to run repeatedly:
  if(kittScanMode)
  {
    WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, pattern[patternCount]);
    patternCount++;
    if(patternCount == 14)
    {
      patternCount = 0;
    }
    WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, flip ? 0x80 : 0x40);
    flip = !flip;
  }

  delay(delayMs);

  // Read input
  byte switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);

  if(pressTimer == 0)
  {
    if(switchValue & 0x01)
    {
      if(kittScanMode)
      {
        if(delayMs < 10000)
        {
          delayMs += 10;
        }
      }
      else
      {
        if(score < 10)
        {
          score++;
          DisplayScore(score);
        }  
      }
      Serial.println("Switch 1 pressed");
    }
    if(switchValue & 0x04)
    {
      if(kittScanMode)
      {
        if(delayMs > 0)
        {
          delayMs -= 10;
        }
      }
      else
      {
        if(score > 0)
        {
          score--;
          DisplayScore(score);
        }  
      }
      Serial.println("Switch 2 pressed");
    }
    
    if(switchValue & 0x01 && switchValue & 0x04 && pressTimer == 0)
    {
      pressTimer = 80;
    }
  }
  
  if(pressTimer!=0)
  {
    pressTimer--;
  }

  if(pressTimer == 20)
  {
    kittScanMode =! kittScanMode; 
    delayMs = 100;
    
    WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, 0x00);
    WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, 0x00);
  }
}

static void DisplayScore(byte score)
{
  int intScore = score == 0x00 ? 0x00 : 0x01 << (score - 1);
  
  WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, intScore <= 0x00FF ? 0x00 : intScore >> 2);
  WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, intScore <= 0x00FF ? intScore : 0x00);
}


