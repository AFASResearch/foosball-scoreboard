int leftLightPin = 0;  //define a pin for Photo resistor
int rightLightPin = 1;

int leftCount = 0;
int rightCount = 0;

int leftThreshold = 450;
int rightThreshold = 900;

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

//byte pattern[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02 };
//byte patternCount = 0;
byte score1 = 0;
bool flip = false;

void setup()
{
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

    WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, 0x00);
      WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, 0x00);

  Serial.begin(9600);  //Begin serial communcation
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
  if(analogRead(leftLightPin) < leftThreshold){
    if(score1 < 10){
    Serial.print("left ");
    increaseScore1();
    }
    delay(1000);
  }
  if(analogRead(rightLightPin) < rightThreshold){
    if(score1 > 0){
    Serial.print("right ");
    decreaseScore1();
    }
    delay(1000);
  }
}

void increaseScore1(){
    score1++;
    int byteScore = 0x01 << (score1-1);
    
    Serial.println(byteScore);
    if(score1 < 9)
    { 
      WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, byteScore);
      WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, 0x00);
    }  
    else
    {
      WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, byteScore >> 2);
      WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, 0x00);
    }
    
    Serial.println("Button 1 pressed");
    
    byte switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);
    while(switchValue & 0x01) 
    { switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);}
}

void decreaseScore1(){
    score1--;
    int byteScore = 0x01 << (score1-1);
    
    Serial.println(byteScore);
    if(score1 < 9){
      WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, byteScore);
      WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, 0x00);
    }
    else
    {
      WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, byteScore >> 2);
      WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, 0x00);
    }
    
    Serial.println("Button 2 pressed");
    byte switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);
    while(switchValue & 0x04) 
    { switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);}
}

void loop()
{
  tafelCheck();
  //WriteSPIValue(SPIIOCHIP1, PORTB_OUTPUT_VALUE, pattern[patternCount]);
  byte switchValue = ReadSPIValue(SPIIOCHIP1, PORTA_INPUT_VALUE);
  
  if(switchValue & 0x01)
  {
    if(score1 < 10){
    increaseScore1();
    }
  }
   
  if(switchValue & 0x04)
  {
    if(score1 > 0){
    decreaseScore1();
    }
  }  

  
  //WriteSPIValue(SPIIOCHIP1, PORTA_OUTPUT_VALUE, flip ? 0x80 : 0x40);
  //flip = !flip; 
  delay(100); 
}
