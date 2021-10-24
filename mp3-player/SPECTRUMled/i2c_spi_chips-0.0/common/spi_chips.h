/* Header file associated with SPI CHIP Structures etc.
 * $Id: spi_chips.h,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef SPI_CHIP_H
#define SPI_CHIP_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif /* NO_MAIN */

#ifndef CHIP_NAME_LEN
#define CHIP_NAME_LEN 32
#endif /* !CHIP_NAME_LEN */

#define SPI_DEV00 0
#define SPI_DEV01 1

static char SPIdev0[]   = "/dev/spidev0.0";
static char SPIdev1[]   = "/dev/spidev0.1";
static char SPIdevNul[] = "SPI /dev undefined";

/* MCP23S08 */
typedef struct
{
  char *spidev;
  int  *spifd;
  unsigned int waddr; /* Write ADDRess */
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
} mcp23S08;

#define MCP23S08_SPI_DRT 48 /* micro-seconds (500kHz bus clock, 24 clocks/3 bytes) */

unsigned int init_mcp23S08(mcp23S08 *chip,
			   unsigned int waddr,
			   char spidev,
			   char *chip_name);

void echo_mcp23S08_status(mcp23S08 chip);

/* MAX7219 */
typedef struct
{
  char *spidev;
  int  *spifd;
  unsigned int NoOp;
  unsigned int Digit0;
  unsigned int Digit1;
  unsigned int Digit2;
  unsigned int Digit3;
  unsigned int Digit4;
  unsigned int Digit5;
  unsigned int Digit6;
  unsigned int Digit7;
  unsigned int DecodeMode;
  unsigned int Intensity;
  unsigned int ScanLimit;
  unsigned int Shutdown;
  unsigned int DisplayTest;
  char name[CHIP_NAME_LEN];
} max7219;

void init_max7219(max7219 *chip,
		  char spidev,
		  char *chip_name);

void echo_max7219_status(max7219 chip);

/* MAX7221 */
typedef struct
{
  char *spidev;
  int  *spifd;
  unsigned int NoOp;
  unsigned int Digit0;
  unsigned int Digit1;
  unsigned int Digit2;
  unsigned int Digit3;
  unsigned int Digit4;
  unsigned int Digit5;
  unsigned int Digit6;
  unsigned int Digit7;
  unsigned int DecodeMode;
  unsigned int Intensity;
  unsigned int ScanLimit;
  unsigned int Shutdown;
  unsigned int DisplayTest;
  char name[CHIP_NAME_LEN];
} max7221;

void init_max7221(max7221 *chip,
		  char spidev,
		  char *chip_name);

void echo_max7221_status(max7221 chip);

/* Useful MAX7219/7221 commands ... */
/* NB: All "Don't care" bits = 0    */
#define NOOP_DATA                0x00
#define SHUTDOWN_MODE            0x00
#define NORMAL_OPERATION         0x01
#define NO_DECODE_ALL_DIGITS     0x00
#define CODE_B_DECODE_ALL_DIGITS 0xff
#define MIN_INTENSITY            0x00
#define MED_INTENSITY            0x07
#define MAX_INTENSITY            0x0f
#define SCAN_ALL_DIGITS          0x07
#define DISPLAY_NORMAL_MODE      0x00
#define DISPLAY_TEST_MODE        0x01
#define ALL_DOTS_ON              0xff /* NO_DECODE_ALL_DIGITS active!     */
#define ALL_DOTS_OFF             0x00 /* NO_DECODE_ALL_DIGITS active!     */
#define ALL_SEG_DP_OFF           0x0f /* CODE_B_DECODE_ALL_DIGITS active! */
#define ALL_SEG_DP_ON            0x88 /* CODE_B_DECODE_ALL_DIGITS active! */
#define ALL_SEG_ON_DP_OFF        0x08 /* CODE_B_DECODE_ALL_DIGITS active! */
#define ALL_SEG_OFF_DP_ON        0x8f /* CODE_B_DECODE_ALL_DIGITS active! */
#define CODE_B_HYPHEN            0x0a /* CODE_B_DECODE_ALL_DIGITS active! */
#define CODE_B_E                 0x0b /* CODE_B_DECODE_ALL_DIGITS active! */
#define CODE_B_H                 0x0c /* CODE_B_DECODE_ALL_DIGITS active! */
#define CODE_B_L                 0x0d /* CODE_B_DECODE_ALL_DIGITS active! */
#define CODE_B_P                 0x0e /* CODE_B_DECODE_ALL_DIGITS active! */
#define DP_OFFSET                0x80

/* PCD8544/Nokia5110 */
typedef struct
{
  char *spidev;
  int  *spifd;
  int rst_gpio;
  int dc_gpio;
  int bl_gpio; /* Reserved for future use */
  unsigned int dcval; /* Data or Command VALue */
  char name[CHIP_NAME_LEN];
} pcd8544;

#define GPIO_UNASSIGNED -1

void init_pcd8544(pcd8544 *chip,
		  int rst_gpio,
		  int dc_gpio,
		  int bl_gpio,
		  char spidev,
		  char *chip_name);
		  
void echo_pcd8544_status(pcd8544 chip);

/* Bit-Banged PCD8544/Nokia5110         */
/* NB: Uses wiringPi pin numbers        */
/* and requires sudo (root) priviliges! */
typedef struct
{
  int rst_wpin;
  int ce_wpin;
  int dc_wpin;
  int din_wpin;
  int clk_wpin;
  int bl_wpin; /* Reserved for future use */
  unsigned char dcval; /* Data or Command VALue */
  unsigned int speed;
  char name[CHIP_NAME_LEN];
} bbpcd8544;

#define WPIN_UNASSIGNED -1
#define PCD8544_SLOW_CLOCK   500 /* uS ==   1kHz */
#define PCD8544_MEDIUM_CLOCK  50 /* uS ==  10kHz */
#define PCD8544_FAST_CLOCK     5 /* uS == 100kHz */
#define PCD8544_VFAST_CLOCK    2 /* uS == 250kHz */
/* Contrast ... */
#define PCD8544_C0          0x2f
#define PCD8544_C1          0x33
#define PCD8544_C2          0x37
#define PCD8544_C3          0x3b
#define PCD8544_C4          0x3d
#define PCD8544_C5          0x3f
#define PCD8544_C6          0x43
#define PCD8544_C7          0x47
#define PCD8544_C8          0x4b
/* Bias (BS0 - BS2) */
#define PCD8544_B0          0x00
#define PCD8544_B1          0x01
#define PCD8544_B2          0x02
#define PCD8544_B3          0x03
#define PCD8544_B4          0x04
#define PCD8544_B5          0x05
#define PCD8544_B6          0x06
#define PCD8544_B7          0x07

void init_bbpcd8544(bbpcd8544 *chip,
		    int rst_wpin,
		    int ce_wpin,
		    int dc_wpin,
		    int din_wpin,
		    int clk_wpin,
		    int bl_wpin,
		    unsigned int speed,
		    char *chip_name);

void echo_bbpcd8544_status(bbpcd8544 chip);

/* Bit-Banged, eXported gpio, PCD8544/Nokia5110 */
/*
 * typedef struct
 * {
 *   int rst_gpio;
 *   int ce_gpio;
 *   int dc_gpio;
 *   int din_gpio;
 *   int clk_gpio;
 *   int bl_gpio; 
 *   unsigned char dcval;
 *   unsigned int speed;
 *   char name[CHIP_NAME_LEN];
 * } bbxpcd8544;
 * 
 * void init_bbxpcd8544(bbxpcd8544 *chip,
 * 		    int rst_gpio,
 * 		    int ce_gpio,
 * 		    int dc_gpio,
 * 		    int din_gpio,
 * 		    int clk_gpio,
 * 		    int bl_gpio,
 * 		    unsigned int speed,
 * 		    char *chip_name);
 * 
 * void echo_bbxpcd8544_status(bbxpcd8544 chip);
 */

/* GPIO controlled 74HC595 */
/* CE0 or CE1 --> RCK (12) */
/* MOSI       --> SER (14) */
/* SCLK       --> SCK (11) */
/* goe_gpio   --> G   (13) [Gate or ¬Output Enable]    */
/* scl_gpio   --> SCL (10) [¬Shift reg.CLear or ¬reset */
typedef struct
{
  char *spidev;
  int  *spifd;
  int goe_gpio;
  int scl_gpio;
  char name[CHIP_NAME_LEN];
} gc74hc595;

#define GC74HC595_SPI_DRT 16 /* micro-seconds (500kHz bus clock, 8 clocks/1 byte) */

void init_gc74hc595(gc74hc595 *chip,
		    int goe_gpio,
		    int scl_gpio,
		    char spidev,
		    char *chip_name);
		    
void echo_gc74hc595_status(gc74hc595 chip);

/* Hard-Wired 74HC595 */
/* CE0 or CE1 --> RCK (12) */
/* MOSI       --> SER (14) */
/* SCLK       --> SCK (11) */
/* Vss        <-- G   (13) [Gate or ¬Output Enable]    */
/* Vdd        <-- SCL (10) [¬Shift reg.CLear or ¬reset */
typedef struct
{
  char *spidev;
  int  *spifd;
  char name[CHIP_NAME_LEN];
} hw74hc595;

#define HW74HC595_SPI_DRT GC74HC595_SPI_DRT

void init_hw74hc595(hw74hc595 *chip,
		    char spidev,
		    char *chip_name);
		    
void echo_hw74hc595_status(hw74hc595 chip);

/* Hard-Wired 74HC164 */
/* Vcc  --> A1 (1) or A2 (2) */
/* MOSI --> A2 (2) or A1 (1) */
/* ie. CE0/CE1 NOT IN USE!   */
/* SCLK --> CLOCK  (8)       */
/* Vcc  --> /RESET (9)       */
typedef struct
{
  char *spidev;
  int  *spifd;
  int chainlen; /* #chips in the chain */
  char name[CHIP_NAME_LEN];
} hw74hc164;

#define HW74HC164X1_SPI_DRT GC74HC595_SPI_DRT

void init_hw74hc164(hw74hc164 *chip,
		    int chainlen,
		    char spidev,
		    char *chip_name);
		    
void echo_hw74hc164_status(hw74hc164 chip);

/* ST7920 GLCD in Serial Mode */
/* ¬CE0 or ¬CE1 --> inverter --> RS        */
/* MOSI                      --> R/¬W      */
/* SCLK                      --> E         */
/* Vss   (Hard-wired)        --> PSB(P/¬S) */
typedef struct
{
  char *spidev;
  int  *spifd;
  char name[CHIP_NAME_LEN];
} st7920sm;

void init_st7920sm(st7920sm *chip,
		   char spidev,
		   char *chip_name);

void echo_st7920sm_status(st7920sm chip);

/* SPI version of SSD1306 OLED */
typedef struct
{
  char *spidev;
  int  *spifd;
  int rst_gpio;
  int dc_gpio;
  unsigned int cmds;     /* Single-byte CoMmand store */
  unsigned int cmdd[2];  /* Double-byte CoMmand store */
  unsigned int cmdt[3];  /* Triple-byte CoMmand store */
  unsigned int cmdm[7];  /* Multi-byte CoMmand store, m (6|7) in cmdm[0] */
  unsigned int cfont[8]; /* Column-based FONT store   */
  /* Page (column bytes) | Frame Buffer store         */
  unsigned int page[8][128];
  char name[CHIP_NAME_LEN];
} spi_ssd1306_oled;

void init_spi_ssd1306_oled(spi_ssd1306_oled *chip,
			   char spidev,
			   int rst_gpio,
			   int dc_gpio,
			   char *chip_name);

void echo_spi_ssd1306_oled_status(spi_ssd1306_oled chip);

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

#endif /* !SPI_CHIP_H */

/* Change Log */
/*
 * $Log: spi_chips.h,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.3  2017/09/28 19:23:52  pi
 * Added 74HC164 (spi-like) support
 *
 * Revision 1.2  2015/11/11 23:21:37  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.15  2015/08/23 13:57:12  pi
 * st7920sm chip added
 *
 * Revision 1.14  2015/02/24 21:37:34  pi
 * Nokia 5110 Biassystem constants added
 *
 * Revision 1.13  2015/02/23 18:37:02  pi
 * Interim commit
 *
 * Revision 1.12  2014/12/30 20:37:11  pi
 * Commented out unusable code
 *
 * Revision 1.11  2014/12/30 15:02:26  pi
 * Interim commit
 *
 * Revision 1.10  2014/12/27 20:43:09  pi
 * Interim commit
 *
 * Revision 1.9  2014/12/23 19:16:20  pi
 * Interim commit
 *
 * Revision 1.8  2014/12/22 17:00:42  pi
 * Interim commit
 *
 * Revision 1.7  2014/12/21 21:54:52  pi
 * Interim commit
 *
 * Revision 1.6  2014/12/21 20:05:18  pi
 * Beginning to add bit-banged spi support for nokia glcd
 *
 * Revision 1.5  2014/11/12 22:25:14  pi
 * Hard-wired 74hc595 defined
 *
 * Revision 1.4  2014/11/08 19:45:03  pi
 * Some tidying up and improved address assignment
 *
 * Revision 1.3  2014/08/01 21:04:03  pi
 * Some time constants adjusted etc.
 *
 * Revision 1.2  2014/07/28 22:13:48  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.9  2014/07/13 20:16:21  pi
 * Starting work on SPI 8-bit LCD interface
 *
 * Revision 1.8  2014/07/09 21:33:47  pi
 * Interim commit
 *
 * Revision 1.7  2014/07/02 21:07:04  pi
 * Interim commit
 *
 * Revision 1.6  2014/06/20 20:33:01  pi
 * Interim commit
 *
 * Revision 1.5  2014/06/19 21:40:05  pi
 * Interim commit
 *
 * Revision 1.4  2014/06/11 20:29:54  pi
 * Beginning to cascade
 *
 * Revision 1.3  2014/06/03 21:02:37  pi
 * Interim commit
 *
 * Revision 1.2  2014/06/02 23:11:16  pi
 * Interim commit
 *
 * Revision 1.1  2014/06/02 20:51:02  pi
 * Initial revision
 *
 * 
 *
 */
