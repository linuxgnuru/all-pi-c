/* Converted from ./common/font.h by glcd_font_tool 0.00 [19/05/2015], (c)F_Trevor_Gowen */
/**********************************************/
/*                                            */
/*       Font file generated by cpi2fnt       */
/*       ------------------------------       */
/*       Combined with the alpha-numeric      */
/*       portion of Greg Harp's old PEARL     */
/*       font (from earlier versions of       */
/*       linux-m86k) by John Shifflett        */
/*                                            */
/**********************************************/
/* $Id: oled128.h,v 1.2 2018/03/31 21:31:30 pi Exp $ */

static const int oled128Width  = 8;
static const int oled128Height = 8;

static unsigned char oled128 [] =
{
   /* 0 0x00 '^@' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 1 0x01 '^A' */
   0x7e, /* 01111110 */
   0x81, /* 10000001 */
   0x95, /* 10010101 */
   0xb1, /* 10110001 */
   0xb1, /* 10110001 */
   0x95, /* 10010101 */
   0x81, /* 10000001 */
   0x7e, /* 01111110 */

   /* 2 0x02 '^B' */
   0x7e, /* 01111110 */
   0xff, /* 11111111 */
   0xeb, /* 11101011 */
   0xcf, /* 11001111 */
   0xcf, /* 11001111 */
   0xeb, /* 11101011 */
   0xff, /* 11111111 */
   0x7e, /* 01111110 */

   /* 3 0x03 '^C' */
   0x0e, /* 00001110 */
   0x1f, /* 00011111 */
   0x3f, /* 00111111 */
   0x7e, /* 01111110 */
   0x3f, /* 00111111 */
   0x1f, /* 00011111 */
   0x0e, /* 00001110 */
   0x00, /* 00000000 */

   /* 4 0x04 '^D' */
   0x08, /* 00001000 */
   0x1c, /* 00011100 */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x3e, /* 00111110 */
   0x1c, /* 00011100 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 5 0x05 '^E' */
   0x38, /* 00111000 */
   0x3a, /* 00111010 */
   0x9f, /* 10011111 */
   0xff, /* 11111111 */
   0x9f, /* 10011111 */
   0x3a, /* 00111010 */
   0x38, /* 00111000 */
   0x00, /* 00000000 */

   /* 6 0x06 '^F' */
   0x18, /* 00011000 */
   0x3c, /* 00111100 */
   0xbe, /* 10111110 */
   0xff, /* 11111111 */
   0xbe, /* 10111110 */
   0x3c, /* 00111100 */
   0x18, /* 00011000 */
   0x00, /* 00000000 */

   /* 7 0x07 '^G' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x18, /* 00011000 */
   0x3c, /* 00111100 */
   0x3c, /* 00111100 */
   0x18, /* 00011000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 8 0x08 '^H' */
   0xff, /* 11111111 */
   0xff, /* 11111111 */
   0xe7, /* 11100111 */
   0xc3, /* 11000011 */
   0xc3, /* 11000011 */
   0xe7, /* 11100111 */
   0xff, /* 11111111 */
   0xff, /* 11111111 */

   /* 9 0x09 '^I' */
   0x00, /* 00000000 */
   0x3c, /* 00111100 */
   0x66, /* 01100110 */
   0x42, /* 01000010 */
   0x42, /* 01000010 */
   0x66, /* 01100110 */
   0x3c, /* 00111100 */
   0x00, /* 00000000 */

   /* 10 0x0a '^J' */
   0xff, /* 11111111 */
   0xc3, /* 11000011 */
   0x99, /* 10011001 */
   0xbd, /* 10111101 */
   0xbd, /* 10111101 */
   0x99, /* 10011001 */
   0xc3, /* 11000011 */
   0xff, /* 11111111 */

   /* 11 0x0b '^K' */
   0x70, /* 01110000 */
   0xf8, /* 11111000 */
   0x88, /* 10001000 */
   0x88, /* 10001000 */
   0xfd, /* 11111101 */
   0x7f, /* 01111111 */
   0x07, /* 00000111 */
   0x0f, /* 00001111 */

   /* 12 0x0c '^L' */
   0x00, /* 00000000 */
   0x4e, /* 01001110 */
   0x5f, /* 01011111 */
   0xf1, /* 11110001 */
   0xf1, /* 11110001 */
   0x5f, /* 01011111 */
   0x4e, /* 01001110 */
   0x00, /* 00000000 */

   /* 13 0x0d '^M' */
   0xc0, /* 11000000 */
   0xe0, /* 11100000 */
   0xff, /* 11111111 */
   0x7f, /* 01111111 */
   0x05, /* 00000101 */
   0x05, /* 00000101 */
   0x07, /* 00000111 */
   0x07, /* 00000111 */

   /* 14 0x0e '^N' */
   0xc0, /* 11000000 */
   0xff, /* 11111111 */
   0x7f, /* 01111111 */
   0x05, /* 00000101 */
   0x05, /* 00000101 */
   0x65, /* 01100101 */
   0x7f, /* 01111111 */
   0x3f, /* 00111111 */

   /* 15 0x0f '^O' */
   0x5a, /* 01011010 */
   0x5a, /* 01011010 */
   0x3c, /* 00111100 */
   0xe7, /* 11100111 */
   0xe7, /* 11100111 */
   0x3c, /* 00111100 */
   0x5a, /* 01011010 */
   0x5a, /* 01011010 */

   /* 16 0x10 '^P' */
   0x7f, /* 01111111 */
   0x3e, /* 00111110 */
   0x3e, /* 00111110 */
   0x1c, /* 00011100 */
   0x1c, /* 00011100 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 17 0x11 '^Q' */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x1c, /* 00011100 */
   0x1c, /* 00011100 */
   0x3e, /* 00111110 */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */

   /* 18 0x12 '^R' */
   0x00, /* 00000000 */
   0x24, /* 00100100 */
   0x66, /* 01100110 */
   0xff, /* 11111111 */
   0xff, /* 11111111 */
   0x66, /* 01100110 */
   0x24, /* 00100100 */
   0x00, /* 00000000 */

   /* 19 0x13 '^S' */
   0x00, /* 00000000 */
   0x5f, /* 01011111 */
   0x5f, /* 01011111 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x5f, /* 01011111 */
   0x5f, /* 01011111 */
   0x00, /* 00000000 */

   /* 20 0x14 '^T' */
   0x06, /* 00000110 */
   0x0f, /* 00001111 */
   0x09, /* 00001001 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x01, /* 00000001 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */

   /* 21 0x15 '^U' */
   0x40, /* 01000000 */
   0x9a, /* 10011010 */
   0xbf, /* 10111111 */
   0xa5, /* 10100101 */
   0xa5, /* 10100101 */
   0xfd, /* 11111101 */
   0x59, /* 01011001 */
   0x02, /* 00000010 */

   /* 22 0x16 '^V' */
   0x00, /* 00000000 */
   0x70, /* 01110000 */
   0x70, /* 01110000 */
   0x70, /* 01110000 */
   0x70, /* 01110000 */
   0x70, /* 01110000 */
   0x70, /* 01110000 */
   0x00, /* 00000000 */

   /* 23 0x17 '^W' */
   0x80, /* 10000000 */
   0x94, /* 10010100 */
   0xb6, /* 10110110 */
   0xff, /* 11111111 */
   0xff, /* 11111111 */
   0xb6, /* 10110110 */
   0x94, /* 10010100 */
   0x80, /* 10000000 */

   /* 24 0x18 '^X' */
   0x00, /* 00000000 */
   0x04, /* 00000100 */
   0x06, /* 00000110 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x06, /* 00000110 */
   0x04, /* 00000100 */
   0x00, /* 00000000 */

   /* 25 0x19 '^Y' */
   0x00, /* 00000000 */
   0x10, /* 00010000 */
   0x30, /* 00110000 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x30, /* 00110000 */
   0x10, /* 00010000 */
   0x00, /* 00000000 */

   /* 26 0x1a '^Z' */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x2a, /* 00101010 */
   0x3e, /* 00111110 */
   0x1c, /* 00011100 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 27 0x1b '^[' */
   0x08, /* 00001000 */
   0x1c, /* 00011100 */
   0x3e, /* 00111110 */
   0x2a, /* 00101010 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 28 0x1c '^\' */
   0x3c, /* 00111100 */
   0x3c, /* 00111100 */
   0x20, /* 00100000 */
   0x20, /* 00100000 */
   0x20, /* 00100000 */
   0x20, /* 00100000 */
   0x20, /* 00100000 */
   0x00, /* 00000000 */

   /* 29 0x1d '^]' */
   0x08, /* 00001000 */
   0x1c, /* 00011100 */
   0x3e, /* 00111110 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x3e, /* 00111110 */
   0x1c, /* 00011100 */
   0x08, /* 00001000 */

   /* 30 0x1e '^^' */
   0x30, /* 00110000 */
   0x38, /* 00111000 */
   0x3c, /* 00111100 */
   0x3e, /* 00111110 */
   0x3e, /* 00111110 */
   0x3c, /* 00111100 */
   0x38, /* 00111000 */
   0x30, /* 00110000 */

   /* 31 0x1f '^_' */
   0x06, /* 00000110 */
   0x0e, /* 00001110 */
   0x1e, /* 00011110 */
   0x3e, /* 00111110 */
   0x3e, /* 00111110 */
   0x1e, /* 00011110 */
   0x0e, /* 00001110 */
   0x06, /* 00000110 */

   /* 32 0x20 ' ' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 33 0x21 '!' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x0e, /* 00001110 */
   0x5f, /* 01011111 */
   0x5f, /* 01011111 */
   0x0e, /* 00001110 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 34 0x22 '"' */
   0x00, /* 00000000 */
   0x03, /* 00000011 */
   0x03, /* 00000011 */
   0x00, /* 00000000 */
   0x03, /* 00000011 */
   0x03, /* 00000011 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 35 0x23 '#' */
   0x14, /* 00010100 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x14, /* 00010100 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x14, /* 00010100 */
   0x00, /* 00000000 */

   /* 36 0x24 '$' */
   0x00, /* 00000000 */
   0x24, /* 00100100 */
   0x2e, /* 00101110 */
   0x6b, /* 01101011 */
   0x6b, /* 01101011 */
   0x3a, /* 00111010 */
   0x12, /* 00010010 */
   0x00, /* 00000000 */

   /* 37 0x25 '%' */
   0x46, /* 01000110 */
   0x66, /* 01100110 */
   0x30, /* 00110000 */
   0x18, /* 00011000 */
   0x0c, /* 00001100 */
   0x66, /* 01100110 */
   0x62, /* 01100010 */
   0x00, /* 00000000 */

   /* 38 0x26 '&' */
   0x30, /* 00110000 */
   0x7e, /* 01111110 */
   0x4f, /* 01001111 */
   0x59, /* 01011001 */
   0x37, /* 00110111 */
   0x7a, /* 01111010 */
   0x48, /* 01001000 */
   0x00, /* 00000000 */

   /* 39 0x27 ''' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x04, /* 00000100 */
   0x07, /* 00000111 */
   0x03, /* 00000011 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 40 0x28 '(' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x1c, /* 00011100 */
   0x3e, /* 00111110 */
   0x63, /* 01100011 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 41 0x29 ')' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x41, /* 01000001 */
   0x63, /* 01100011 */
   0x3e, /* 00111110 */
   0x1c, /* 00011100 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 42 0x2a '*' */
   0x08, /* 00001000 */
   0x2a, /* 00101010 */
   0x3e, /* 00111110 */
   0x1c, /* 00011100 */
   0x1c, /* 00011100 */
   0x3e, /* 00111110 */
   0x2a, /* 00101010 */
   0x08, /* 00001000 */

   /* 43 0x2b '+' */
   0x00, /* 00000000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x3e, /* 00111110 */
   0x3e, /* 00111110 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 44 0x2c ',' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x80, /* 10000000 */
   0xe0, /* 11100000 */
   0x60, /* 01100000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 45 0x2d '-' */
   0x00, /* 00000000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 46 0x2e '.' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x60, /* 01100000 */
   0x60, /* 01100000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 47 0x2f '/' */
   0x40, /* 01000000 */
   0x60, /* 01100000 */
   0x30, /* 00110000 */
   0x18, /* 00011000 */
   0x0c, /* 00001100 */
   0x06, /* 00000110 */
   0x03, /* 00000011 */
   0x01, /* 00000001 */

   /* 48 0x30 '0' */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x59, /* 01011001 */
   0x5d, /* 01011101 */
   0x4d, /* 01001101 */
   0x7f, /* 01111111 */
   0x3e, /* 00111110 */
   0x00, /* 00000000 */

   /* 49 0x31 '1' */
   0x00, /* 00000000 */
   0x02, /* 00000010 */
   0x02, /* 00000010 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 50 0x32 '2' */
   0x42, /* 01000010 */
   0x63, /* 01100011 */
   0x71, /* 01110001 */
   0x59, /* 01011001 */
   0x4d, /* 01001101 */
   0x47, /* 01000111 */
   0x42, /* 01000010 */
   0x00, /* 00000000 */

   /* 51 0x33 '3' */
   0x22, /* 00100010 */
   0x63, /* 01100011 */
   0x41, /* 01000001 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x7f, /* 01111111 */
   0x36, /* 00110110 */
   0x00, /* 00000000 */

   /* 52 0x34 '4' */
   0x18, /* 00011000 */
   0x1c, /* 00011100 */
   0x16, /* 00010110 */
   0x13, /* 00010011 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x10, /* 00010000 */
   0x00, /* 00000000 */

   /* 53 0x35 '5' */
   0x27, /* 00100111 */
   0x67, /* 01100111 */
   0x45, /* 01000101 */
   0x45, /* 01000101 */
   0x45, /* 01000101 */
   0x7d, /* 01111101 */
   0x39, /* 00111001 */
   0x00, /* 00000000 */

   /* 54 0x36 '6' */
   0x3c, /* 00111100 */
   0x7e, /* 01111110 */
   0x4b, /* 01001011 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x78, /* 01111000 */
   0x30, /* 00110000 */
   0x00, /* 00000000 */

   /* 55 0x37 '7' */
   0x01, /* 00000001 */
   0x61, /* 01100001 */
   0x71, /* 01110001 */
   0x19, /* 00011001 */
   0x0d, /* 00001101 */
   0x07, /* 00000111 */
   0x03, /* 00000011 */
   0x00, /* 00000000 */

   /* 56 0x38 '8' */
   0x36, /* 00110110 */
   0x7f, /* 01111111 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x7f, /* 01111111 */
   0x36, /* 00110110 */
   0x00, /* 00000000 */

   /* 57 0x39 '9' */
   0x06, /* 00000110 */
   0x0f, /* 00001111 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x69, /* 01101001 */
   0x3f, /* 00111111 */
   0x1e, /* 00011110 */
   0x00, /* 00000000 */

   /* 58 0x3a ':' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x66, /* 01100110 */
   0x66, /* 01100110 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 59 0x3b ';' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x80, /* 10000000 */
   0xe6, /* 11100110 */
   0x66, /* 01100110 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 60 0x3c '<' */
   0x00, /* 00000000 */
   0x08, /* 00001000 */
   0x1c, /* 00011100 */
   0x36, /* 00110110 */
   0x63, /* 01100011 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 61 0x3d '=' */
   0x00, /* 00000000 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x00, /* 00000000 */

   /* 62 0x3e '>' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x41, /* 01000001 */
   0x63, /* 01100011 */
   0x36, /* 00110110 */
   0x1c, /* 00011100 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 63 0x3f '?' */
   0x00, /* 00000000 */
   0x02, /* 00000010 */
   0x03, /* 00000011 */
   0x51, /* 01010001 */
   0x59, /* 01011001 */
   0x0f, /* 00001111 */
   0x06, /* 00000110 */
   0x00, /* 00000000 */

   /* 64 0x40 '@' */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x5d, /* 01011101 */
   0x5d, /* 01011101 */
   0x5f, /* 01011111 */
   0x1e, /* 00011110 */
   0x00, /* 00000000 */

   /* 65 0x41 'A' */
   0x78, /* 01111000 */
   0x7c, /* 01111100 */
   0x16, /* 00010110 */
   0x13, /* 00010011 */
   0x16, /* 00010110 */
   0x7c, /* 01111100 */
   0x78, /* 01111000 */
   0x00, /* 00000000 */

   /* 66 0x42 'B' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x7f, /* 01111111 */
   0x36, /* 00110110 */
   0x00, /* 00000000 */

   /* 67 0x43 'C' */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x63, /* 01100011 */
   0x22, /* 00100010 */
   0x00, /* 00000000 */

   /* 68 0x44 'D' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x7f, /* 01111111 */
   0x3e, /* 00111110 */
   0x00, /* 00000000 */

   /* 69 0x45 'E' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x49, /* 01001001 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */

   /* 70 0x46 'F' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x09, /* 00001001 */
   0x09, /* 00001001 */
   0x09, /* 00001001 */
   0x01, /* 00000001 */
   0x01, /* 00000001 */
   0x00, /* 00000000 */

   /* 71 0x47 'G' */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x49, /* 01001001 */
   0x7b, /* 01111011 */
   0x3a, /* 00111010 */
   0x00, /* 00000000 */

   /* 72 0x48 'H' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */

   /* 73 0x49 'I' */
   0x00, /* 00000000 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */

   /* 74 0x4a 'J' */
   0x30, /* 00110000 */
   0x70, /* 01110000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x7f, /* 01111111 */
   0x3f, /* 00111111 */
   0x00, /* 00000000 */

   /* 75 0x4b 'K' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x08, /* 00001000 */
   0x1c, /* 00011100 */
   0x36, /* 00110110 */
   0x63, /* 01100011 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */

   /* 76 0x4c 'L' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x00, /* 00000000 */

   /* 77 0x4d 'M' */
   0x7f, /* 01111111 */
   0x7e, /* 01111110 */
   0x0c, /* 00001100 */
   0x18, /* 00011000 */
   0x0c, /* 00001100 */
   0x7e, /* 01111110 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */

   /* 78 0x4e 'N' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x06, /* 00000110 */
   0x0c, /* 00001100 */
   0x18, /* 00011000 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */

   /* 79 0x4f 'O' */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x7f, /* 01111111 */
   0x3e, /* 00111110 */
   0x00, /* 00000000 */

   /* 80 0x50 'P' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x09, /* 00001001 */
   0x09, /* 00001001 */
   0x09, /* 00001001 */
   0x0f, /* 00001111 */
   0x06, /* 00000110 */
   0x00, /* 00000000 */

   /* 81 0x51 'Q' */
   0x3e, /* 00111110 */
   0x7f, /* 01111111 */
   0x51, /* 01010001 */
   0x71, /* 01110001 */
   0x61, /* 01100001 */
   0xff, /* 11111111 */
   0xbe, /* 10111110 */
   0x00, /* 00000000 */

   /* 82 0x52 'R' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x09, /* 00001001 */
   0x19, /* 00011001 */
   0x39, /* 00111001 */
   0x6f, /* 01101111 */
   0x46, /* 01000110 */
   0x00, /* 00000000 */

   /* 83 0x53 'S' */
   0x22, /* 00100010 */
   0x67, /* 01100111 */
   0x4d, /* 01001101 */
   0x49, /* 01001001 */
   0x59, /* 01011001 */
   0x73, /* 01110011 */
   0x22, /* 00100010 */
   0x00, /* 00000000 */

   /* 84 0x54 'T' */
   0x00, /* 00000000 */
   0x01, /* 00000001 */
   0x01, /* 00000001 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x01, /* 00000001 */
   0x01, /* 00000001 */
   0x00, /* 00000000 */

   /* 85 0x55 'U' */
   0x3f, /* 00111111 */
   0x7f, /* 01111111 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x7f, /* 01111111 */
   0x3f, /* 00111111 */
   0x00, /* 00000000 */

   /* 86 0x56 'V' */
   0x03, /* 00000011 */
   0x0f, /* 00001111 */
   0x3c, /* 00111100 */
   0x70, /* 01110000 */
   0x70, /* 01110000 */
   0x3c, /* 00111100 */
   0x0f, /* 00001111 */
   0x03, /* 00000011 */

   /* 87 0x57 'W' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x30, /* 00110000 */
   0x18, /* 00011000 */
   0x30, /* 00110000 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */

   /* 88 0x58 'X' */
   0x41, /* 01000001 */
   0x63, /* 01100011 */
   0x36, /* 00110110 */
   0x1c, /* 00011100 */
   0x1c, /* 00011100 */
   0x36, /* 00110110 */
   0x63, /* 01100011 */
   0x41, /* 01000001 */

   /* 89 0x59 'Y' */
   0x03, /* 00000011 */
   0x07, /* 00000111 */
   0x0c, /* 00001100 */
   0x78, /* 01111000 */
   0x78, /* 01111000 */
   0x0c, /* 00001100 */
   0x07, /* 00000111 */
   0x03, /* 00000011 */

   /* 90 0x5a 'Z' */
   0x41, /* 01000001 */
   0x61, /* 01100001 */
   0x71, /* 01110001 */
   0x59, /* 01011001 */
   0x4d, /* 01001101 */
   0x47, /* 01000111 */
   0x43, /* 01000011 */
   0x00, /* 00000000 */

   /* 91 0x5b '[' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 92 0x5c '\' */
   0x01, /* 00000001 */
   0x03, /* 00000011 */
   0x06, /* 00000110 */
   0x0c, /* 00001100 */
   0x18, /* 00011000 */
   0x30, /* 00110000 */
   0x60, /* 01100000 */
   0x40, /* 01000000 */

   /* 93 0x5d ']' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 94 0x5e '^' */
   0x08, /* 00001000 */
   0x0c, /* 00001100 */
   0x06, /* 00000110 */
   0x03, /* 00000011 */
   0x06, /* 00000110 */
   0x0c, /* 00001100 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 95 0x5f '_' */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x00, /* 00000000 */

   /* 96 0x60 '`' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x03, /* 00000011 */
   0x07, /* 00000111 */
   0x04, /* 00000100 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 97 0x61 'a' */
   0x20, /* 00100000 */
   0x74, /* 01110100 */
   0x54, /* 01010100 */
   0x54, /* 01010100 */
   0x54, /* 01010100 */
   0x7c, /* 01111100 */
   0x78, /* 01111000 */
   0x00, /* 00000000 */

   /* 98 0x62 'b' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x7c, /* 01111100 */
   0x38, /* 00111000 */
   0x00, /* 00000000 */

   /* 99 0x63 'c' */
   0x38, /* 00111000 */
   0x7c, /* 01111100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x6c, /* 01101100 */
   0x28, /* 00101000 */
   0x00, /* 00000000 */

   /* 100 0x64 'd' */
   0x38, /* 00111000 */
   0x7c, /* 01111100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */

   /* 101 0x65 'e' */
   0x38, /* 00111000 */
   0x7c, /* 01111100 */
   0x54, /* 01010100 */
   0x54, /* 01010100 */
   0x54, /* 01010100 */
   0x5c, /* 01011100 */
   0x18, /* 00011000 */
   0x00, /* 00000000 */

   /* 102 0x66 'f' */
   0x08, /* 00001000 */
   0x7e, /* 01111110 */
   0x7f, /* 01111111 */
   0x09, /* 00001001 */
   0x01, /* 00000001 */
   0x03, /* 00000011 */
   0x02, /* 00000010 */
   0x00, /* 00000000 */

   /* 103 0x67 'g' */
   0x18, /* 00011000 */
   0xbc, /* 10111100 */
   0xa4, /* 10100100 */
   0xa4, /* 10100100 */
   0xa4, /* 10100100 */
   0xfc, /* 11111100 */
   0x7c, /* 01111100 */
   0x00, /* 00000000 */

   /* 104 0x68 'h' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x04, /* 00000100 */
   0x04, /* 00000100 */
   0x04, /* 00000100 */
   0x7c, /* 01111100 */
   0x78, /* 01111000 */
   0x00, /* 00000000 */

   /* 105 0x69 'i' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x04, /* 00000100 */
   0x7d, /* 01111101 */
   0x7d, /* 01111101 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 106 0x6a 'j' */
   0x40, /* 01000000 */
   0xc0, /* 11000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0x80, /* 10000000 */
   0xfd, /* 11111101 */
   0x7d, /* 01111101 */
   0x00, /* 00000000 */

   /* 107 0x6b 'k' */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x10, /* 00010000 */
   0x38, /* 00111000 */
   0x6c, /* 01101100 */
   0x44, /* 01000100 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 108 0x6c 'l' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x01, /* 00000001 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 109 0x6d 'm' */
   0x7c, /* 01111100 */
   0x7c, /* 01111100 */
   0x0c, /* 00001100 */
   0x18, /* 00011000 */
   0x0c, /* 00001100 */
   0x7c, /* 01111100 */
   0x78, /* 01111000 */
   0x00, /* 00000000 */

   /* 110 0x6e 'n' */
   0x7c, /* 01111100 */
   0x7c, /* 01111100 */
   0x04, /* 00000100 */
   0x04, /* 00000100 */
   0x04, /* 00000100 */
   0x7c, /* 01111100 */
   0x78, /* 01111000 */
   0x00, /* 00000000 */

   /* 111 0x6f 'o' */
   0x38, /* 00111000 */
   0x7c, /* 01111100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x44, /* 01000100 */
   0x7c, /* 01111100 */
   0x38, /* 00111000 */
   0x00, /* 00000000 */

   /* 112 0x70 'p' */
   0xfc, /* 11111100 */
   0xfc, /* 11111100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x3c, /* 00111100 */
   0x18, /* 00011000 */
   0x00, /* 00000000 */

   /* 113 0x71 'q' */
   0x18, /* 00011000 */
   0x3c, /* 00111100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0x24, /* 00100100 */
   0xfc, /* 11111100 */
   0xfc, /* 11111100 */
   0x00, /* 00000000 */

   /* 114 0x72 'r' */
   0x7c, /* 01111100 */
   0x7c, /* 01111100 */
   0x08, /* 00001000 */
   0x04, /* 00000100 */
   0x04, /* 00000100 */
   0x0c, /* 00001100 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 115 0x73 's' */
   0x48, /* 01001000 */
   0x5c, /* 01011100 */
   0x54, /* 01010100 */
   0x54, /* 01010100 */
   0x54, /* 01010100 */
   0x74, /* 01110100 */
   0x24, /* 00100100 */
   0x00, /* 00000000 */

   /* 116 0x74 't' */
   0x00, /* 00000000 */
   0x04, /* 00000100 */
   0x3f, /* 00111111 */
   0x7f, /* 01111111 */
   0x44, /* 01000100 */
   0x64, /* 01100100 */
   0x20, /* 00100000 */
   0x00, /* 00000000 */

   /* 117 0x75 'u' */
   0x3c, /* 00111100 */
   0x7c, /* 01111100 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x40, /* 01000000 */
   0x7c, /* 01111100 */
   0x3c, /* 00111100 */
   0x00, /* 00000000 */

   /* 118 0x76 'v' */
   0x1c, /* 00011100 */
   0x3c, /* 00111100 */
   0x60, /* 01100000 */
   0x40, /* 01000000 */
   0x60, /* 01100000 */
   0x3c, /* 00111100 */
   0x1c, /* 00011100 */
   0x00, /* 00000000 */

   /* 119 0x77 'w' */
   0x3c, /* 00111100 */
   0x7c, /* 01111100 */
   0x60, /* 01100000 */
   0x30, /* 00110000 */
   0x60, /* 01100000 */
   0x7c, /* 01111100 */
   0x3c, /* 00111100 */
   0x00, /* 00000000 */

   /* 120 0x78 'x' */
   0x44, /* 01000100 */
   0x6c, /* 01101100 */
   0x38, /* 00111000 */
   0x10, /* 00010000 */
   0x38, /* 00111000 */
   0x6c, /* 01101100 */
   0x44, /* 01000100 */
   0x00, /* 00000000 */

   /* 121 0x79 'y' */
   0x04, /* 00000100 */
   0x8c, /* 10001100 */
   0xd8, /* 11011000 */
   0x70, /* 01110000 */
   0x30, /* 00110000 */
   0x18, /* 00011000 */
   0x0c, /* 00001100 */
   0x04, /* 00000100 */

   /* 122 0x7a 'z' */
   0x44, /* 01000100 */
   0x64, /* 01100100 */
   0x74, /* 01110100 */
   0x54, /* 01010100 */
   0x5c, /* 01011100 */
   0x4c, /* 01001100 */
   0x44, /* 01000100 */
   0x00, /* 00000000 */

   /* 123 0x7b '{' */
   0x00, /* 00000000 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x3e, /* 00111110 */
   0x77, /* 01110111 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x00, /* 00000000 */

   /* 124 0x7c '|' */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x7f, /* 01111111 */
   0x7f, /* 01111111 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */
   0x00, /* 00000000 */

   /* 125 0x7d '}' */
   0x00, /* 00000000 */
   0x41, /* 01000001 */
   0x41, /* 01000001 */
   0x77, /* 01110111 */
   0x3e, /* 00111110 */
   0x08, /* 00001000 */
   0x08, /* 00001000 */
   0x00, /* 00000000 */

   /* 126 0x7e '~' */
   0x02, /* 00000010 */
   0x01, /* 00000001 */
   0x01, /* 00000001 */
   0x03, /* 00000011 */
   0x02, /* 00000010 */
   0x02, /* 00000010 */
   0x01, /* 00000001 */
   0x00, /* 00000000 */

   /* 127 0x7f '' */
   0x70, /* 01110000 */
   0x78, /* 01111000 */
   0x4c, /* 01001100 */
   0x46, /* 01000110 */
   0x4c, /* 01001100 */
   0x78, /* 01111000 */
   0x70, /* 01110000 */
   0x00, /* 00000000 */

};

/* Change Log */
/*
 * $Log: oled128.h,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.1  2015/05/19 17:23:20  pi
 * Initial revision
 *
 *
 *
 *
 */