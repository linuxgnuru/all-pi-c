/*
   shift register pins:
         +--------+
   2out -+  1  16 +- Vin
   3out -+  2  15 +- 1out
   4out -+  3  14 +- data (blue)
   5out -+  4  13 +- ground
   6out -+  5  12 +- latch (green + 1u cap if first)
   7out -+  6  11 +- lock (yellow)
   8out -+  7  10 +- Vin
    gnd -+  8   9 +- serial out
         +--------+
 */
/*
   const byte digitArray[] = {
 // BCDGE.FA
  0b01111011, // 0
  0b00001010, // 1
  0b11110010, // 2
  0b10111010, // 3
  0b10001011, // 4
  0b10111001, // 5
  0b11111001, // 6
  0b00011010, // 7
  0b11111011, // 8
  0b10111011  // 9
};
for decimal point:
    bitWrite(myData, 2, dp);
    */

static const int dpPin = 2;

static const int leds[10][7] = {
//     A   B   C   D   E   F   G
    {  4,  1,  3,  5,  6,  0, -1 }, // 0
    { -1,  1,  3, -1, -1, -1, -1 }, // 1
    {  4,  1, -1,  5,  6, -1,  7 }, // 2
    {  4,  1,  3,  5, -1, -1,  7 }, // 3
    { -1,  1,  3, -1, -1,  0,  7 }, // 4
    {  4, -1,  3,  5, -1,  0,  7 }, // 5
    {  4, -1,  3,  5,  6,  0,  7 }, // 6
    {  4,  1,  3, -1, -1, -1, -1 }, // 7
    {  4,  1,  3,  5,  6,  0,  7 }, // 8
    {  4,  1,  3,  5, -1,  0,  7 }  // 9
};

#if 0
//    4, 6, 5, 0, 1, 2, 3, 7
// use: A, B, C, D, E, F, G, dp
static const int ledPins[8] = {
    4, 1, 3, 5, 6, 0, 7, 2
};

#endif
