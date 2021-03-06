/* Column-based font bitmap from ../PCD8544_RPi_BBSPI/PCD8544.c */
/* $Id: PCD8544_font.h,v 1.2 2018/03/31 21:31:30 pi Exp $ */
/* Re-named and comments added                           */
/* Height & Width order swapped to indicate col/row font */

/* Conditionally Flag this file */
#ifndef PCD8544_FONT_H
#define PCD8544_FONT_H 1

static const int font8x5Width  = 5;
static const int font8x5Height = 8;

static unsigned char  font8x5[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00,   /* 0 0x00 '^@' */
  0x3E, 0x5B, 0x4F, 0x5B, 0x3E,   /* 1 0x01 '^A' */
  0x3E, 0x6B, 0x4F, 0x6B, 0x3E,   /* 2 0x02 '^B' */
  0x1C, 0x3E, 0x7C, 0x3E, 0x1C,   /* 3 0x03 '^C' */
  0x18, 0x3C, 0x7E, 0x3C, 0x18,   /* 4 0x04 '^D' */
  0x1C, 0x57, 0x7D, 0x57, 0x1C,   /* 5 0x05 '^E' */
  0x1C, 0x5E, 0x7F, 0x5E, 0x1C,   /* 6 0x06 '^F' */
  0x00, 0x18, 0x3C, 0x18, 0x00,   /* 7 0x07 '^G' */
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF,   /* 8 0x08 '^H' */
  0x00, 0x18, 0x24, 0x18, 0x00,   /* 9 0x09 '^I' */
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF,   /* 10 0x0a '^J' */
  0x30, 0x48, 0x3A, 0x06, 0x0E,   /* 11 0x0b '^K' */
  0x26, 0x29, 0x79, 0x29, 0x26,   /* 12 0x0c '^L' */
  0x40, 0x7F, 0x05, 0x05, 0x07,   /* 13 0x0d '^M' */
  0x40, 0x7F, 0x05, 0x25, 0x3F,   /* 14 0x0e '^N' */
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A,   /* 15 0x0f '^O' */
  0x7F, 0x3E, 0x1C, 0x1C, 0x08,   /* 16 0x10 '^P' */
  0x08, 0x1C, 0x1C, 0x3E, 0x7F,   /* 17 0x11 '^Q' */
  0x14, 0x22, 0x7F, 0x22, 0x14,   /* 18 0x12 '^R' */
  0x5F, 0x5F, 0x00, 0x5F, 0x5F,   /* 19 0x13 '^S' */
  0x06, 0x09, 0x7F, 0x01, 0x7F,   /* 20 0x14 '^T' */
  0x00, 0x66, 0x89, 0x95, 0x6A,   /* 21 0x15 '^U' */
  0x60, 0x60, 0x60, 0x60, 0x60,   /* 22 0x16 '^V' */
  0x94, 0xA2, 0xFF, 0xA2, 0x94,   /* 23 0x17 '^W' */
  0x08, 0x04, 0x7E, 0x04, 0x08,   /* 24 0x18 '^X' */
  0x10, 0x20, 0x7E, 0x20, 0x10,   /* 25 0x19 '^Y' */
  0x08, 0x08, 0x2A, 0x1C, 0x08,   /* 26 0x1a '^Z' */
  0x08, 0x1C, 0x2A, 0x08, 0x08,   /* 27 0x1b '^[' */
  0x1E, 0x10, 0x10, 0x10, 0x10,   /* 28 0x1c '^\' */
  0x0C, 0x1E, 0x0C, 0x1E, 0x0C,   /* 29 0x1d '^]' */
  0x30, 0x38, 0x3E, 0x38, 0x30,   /* 30 0x1e '^^' */
  0x06, 0x0E, 0x3E, 0x0E, 0x06,   /* 31 0x1f '^_' */
  0x00, 0x00, 0x00, 0x00, 0x00,   /* 32 0x20 ' ' */
  0x00, 0x00, 0x5F, 0x00, 0x00,   /* 33 0x21 '!' */
  0x00, 0x07, 0x00, 0x07, 0x00,   /* 34 0x22 '"' */
  0x14, 0x7F, 0x14, 0x7F, 0x14,   /* 35 0x23 '#' */
  0x24, 0x2A, 0x7F, 0x2A, 0x12,   /* 36 0x24 '$' */
  0x23, 0x13, 0x08, 0x64, 0x62,   /* 37 0x25 '%' */
  0x36, 0x49, 0x56, 0x20, 0x50,   /* 38 0x26 '&' */
  0x00, 0x08, 0x07, 0x03, 0x00,   /* 39 0x27 ''' */
  0x00, 0x1C, 0x22, 0x41, 0x00,   /* 40 0x28 '(' */
  0x00, 0x41, 0x22, 0x1C, 0x00,   /* 41 0x29 ')' */
  0x2A, 0x1C, 0x7F, 0x1C, 0x2A,   /* 42 0x2a '*' */
  0x08, 0x08, 0x3E, 0x08, 0x08,   /* 43 0x2b '+' */
  0x00, 0x80, 0x70, 0x30, 0x00,   /* 44 0x2c ',' */
  0x08, 0x08, 0x08, 0x08, 0x08,   /* 45 0x2d '-' */
  0x00, 0x00, 0x60, 0x60, 0x00,   /* 46 0x2e '.' */
  0x20, 0x10, 0x08, 0x04, 0x02,   /* 47 0x2f '/' */
  0x3E, 0x51, 0x49, 0x45, 0x3E,   /* 48 0x30 '0' */
  0x00, 0x42, 0x7F, 0x40, 0x00,   /* 49 0x31 '1' */
  0x72, 0x49, 0x49, 0x49, 0x46,   /* 50 0x32 '2' */
  0x21, 0x41, 0x49, 0x4D, 0x33,   /* 51 0x33 '3' */
  0x18, 0x14, 0x12, 0x7F, 0x10,   /* 52 0x34 '4' */
  0x27, 0x45, 0x45, 0x45, 0x39,   /* 53 0x35 '5' */
  0x3C, 0x4A, 0x49, 0x49, 0x31,   /* 54 0x36 '6' */
  0x41, 0x21, 0x11, 0x09, 0x07,   /* 55 0x37 '7' */
  0x36, 0x49, 0x49, 0x49, 0x36,   /* 56 0x38 '8' */
  0x46, 0x49, 0x49, 0x29, 0x1E,   /* 57 0x39 '9' */
  0x00, 0x00, 0x14, 0x00, 0x00,   /* 58 0x3a ':' */
  0x00, 0x40, 0x34, 0x00, 0x00,   /* 59 0x3b ';' */
  0x00, 0x08, 0x14, 0x22, 0x41,   /* 60 0x3c '<' */
  0x14, 0x14, 0x14, 0x14, 0x14,   /* 61 0x3d '=' */
  0x00, 0x41, 0x22, 0x14, 0x08,   /* 62 0x3e '>' */
  0x02, 0x01, 0x59, 0x09, 0x06,   /* 63 0x3f '?' */
  0x3E, 0x41, 0x5D, 0x59, 0x4E,   /* 64 0x40 '@' */
  0x7C, 0x12, 0x11, 0x12, 0x7C,   /* 65 0x41 'A' */
  0x7F, 0x49, 0x49, 0x49, 0x36,   /* 66 0x42 'B' */
  0x3E, 0x41, 0x41, 0x41, 0x22,   /* 67 0x43 'C' */
  0x7F, 0x41, 0x41, 0x41, 0x3E,   /* 68 0x44 'D' */
  0x7F, 0x49, 0x49, 0x49, 0x41,   /* 69 0x45 'E' */
  0x7F, 0x09, 0x09, 0x09, 0x01,   /* 70 0x46 'F' */
  0x3E, 0x41, 0x41, 0x51, 0x73,   /* 71 0x47 'G' */
  0x7F, 0x08, 0x08, 0x08, 0x7F,   /* 72 0x48 'H' */
  0x00, 0x41, 0x7F, 0x41, 0x00,   /* 73 0x49 'I' */
  0x20, 0x40, 0x41, 0x3F, 0x01,   /* 74 0x4a 'J' */
  0x7F, 0x08, 0x14, 0x22, 0x41,   /* 75 0x4b 'K' */
  0x7F, 0x40, 0x40, 0x40, 0x40,   /* 76 0x4c 'L' */
  0x7F, 0x02, 0x1C, 0x02, 0x7F,   /* 77 0x4d 'M' */
  0x7F, 0x04, 0x08, 0x10, 0x7F,   /* 78 0x4e 'N' */
  0x3E, 0x41, 0x41, 0x41, 0x3E,   /* 79 0x4f 'O' */
  0x7F, 0x09, 0x09, 0x09, 0x06,   /* 80 0x50 'P' */
  0x3E, 0x41, 0x51, 0x21, 0x5E,   /* 81 0x51 'Q' */
  0x7F, 0x09, 0x19, 0x29, 0x46,   /* 82 0x52 'R' */
  0x26, 0x49, 0x49, 0x49, 0x32,   /* 83 0x53 'S' */
  0x03, 0x01, 0x7F, 0x01, 0x03,   /* 84 0x54 'T' */
  0x3F, 0x40, 0x40, 0x40, 0x3F,   /* 85 0x55 'U' */
  0x1F, 0x20, 0x40, 0x20, 0x1F,   /* 86 0x56 'V' */
  0x3F, 0x40, 0x38, 0x40, 0x3F,   /* 87 0x57 'W' */
  0x63, 0x14, 0x08, 0x14, 0x63,   /* 88 0x58 'X' */
  0x03, 0x04, 0x78, 0x04, 0x03,   /* 89 0x59 'Y' */
  0x61, 0x59, 0x49, 0x4D, 0x43,   /* 90 0x5a 'Z' */
  0x00, 0x7F, 0x41, 0x41, 0x41,   /* 91 0x5b '[' */
  0x02, 0x04, 0x08, 0x10, 0x20,   /* 92 0x5c '\' */
  0x00, 0x41, 0x41, 0x41, 0x7F,   /* 93 0x5d ']' */
  0x04, 0x02, 0x01, 0x02, 0x04,   /* 94 0x5e '^' */
  0x40, 0x40, 0x40, 0x40, 0x40,   /* 95 0x5f '_' */
  0x00, 0x03, 0x07, 0x08, 0x00,   /* 96 0x60 '`' */
  0x20, 0x54, 0x54, 0x78, 0x40,   /* 97 0x61 'a' */
  0x7F, 0x28, 0x44, 0x44, 0x38,   /* 98 0x62 'b' */
  0x38, 0x44, 0x44, 0x44, 0x28,   /* 99 0x63 'c' */
  0x38, 0x44, 0x44, 0x28, 0x7F,   /* 100 0x64 'd' */
  0x38, 0x54, 0x54, 0x54, 0x18,   /* 101 0x65 'e' */
  0x00, 0x08, 0x7E, 0x09, 0x02,   /* 102 0x66 'f' */
  0x18, 0xA4, 0xA4, 0x9C, 0x78,   /* 103 0x67 'g' */
  0x7F, 0x08, 0x04, 0x04, 0x78,   /* 104 0x68 'h' */
  0x00, 0x44, 0x7D, 0x40, 0x00,   /* 105 0x69 'i' */
  0x20, 0x40, 0x40, 0x3D, 0x00,   /* 106 0x6a 'j' */
  0x7F, 0x10, 0x28, 0x44, 0x00,   /* 107 0x6b 'k' */
  0x00, 0x41, 0x7F, 0x40, 0x00,   /* 108 0x6c 'l' */
  0x7C, 0x04, 0x78, 0x04, 0x78,   /* 109 0x6d 'm' */
  0x7C, 0x08, 0x04, 0x04, 0x78,   /* 110 0x6e 'n' */
  0x38, 0x44, 0x44, 0x44, 0x38,   /* 111 0x6f 'o' */
  0xFC, 0x18, 0x24, 0x24, 0x18,   /* 112 0x70 'p' */
  0x18, 0x24, 0x24, 0x18, 0xFC,   /* 113 0x71 'q' */
  0x7C, 0x08, 0x04, 0x04, 0x08,   /* 114 0x72 'r' */
  0x48, 0x54, 0x54, 0x54, 0x24,   /* 115 0x73 's' */
  0x04, 0x04, 0x3F, 0x44, 0x24,   /* 116 0x74 't' */
  0x3C, 0x40, 0x40, 0x20, 0x7C,   /* 117 0x75 'u' */
  0x1C, 0x20, 0x40, 0x20, 0x1C,   /* 118 0x76 'v' */
  0x3C, 0x40, 0x30, 0x40, 0x3C,   /* 119 0x77 'w' */
  0x44, 0x28, 0x10, 0x28, 0x44,   /* 120 0x78 'x' */
  0x4C, 0x90, 0x90, 0x90, 0x7C,   /* 121 0x79 'y' */
  0x44, 0x64, 0x54, 0x4C, 0x44,   /* 122 0x7a 'z' */
  0x00, 0x08, 0x36, 0x41, 0x00,   /* 123 0x7b '{' */
  0x00, 0x00, 0x77, 0x00, 0x00,   /* 124 0x7c '|' */
  0x00, 0x41, 0x36, 0x08, 0x00,   /* 125 0x7d '}' */
  0x02, 0x01, 0x02, 0x04, 0x02,   /* 126 0x7e '~' */
  0x3C, 0x26, 0x23, 0x26, 0x3C,   /* 127 0x7f '' */
  0x1E, 0xA1, 0xA1, 0x61, 0x12,
  0x3A, 0x40, 0x40, 0x20, 0x7A,
  0x38, 0x54, 0x54, 0x55, 0x59,
  0x21, 0x55, 0x55, 0x79, 0x41,
  0x21, 0x54, 0x54, 0x78, 0x41,
  0x21, 0x55, 0x54, 0x78, 0x40,
  0x20, 0x54, 0x55, 0x79, 0x40,
  0x0C, 0x1E, 0x52, 0x72, 0x12,
  0x39, 0x55, 0x55, 0x55, 0x59,
  0x39, 0x54, 0x54, 0x54, 0x59,
  0x39, 0x55, 0x54, 0x54, 0x58,
  0x00, 0x00, 0x45, 0x7C, 0x41,
  0x00, 0x02, 0x45, 0x7D, 0x42,
  0x00, 0x01, 0x45, 0x7C, 0x40,
  0xF0, 0x29, 0x24, 0x29, 0xF0,
  0xF0, 0x28, 0x25, 0x28, 0xF0,
  0x7C, 0x54, 0x55, 0x45, 0x00,
  0x20, 0x54, 0x54, 0x7C, 0x54,
  0x7C, 0x0A, 0x09, 0x7F, 0x49,
  0x32, 0x49, 0x49, 0x49, 0x32,
  0x32, 0x48, 0x48, 0x48, 0x32,
  0x32, 0x4A, 0x48, 0x48, 0x30,
  0x3A, 0x41, 0x41, 0x21, 0x7A,
  0x3A, 0x42, 0x40, 0x20, 0x78,
  0x00, 0x9D, 0xA0, 0xA0, 0x7D,
  0x39, 0x44, 0x44, 0x44, 0x39,
  0x3D, 0x40, 0x40, 0x40, 0x3D,
  0x3C, 0x24, 0xFF, 0x24, 0x24,
  0x48, 0x7E, 0x49, 0x43, 0x66,
  0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
  0xFF, 0x09, 0x29, 0xF6, 0x20,
  0xC0, 0x88, 0x7E, 0x09, 0x03,
  0x20, 0x54, 0x54, 0x79, 0x41,
  0x00, 0x00, 0x44, 0x7D, 0x41,
  0x30, 0x48, 0x48, 0x4A, 0x32,
  0x38, 0x40, 0x40, 0x22, 0x7A,
  0x00, 0x7A, 0x0A, 0x0A, 0x72,
  0x7D, 0x0D, 0x19, 0x31, 0x7D,
  0x26, 0x29, 0x29, 0x2F, 0x28,
  0x26, 0x29, 0x29, 0x29, 0x26,
  0x30, 0x48, 0x4D, 0x40, 0x20,
  0x38, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x38,
  0x2F, 0x10, 0xC8, 0xAC, 0xBA,
  0x2F, 0x10, 0x28, 0x34, 0xFA,
  0x00, 0x00, 0x7B, 0x00, 0x00,
  0x08, 0x14, 0x2A, 0x14, 0x22,
  0x22, 0x14, 0x2A, 0x14, 0x08,
  0xAA, 0x00, 0x55, 0x00, 0xAA,
  0xAA, 0x55, 0xAA, 0x55, 0xAA,
  0x00, 0x00, 0x00, 0xFF, 0x00,
  0x10, 0x10, 0x10, 0xFF, 0x00,
  0x14, 0x14, 0x14, 0xFF, 0x00,
  0x10, 0x10, 0xFF, 0x00, 0xFF,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x14, 0x14, 0x14, 0xFC, 0x00,
  0x14, 0x14, 0xF7, 0x00, 0xFF,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x14, 0x14, 0xF4, 0x04, 0xFC,
  0x14, 0x14, 0x17, 0x10, 0x1F,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0x1F, 0x00,
  0x10, 0x10, 0x10, 0xF0, 0x00,
  0x00, 0x00, 0x00, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0xF0, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x10,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0x10, 0x10, 0x10, 0xFF, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x14,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0x00, 0x1F, 0x10, 0x17,
  0x00, 0x00, 0xFC, 0x04, 0xF4,
  0x14, 0x14, 0x17, 0x10, 0x17,
  0x14, 0x14, 0xF4, 0x04, 0xF4,
  0x00, 0x00, 0xFF, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x14, 0x14, 0xF7, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x17, 0x14,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0xF4, 0x14,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x00, 0x00, 0x1F, 0x10, 0x1F,
  0x00, 0x00, 0x00, 0x1F, 0x14,
  0x00, 0x00, 0x00, 0xFC, 0x14,
  0x00, 0x00, 0xF0, 0x10, 0xF0,
  0x10, 0x10, 0xFF, 0x10, 0xFF,
  0x14, 0x14, 0x14, 0xFF, 0x14,
  0x10, 0x10, 0x10, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0xF0, 0x10,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0xFF,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x38, 0x44, 0x44, 0x38, 0x44,
  0x7C, 0x2A, 0x2A, 0x3E, 0x14,
  0x7E, 0x02, 0x02, 0x06, 0x06,
  0x02, 0x7E, 0x02, 0x7E, 0x02,
  0x63, 0x55, 0x49, 0x41, 0x63,
  0x38, 0x44, 0x44, 0x3C, 0x04,
  0x40, 0x7E, 0x20, 0x1E, 0x20,
  0x06, 0x02, 0x7E, 0x02, 0x02,
  0x99, 0xA5, 0xE7, 0xA5, 0x99,
  0x1C, 0x2A, 0x49, 0x2A, 0x1C,
  0x4C, 0x72, 0x01, 0x72, 0x4C,
  0x30, 0x4A, 0x4D, 0x4D, 0x30,
  0x30, 0x48, 0x78, 0x48, 0x30,
  0xBC, 0x62, 0x5A, 0x46, 0x3D,
  0x3E, 0x49, 0x49, 0x49, 0x00,
  0x7E, 0x01, 0x01, 0x01, 0x7E,
  0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
  0x44, 0x44, 0x5F, 0x44, 0x44,
  0x40, 0x51, 0x4A, 0x44, 0x40,
  0x40, 0x44, 0x4A, 0x51, 0x40,
  0x00, 0x00, 0xFF, 0x01, 0x03,
  0xE0, 0x80, 0xFF, 0x00, 0x00,
  0x08, 0x08, 0x6B, 0x6B, 0x08,
  0x36, 0x12, 0x36, 0x24, 0x36,
  0x06, 0x0F, 0x09, 0x0F, 0x06,
  0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x00, 0x10, 0x10, 0x00,
  0x30, 0x40, 0xFF, 0x01, 0x01,
  0x00, 0x1F, 0x01, 0x01, 0x1E,
  0x00, 0x19, 0x1D, 0x17, 0x12,
  0x00, 0x3C, 0x3C, 0x3C, 0x3C,
  0x00, 0x00, 0x00, 0x00, 0x00,
};

#endif /* PCD8544_FONT_H */

/* Change Log */
/*
 * $Log: PCD8544_font.h,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.2  2015/05/22 18:14:59  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.3  2014/06/21 18:04:16  pi
 * Interim commit
 *
 * Revision 1.2  2014/06/21 17:31:03  pi
 * Revisions now logged etc.
 *
 *
 */
