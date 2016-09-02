#include <SPI.h>

const int SPI_CS_PIN = 53;
const byte IODIRA = 0x00;
const byte IODIRB = 0x01;
const byte GPPUA = 0x0C;
const byte IOCON = 0x0A;
const byte IOEXTENDERADDRESS = 0x40;
const byte SPIIOCHIP1 = 0x00;
const byte SPIIOCHIP2 = 0x02;
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

  // Enable hardware address pins
  WriteSPIValue(SPIIOCHIP1, IOCON, 0x08);
  
  // Set IO direction for pin GPA6 & GPA7 to input. All other port A pins to output
  WriteSPIValue(SPIIOCHIP1, IODIRA, 0xC0);

  // Set input polarity for pin GPA6 & GPA7
  WriteSPIValue(SPIIOCHIP1, PORTA_INPUT_POLARITY, 0xC0);
  
  // Set IO direction for all port B pins to output
  WriteSPIValue(SPIIOCHIP1,IODIRB, 0x00);

  // Configure pull up for pin GPA6 & GPA7
  WriteSPIValue(SPIIOCHIP1,GPPUA, 0xC0);

  Serial.begin(9600);
  Serial.println("Ready!");

  score = 1;
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
    if(score < 10 && flip)
    {
      score++;
    }
    if(score > 1 && !flip)
    {
      score--;
    }
    if(score == 10 || score == 1)
    {
      flip = !flip;
    }
    DisplayScore(score);
    /*
    WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, pattern[patternCount]);
    patternCount++;
    if(patternCount == 14)
    {
      patternCount = 0;
    }
    WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, flip ? 0x01 : 0x02);
    flip = !flip;
    */
  }

  delay(delayMs);

  // Read input
  byte switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);

  if(pressTimer == 0)
  {
    if(switchValue & 0x40)
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
    if(switchValue & 0x80)
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
    
    if(switchValue & 0x40 && switchValue & 0x80 && pressTimer == 0)
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
    score = 0;
  }
}

static void DisplayScore(byte score)
{
  int intScore = score == 0x00 ? 0x00 : 0x01 << (score - 1);
  
  Serial.print("Score: ");
  Serial.println(intScore);
  WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, intScore <= 0x00FF ? 0x00 : intScore >> 8);
  WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, intScore <= 0x00FF ? intScore : 0x00);
}


