//Pin connected to ST_CP of 74HC595
int latchPin = 8; // green
//Pin connected to SH_CP of 74HC595
int clockPin = 12; // yellow
////Pin connected to DS of 74HC595
int dataPin = 11; // blue

//holders for infromation you're going to pass to shifting function
byte dataRED1;
byte dataGREEN1;
byte dataArrayRED1[10];
byte dataArrayGREEN1[10];

byte dataRED2;
byte dataGREEN2;
byte dataArrayRED2[10];
byte dataArrayGREEN2[10];

byte dataRED3;
byte dataGREEN3;
byte dataArrayRED3[10];
byte dataArrayGREEN3[10];

void setup()
{
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  Serial.begin(9600);
  //Arduino doesn't seem to have a way to write binary straight into the code
  //so these values are in HEX.  Decimal would have been fine, too.
  dataArrayRED1[0] = 0xFF; //11111111
  dataArrayRED1[1] = 0xFE; //11111110
  dataArrayRED1[2] = 0xFC; //11111100
  dataArrayRED1[3] = 0xF8; //11111000
  dataArrayRED1[4] = 0xF0; //11110000
  dataArrayRED1[5] = 0xE0; //11100000
  dataArrayRED1[6] = 0xC0; //11000000
  dataArrayRED1[7] = 0x80; //10000000
  dataArrayRED1[8] = 0x00; //00000000
  dataArrayRED1[9] = 0x55; //00000001

  dataArrayRED2[0] = 0xAA; //11100000
  dataArrayRED2[1] = 0x55; //00000000
  dataArrayRED2[2] = 0xAA; //10000000
  dataArrayRED2[3] = 0x55; //11000000
  dataArrayRED2[4] = 0xAA; //11100000
  dataArrayRED2[5] = 0x55; //11110000
  dataArrayRED2[6] = 0xAA; //11111000
  dataArrayRED2[7] = 0x55; //11111100
  dataArrayRED2[8] = 0xAA; //11111110
  dataArrayRED2[9] = 0x55; //11111111

  dataArrayRED3[0] = 0x01; //11111111
  dataArrayRED3[1] = 0x00; //11111110
  dataArrayRED3[2] = 0x01; //11111100
  dataArrayRED3[3] = 0x03; //11111000
  dataArrayRED3[4] = 0x07; //11110000
  dataArrayRED3[5] = 0x0F; //11100000
  dataArrayRED3[6] = 0x1F; //11000000
  dataArrayRED3[7] = 0x3F; //10000000
  dataArrayRED3[8] = 0x7F; //00000000
  dataArrayRED3[9] = 0xAA; //00000000

  //Arduino doesn't seem to have a way to write binary straight into the code
  //so these values are in HEX.  Decimal would have been fine, too.
  dataArrayGREEN1[0] = 0xFF; //11111111
  dataArrayGREEN1[1] = 0x7F; //01111111
  dataArrayGREEN1[2] = 0x3F; //00111111
  dataArrayGREEN1[3] = 0x1F; //00011111
  dataArrayGREEN1[4] = 0x0F; //00001111
  dataArrayGREEN1[5] = 0x07; //00000111
  dataArrayGREEN1[6] = 0x03; //00000011
  dataArrayGREEN1[7] = 0x01; //00000001
  dataArrayGREEN1[8] = 0x00; //00000000
  dataArrayGREEN1[9] = 0x55; //10000000

  dataArrayGREEN2[0] = 0x55; //11111111
  dataArrayGREEN2[1] = 0xAA; //01111111
  dataArrayGREEN2[2] = 0x55; //00111111
  dataArrayGREEN2[3] = 0xAA; //00011111
  dataArrayGREEN2[4] = 0x55; //00001111
  dataArrayGREEN2[5] = 0xAA; //00000111
  dataArrayGREEN2[6] = 0x55; //00000011
  dataArrayGREEN2[7] = 0xAA; //00000001
  dataArrayGREEN2[8] = 0x55; //00000000
  dataArrayGREEN2[9] = 0xAA; //00000111

  dataArrayGREEN3[0] = 0x80; //11111111
  dataArrayGREEN3[1] = 0x00; //11111110
  dataArrayGREEN3[2] = 0x01; //11111100
  dataArrayGREEN3[3] = 0x03; //11111000
  dataArrayGREEN3[4] = 0x07; //11110000
  dataArrayGREEN3[5] = 0x0F; //11100000
  dataArrayGREEN3[6] = 0x1F; //11000000
  dataArrayGREEN3[7] = 0x3F; //10000000
  dataArrayGREEN3[8] = 0x7F; //00000000
  dataArrayGREEN3[9] = 0xAA;
  
  //function that blinks all the LEDs
  //gets passed the number of blinks and the pause time
  blinkAll_2Bytes(2,500); 
  for (int k = 0; k < 10; k++)
  {
    dataRED2 = dataArrayRED2[k];
    dataGREEN2 = dataArrayGREEN2[k];
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, dataGREEN2);
    shiftOut(dataPin, clockPin, dataRED2);
    digitalWrite(latchPin, 1);
    delay(300);
  }
}

void loop()
{
  for (int j = 0; j < 10; j++)
  {
    //load the light sequence you want from array
    dataRED1 = dataArrayRED1[j];
    dataGREEN1 = dataArrayGREEN1[j];
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, 0);
    //move 'em out
    shiftOut(dataPin, clockPin, dataGREEN1);
    shiftOut(dataPin, clockPin, dataRED1);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, 1);
    delay(300);
  }
  for (int l = 0; l < 10; l++)
  {
    //load the light sequence you want from array
    dataRED3 = dataArrayRED3[l];
    dataGREEN3 = dataArrayGREEN3[l];
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, 0);
    //move 'em out
    shiftOut(dataPin, clockPin, dataGREEN3);
    shiftOut(dataPin, clockPin, dataRED3);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, 1);
    delay(300);
  }
}

// the heart of the program
void shiftOut(int myDataPin, int myClockPin, byte myDataOut)
{
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low
  //internal function setup
  int i = 0;
  int pinState;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
  for (i = 7; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    pinState = (myDataOut & (1<<i) ? 1 : 0);
    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }
  //stop shifting
  digitalWrite(myClockPin, 0);
}

//blinks the whole register based on the number of times you want to
//blink "n" and the pause between them "d"
//starts with a moment of darkness to make sure the first blink
//has its full visual effect.
void blinkAll_2Bytes(int n, int d)
{
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, 1);
  delay(200);
  for (int x = 0; x < n; x++)
  {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 255);
    shiftOut(dataPin, clockPin, 255);
    digitalWrite(latchPin, 1);
    delay(d);
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 0);
    digitalWrite(latchPin, 1);
    delay(d);
  }
}

