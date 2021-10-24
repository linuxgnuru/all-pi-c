/* Header file associated with I2C CHIP Structures etc.
 * $Id: i2c_chips.h,v 1.14 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef I2C_CHIP_H
#define I2C_CHIP_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif /* NO_MAIN */

#define ALL_GPIO_BITS_OUT 0x00
#define ALL_GPIO_BITS_IN 0xff
#define ALL_GPIO_BITS_LOW 0x00
#define ALL_GPIO_BITS_HIGH 0xff
#define ALL_GPIO_BITS_NORMAL 0x00
#define ALL_GPIO_BITS_INVERTED 0xff
#define HIGH_GPIO_NIBBLE_INVERTED 0xf0
#define LOW_GPIO_NIBBLE_INVERTED 0x0f

#define CHIP_NAME_LEN 32

/* MCP23017 */
typedef struct
{
  unsigned int addr;
  unsigned int iocon;
  unsigned int iodira;
  unsigned int iodirb;
  unsigned int ipola;
  unsigned int ipolb;
  unsigned int gpioa;
  unsigned int gpiob;
  /* Registers not yet in use >>> */
  /*
   * unsigned int gpintena;
   * unsigned int defvala;
   * unsigned int intcona;
   * unsigned int gppua;
   * unsigned int intfa;
   * unsigned int intcapa;
   * unsigned int olata;
   * unsigned int gpintenb;
   * unsigned int defvalb;
   * unsigned int intconb;
   * unsigned int gppub;
   * unsigned int intfb;
   * unsigned int intcapb;
   * unsigned int olatb;
   */
  /* <<< */
  char name[CHIP_NAME_LEN];
} mcp23017;

#define MCP23017_I2C_DRT 284 /* micro-seconds (100kHz bus clock, 28 clocks + tpv) */

unsigned int init_mcp23017(mcp23017 *chip,
			   unsigned int addr,
			   char *chip_name);

void echo_mcp23017_status(mcp23017 chip);

/* MCP23008 */
typedef struct
{
  unsigned int addr;
  unsigned int iocon;
  unsigned int iodir;
  unsigned int iopol;
  unsigned int gpio;
  /* Registers not yet in use >>> */
  /*
   * unsigned int gpinten;
   * unsigned int defval;
   * unsigned int intcon;
   * unsigned int gppu;
   * unsigned int intf;
   * unsigned int intcap;
   * unsigned int olat;
   */
  /* <<< */
  char name[CHIP_NAME_LEN];
} mcp23008;

#define MCP23008_I2C_DRT 284 /* micro-seconds (100kHz bus clock, 28 clocks + tpv) */

unsigned int init_mcp23008(mcp23008 *chip,
			   unsigned int addr,
			   char *chip_name);

void echo_mcp23008_status(mcp23008 chip);

/* PCF8574 */
typedef struct
{
  unsigned int addr;
  unsigned int qiop;
  char name[CHIP_NAME_LEN];
} pcf8574;

/* #define PCF8574_I2C_DRT 184 /* micro-seconds (100kHz bus clock, 18 clocks + tpv) */
#define PCF8574_I2C_DRT 204 /* micro-seconds (100kHz bus clock, 20 clocks + tpv) */

unsigned int init_pcf8574(pcf8574 *chip,
			  unsigned int addr,
			  char *chip_name);

void echo_pcf8574_status(pcf8574 chip);

/* LCD Backpack V2 */
typedef struct
{
  unsigned int addr;
  unsigned int printstr;
  unsigned int cursorposn;
  unsigned int clearline;
  unsigned int cleardisp;
  unsigned int lcdtype;
  unsigned int hd44780;
  unsigned int backlight;
  unsigned int writechar;
  unsigned int createchar;
  unsigned int setbaudrate;
  /* Not in use by I2C */
  /*
   * unsigned int changei2caddr;
   */
  char name[CHIP_NAME_LEN];
} lcdbpv2;

unsigned int init_lcdbpv2(lcdbpv2 *chip,
			  unsigned int addr,
			  char *chip_name);

void echo_lcdbpv2_status(lcdbpv2 chip);

/* I2C version of SSD1306 OLED */
typedef struct
{
  unsigned int addr;     /* 0x3c or 0x3d */
  unsigned int c0dc;
  unsigned int cmds;     /* Single-byte CoMmand store */
  unsigned int cmdd[2];  /* Double-byte CoMmand store */
  unsigned int cmdt[3];  /* Triple-byte CoMmand store */
  unsigned int cmdm[7];  /* Multi-byte CoMmand store, m (6|7) in cmdm[0] */
  unsigned int cfont[8]; /* Column-based FONT store   */
  /* Page (column bytes) | Frame Buffer store         */
  unsigned int page[8][128];
  char name[CHIP_NAME_LEN];
} i2c_ssd1306_oled;

#define OLED_COMMAND              0x00
#define OLED_DATA                 0x40
#define OLED_CONTRAST_RESET       0x7f
#define OLED_HORIZONTAL_ADDR_MODE 0x00
#define OLED_VERTICAL_ADDR_MODE   0x01
#define OLED_PAGE_ADDR_MODE       0x02
#define OLED_COLUMN_START_RESET   0
#define OLED_COLUMN_END_RESET     127
#define OLED_PAGE_START_RESET     0
#define OLED_PAGE_END_RESET       7
#define OLED_CHAR_WIDTH           128 /* 8x8 font */

/* Single-byte Commands ... */
#define OLED_DISPLAY_ON_WITH_RAM 0xa4 
#define OLED_DISPLAY_ON_NO_RAM   0xa5
#define OLED_DISPLAY_NORMAL      0xa6 
#define OLED_DISPLAY_INVERSE     0xa7 
#define OLED_DISPLAY_OFF         0xae
#define OLED_DISPLAY_ON          0xaf
#define OLED_SET_SEG0_0          0xa0
#define OLED_SET_SEG0_127        0xa1 /* Re-map */
#define OLED_SET_COMO_NORMAL     0xC0
#define OLED_SET_COMO_REMAP      0xC8

/* Double-byte Command prefixes ... */
#define OLED_SET_CONTRAST        0x81
#define OLED_SET_MEM_ADDR_MODE   0x20

/* Triple-byte Command prefixes ... */
#define OLED_SET_COLUMN_ADDR     0x21
#define OLED_SET_PAGE_ADDR       0x22

/* Multi-byte Command prefixes ... */

/* Contrast ... */
#define OLED_C0 0x0f
#define OLED_C1 0x1f
#define OLED_C2 0x3f
#define OLED_C3 0x5f
#define OLED_C4 OLED_CONTRAST_RESET
#define OLED_C5 0x9f
#define OLED_C6 0xbf
#define OLED_C7 0xdf
#define OLED_C8 0xef

unsigned init_i2c_ssd1306_oled(i2c_ssd1306_oled *chip,
			       unsigned int addr,
			       char *chip_name);

void echo_i2c_ssd1306_oled_status(i2c_ssd1306_oled chip);

/* HT16K33 */
#define DDAP_ADDR   0x00
#define SSR_STANDBY 0x20 /* STANDBY mode (System Oscillator Off) */
#define SSR_NORMAL  0x21 /* NORMAL mode  (System Oscillator On)  */
#define RISR_ADDR   0xa0
#define RISR_ROWD   0xa0 /* ROW Driver output mode    */
#define RISR_INTL   0xa1 /* ROW INTerrupt active low  */
#define RISR_INTH   0xa3 /* ROW INTerrupt active high */
#define DSPR_ADDR   0x80 
#define DMSR_ADDR   0xef
#define DISPOFFMSK  0x80 /* & */  
#define DISPONMSK   0x01 /* | */    
#define DSPRBLNKMSK 0x05
#define BLINKOFF    0x89 /* & */
#define BLINK20HZ   0x8b /* | */
#define BLINK10HZ   0x8c /* | */
#define BLINK05HZ   0x8e /* | */    
#define DIMSETMASK  0xe0
#define DIM01_16    0x00 /* | */
#define DIM02_16    0x01 /* | */
#define DIM03_16    0x02 /* | */
#define DIM04_16    0x03 /* | */
#define DIM05_16    0x04 /* | */
#define DIM06_16    0x05 /* | */
#define DIM07_16    0x06 /* | */
#define DIM08_16    0x07 /* | */
#define DIM09_16    0x08 /* | */
#define DIM10_16    0x09 /* | */
#define DIM11_16    0x0a /* | */
#define DIM12_16    0x0b /* | */
#define DIM13_16    0x0c /* | */
#define DIM14_16    0x0d /* | */
#define DIM15_16    0x0e /* | */
#define DIM16_16    0x0f /* | */
/* Configurations ... */
#define MTX_8X8     0
#define MTX_16X8    1
#define C4DGTRGB    2

/* Adafruit 8x8 LED Matrix layout ...     */
/* ROW0 at "top", below I2C connector ... */
#define ROW0ADDR    0x00
#define ROW1ADDR    0x02
#define ROW2ADDR    0x04
#define ROW3ADDR    0x06
#define ROW4ADDR    0x08
#define ROW5ADDR    0x0a
#define ROW6ADDR    0x0c
#define ROW7ADDR    0x0e
/* COL0 at left edge ... */
#define COL0MASK    0x80 /* | */
#define COL1MASK    0x01 /* | */
#define COL2MASK    0x02 /* | */
#define COL3MASK    0x04 /* | */
#define COL4MASK    0x08 /* | */
#define COL5MASK    0x10 /* | */
#define COL6MASK    0x20 /* | */
#define COL7MASK    0x40 /* | */
#define ALLCOLON    0xff      
#define ALLCOLOFF   0x00

/* Green "Clock Style" 4-Digit LED Display + RGB LED */
/* NB: Using HT16K33 notation cf. a matrix layout    */
/* Digit & RGB Columns   */
/* (Digits use Rows 0-7) */
#define DGT1ADDR    0x00 
#define DGT2ADDR    0x02
#define DGT3ADDR    0x04
#define DGT4ADDR    0x06
/* (RGB uses Rows 8-10, order BGR!) */
#define RGBADDR     0x09
/* Segment Masks         */
#define SEGAMASK    0x01 /* | */
#define SEGBMASK    0x02 /* | */
#define SEGCMASK    0x04 /* | */
#define SEGDMASK    0x08 /* | */
#define SEGEMASK    0x10 /* | */
#define SEGFMASK    0x20 /* | */
#define SEGGMASK    0x40 /* | */
#define SEGDPMASK   0x80 /* | */
/* DP only available for digits 2 & 3 */
#define ALLSEGON    0xff
#define ALLSEGOFF   0x00
#define ALLRGBON    0x07
#define ALLRGBOFF   0x00
/* RGB Masks (order BGR!) */
#define BLUEON      0x04 /* | */
#define GREENON     0x02 /* | */
#define REDON       0x01 /* | */

typedef struct
{
  unsigned int addr;
  unsigned int conf;
  unsigned int ddap; /* Display Data Address Pointer */  
  unsigned int ddcb; /* Display Data Column Byte     */
  unsigned int ssr;  /* System Setup Register        */
  unsigned int risr; /* Row/Int Set Register         */
  unsigned int dspr; /* Display SetuP Register       */
  unsigned int dmsr; /* DiMming Set Register         */
  /* Registers not in use... */
  /*
   * unsigned int kdap; Key Data Address Pointer
   * unsigned int ifap; Int Flag Address Pointer
   */
  /* 8x8 LED Matrix data etc. */
  unsigned int crfont[8]; /* Column-based Raw FONT store       */
  unsigned int ccfont[8]; /* Column-based Converted FONT store */
  /* Clock LED Display & RGB LED data etc. */
  unsigned int digits[4]; /* NB: 7-seg CC encoded digits */
  unsigned int rgb;
  char name[CHIP_NAME_LEN];
} ht16k33;

unsigned init_ht16k33(ht16k33 *chip,
		      unsigned int addr,
		      unsigned int conf,
		      char *chip_name);

void echo_ht16k33_status(ht16k33 chip);

void convert_ht16k33_font(ht16k33 *chip);

void load_ht16k33_crfont(ht16k33 *chip,
			 char *thisfont,
			 unsigned int fidx,
			 int nrow);

void load_ht16k33_digits(ht16k33 *chip,
			 unsigned int digits[4]);

void load_ht16k33_rgb(ht16k33 *chip,
		      unsigned int rgb);

#endif /* !I2C_CHIP_H */

/* Change Log */
/*
 * $Log: i2c_chips.h,v $
 * Revision 1.14  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.13  2017/12/14 21:45:27  pi
 * Interim commit
 *
 * Revision 1.12  2017/12/13 22:46:54  pi
 * Interim commit
 *
 * Revision 1.11  2017/12/11 22:13:14  pi
 * Interim commit
 *
 * Revision 1.10  2017/08/08 19:11:31  pi
 * Interim commit
 *
 * Revision 1.9  2017/07/19 18:08:31  pi
 * Interim Commit
 *
 * Revision 1.8  2017/07/18 15:41:35  pi
 * Interim Commit
 *
 * Revision 1.7  2017/07/17 17:24:14  pi
 * Interim Commit
 *
 * Revision 1.6  2017/07/17 16:15:44  pi
 * Interim Commit
 *
 * Revision 1.5  2017/07/17 14:51:11  pi
 * Interim commit
 *
 * Revision 1.4  2017/07/15 20:15:47  pi
 * Interim commit
 *
 * Revision 1.3  2017/07/14 21:02:01  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/14 20:07:04  pi
 * HT16K33 chip added
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.9  2015/08/26 19:58:09  pi
 * Added OLED contrast #defines etc.
 *
 * Revision 1.8  2015/05/26 20:49:10  pi
 * Interim commit
 *
 * Revision 1.7  2015/04/17 21:10:43  pi
 * Interim commit
 *
 * Revision 1.6  2015/04/16 16:44:58  pi
 * SSD1306 oled "chip" added
 *
 * Revision 1.5  2014/11/08 19:45:03  pi
 * Some tidying up and improved address assignment
 *
 * Revision 1.4  2014/10/28 18:47:24  pi
 * Interim commit
 *
 * Revision 1.3  2014/10/11 21:50:50  pi
 * Revised strobe timings & mechanism
 *
 * Revision 1.2  2014/08/01 21:04:03  pi
 * Some time constants adjusted etc.
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.3  2014/03/28 22:45:46  pi
 * Interim commit
 *
 * Revision 1.2  2014/03/01 18:21:13  pi
 * New routines etc. for LCD backpack device added
 *
 * Revision 1.1  2013/06/03 20:58:09  pi
 * Initial revision
 *
 * Revision 1.3  2013/05/25 22:43:42  pi
 * Added skeleton code for other MCP23008/23017 registers
 *
 * Revision 1.2  2013/05/15 16:11:20  pi
 * Interim commit
 *
 * Revision 1.1  2013/05/15 15:08:57  pi
 * Initial revision
 *
 *
 *
 *
 */
