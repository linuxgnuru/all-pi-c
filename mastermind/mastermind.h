void swap(int *a, int *b)
{
  int temp = *a;
  
  *a = *b;
  *b = temp;
}

void create_passcode()
{
  for (int i = 0; i < MAX_DIG; i++)
    used[i] = 0;
  for (int cur_index = 0; cur_index < NUM_LEDS; cur_index++)
  {
    int ran = random(0, MAX_DIG);
    used[ran] = 1;
    passcode[cur_index] = ran + 48;
  }
/*
  Serial.println("");
  Serial.print("[Pass: ");
  for (int i = 0; i < NUM_LEDS; i++)
  {
    Serial.print(passcode[i]);
  }
  Serial.println("]");
  */
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut)
{
  int pinState;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  for (int i = 7; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    pinState = (myDataOut & (1<<i) ? 1 : 0);
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }
  digitalWrite(myClockPin, 0);
}

void g2r(int d)
{
  int d1 = 0b00000000;
  int d2 = 0b00000000;
  // loop through all printable digits for 7 segment LED
  for (int i = 0; i < MAX_DIG; i++)
  {
    digitalWrite(dLatchPin, 0);
    shiftOut(dDataPin, dClockPin, digitArray[i]);
    digitalWrite(dLatchPin, 1);
    delay(250);
  }
  for (int i = 7; i > -1; i--)
  {
    bitSet(d1, i);
    digitalWrite(rLatchPin, 0);
    shiftOut(rDataPin, rClockPin, d1);
    shiftOut(rDataPin, rClockPin, d2);
    digitalWrite(rLatchPin, 1);
    delay(d);
    if (i != 5 && i != 2)
    {
      bitClear(d1, i);
      digitalWrite(rLatchPin, 0);
      shiftOut(rDataPin, rClockPin, d1);
      shiftOut(rDataPin, rClockPin, d2);
      digitalWrite(rLatchPin, 1);
    }
  }
  d1 = 0b00100100;
  for (int i = 7; i > 0; i--)
  {
    bitSet(d2, i);
    digitalWrite(rLatchPin, 0);
    shiftOut(rDataPin, rClockPin, d1);
    shiftOut(rDataPin, rClockPin, d2);
    digitalWrite(rLatchPin, 1);
    delay(d);
    if (i != 7 && i != 4)
    {
      bitClear(d2, i);
      digitalWrite(rLatchPin, 0);
      shiftOut(rDataPin, rClockPin, d1);
      shiftOut(rDataPin, rClockPin, d2);
      digitalWrite(rLatchPin, 1);
    }
  }
}

void blip(int times, int d)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(alertPin, HIGH);
    delay(d);
    digitalWrite(alertPin, LOW);
    delay(d);
  }
}

void changeBit(int pos, int color)
{
  switch (pos)
  {
  case 0: // data1: 7 6 5
    switch (color)
    {
    case LED_G:
      bitSet(data1, 7);
      bitClear(data1, 6);
      bitClear(data1, 5);
      break;
    case LED_Y:
      bitClear(data1, 7);
      bitSet(data1, 6);
      bitClear(data1, 5);
      break;
    case LED_R:
      bitClear(data1, 7);
      bitClear(data1, 6);
      bitSet(data1, 5);
      break;
    case LED_0:
      for (int i = 5; i < 8; i++)
        bitClear(data1, i);
      break;
    }
    break;
  case 1: // data1: 4 3 2
    switch (color)
    {
    case LED_G:
      bitSet(data1, 4);
      bitClear(data1, 3);
      bitClear(data1, 2);
      break;
    case LED_Y:
      bitClear(data1, 4);
      bitSet(data1, 3);
      bitClear(data1, 2);
      break;
    case LED_R:
      bitClear(data1, 4);
      bitClear(data1, 3);
      bitSet(data1, 2);
      break;
    case LED_0:
      for (int i = 2; i < 5; i++)
        bitClear(data1, i);
      break;
    }
    break;
  case 2: // data1: 1 0, data2: 7
    switch (color)
    {
    case LED_G:
      bitSet(data1, 1);
      bitClear(data1, 0);
      bitClear(data2, 7);
      break;
    case LED_Y:
      bitClear(data1, 1);
      bitSet(data1, 0);
      bitClear(data2, 7);
      break;
    case LED_R:
      bitClear(data1, 1);
      bitClear(data1, 0);
      bitSet(data2, 7);
      break;
    case LED_0:
      bitClear(data1, 1);
      bitClear(data1, 0);
      bitClear(data2, 7);
      break;
    }
    break;
  case 3: // data2: 6 5 4
    switch (color)
    {
    case LED_G:
      bitSet(data2, 6);
      bitClear(data2, 5);
      bitClear(data2, 4);
      break;
    case LED_Y:
      bitClear(data2, 6);
      bitSet(data2, 5);
      bitClear(data2, 4);
      break;
    case LED_R:
      bitClear(data2, 6);
      bitClear(data2, 5);
      bitSet(data2, 4);
      break;
    case LED_0:
      for (int i = 4; i < 7; i++)
        bitClear(data2, i);
      break;
    }
    break;
  }
  digitalWrite(rLatchPin, 0);
  shiftOut(rDataPin, rClockPin, data1);
  shiftOut(rDataPin, rClockPin, data2);
  digitalWrite(rLatchPin, 1);
}

void printSingleDigit(char k, int cf)
{
  if (k != '*' && k != '#' && k != 'A' && k != 'B' && k != 'C' && k != 'D')
  {
    digitalWrite(dLatchPin, 0);
    shiftOut(dDataPin, dClockPin, digitArray[k - 48]);
    digitalWrite(dLatchPin, 1);
    if (cf == true)
    {
      delay(500);
      shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    }
  }
}

void printMultDigit(char *text, int text_len)
{
  for (int i = 0; i < text_len; i++)
  {
    printSingleDigit(text[i], true);
    delay(100);
  }
}

// possibly combine both clears; and have a if flag
void shiftClear(int latchP, int dataP, int clockP, int bNum)
{
  digitalWrite(latchP, 0);
  shiftOut(dataP, clockP, 0b00000000);
  if (bNum == 2)
    shiftOut(dataP, clockP, 0b00000000);
  digitalWrite(latchP, 1);
}

void printNumLeft()
{
  int disp_tries = 0;
  char ten[2];

  disp_tries = abs(max_tries - num_tries);
  ten[0] = (max_tries > 9 ? (disp_tries / 10) + 48 : '0');
  ten[1] = (disp_tries % 10) + 48;
  printMultDigit(ten, 2);
}

void printMessage(char *message, int sz)
{
  for (int i = 0; i < sz; i++)
  {
    digitalWrite(dLatchPin, 0);
    shiftOut(dDataPin, dClockPin, alphabetArray[message[i] - 65]);
    digitalWrite(dLatchPin, 1);
    delay(300);
    shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    delay(100);
  }
}

void showWin()
{
  for (int j = 0; j < 6; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      digitalWrite(dLatchPin, 0);
      shiftOut(dDataPin, dClockPin, winAnim[i]);
      digitalWrite(dLatchPin, 1);
      delay(90);
      shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    }
  }
}


