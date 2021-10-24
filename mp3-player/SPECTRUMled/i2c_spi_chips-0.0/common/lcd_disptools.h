/* Header file associated with LCD DISPlay TOOLS structures etc.
 * $Id: lcd_disptools.h,v 1.14 2018/04/13 16:26:25 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef LCD_DISPTOOLS_H
#define LCD_DISPTOOLS_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif /* NO_MAIN */

#ifndef I2C_CHIP_H
#include "i2c_chips.h"
#endif /* !I2C_CHIP_H */
#ifndef SPI_CHIP_H
#include "spi_chips.h"
#endif /* !SPI_CHIP_H */

/* Parallel 8-bit interface with GPIO RS & E          */
/* HD44780U LCD controller/driver commands & masks    */
/* NB: 8-bit mode & "Don't care" bits ALWAYS set High */
#define LCD_CLEAR_DISPLAY           0x01
#define LCD_RETURN_HOME             0x03
#define LCD_CURSOR_UP_NO_SHIFT      0x06
#define LCD_CURSOR_UP_SHIFTED       0x07
#define LCD_CURSOR_DOWN_NO_SHIFT    0x04
#define LCD_CURSOR_DOWN_SHIFTED     0x05
#define LCD_DISPLAY_OFF             0x08
#define LCD_DISPLAY_ON_NO_CURSOR    0x0C
#define LCD_DISPLAY_ON_CURSOR       0x0E
#define LCD_DISPLAY_ON_CURSOR_BLINK 0x0F
#define LCD_CURSOR_SHIFT_RIGHT      0x17
#define LCD_CURSOR_SHIFT_LEFT       0x13
#define LCD_DISPLAY_SHIFT_RIGHT     0x1F
#define LCD_DISPLAY_SHIFT_LEFT      0x1B
#define LCD_SET_CGRAM               0x40
#define LCD_SET_DDRAM               0x80
#define LCD_CGRAM_ADDR_MASK         0x3F
#define LCD_DDRAM_ADDR_MASK         0x7F
#define LCD_FUNC_SET_8BIT_INIT      0x3F /* Only used during INITialisation */
#define LCD_FUNC_SET_8BIT_NF18      0x33 /* N = 1, F = 5x8  */
#define LCD_FUNC_SET_8BIT_NF1A      0x37 /* N = 1, F = 5x10 */
#define LCD_FUNC_SET_8BIT_NF28      0x3B /* N = 2, F = 5x8  */
#define LCD_DDRAM_SIZE              80
#define LCD_DDRAM_ADDR_L1           00
#define LCD_DDRAM_ADDR_L2           40
#define LCD_DDRAM_ADDR_1604_L3      16
#define LCD_DDRAM_ADDR_1604_L4      56
#define LCD_DDRAM_ADDR_2004_L3      20
#define LCD_DDRAM_ADDR_2004_L4      40

/* Funduino-style Parallel 4-bit interface (D7-D4) ... */
/* with control lines on lower 4-bits (D3-0) ...       */
/* D3 == Backlight (Active High)                       */
/* D2 == E                                             */
/* D1 == RW (Always Low for write only)                */
/* D0 == RS                                            */
/* HD44780U LCD controller/driver commands & masks     */
/* NB: 4-bit mode & "Don't care" bits ALWAYS set Low   */
#define LCD_CLEAR_DISPLAY_H           0x00
#define LCD_CLEAR_DISPLAY_L           0x01
#define LCD_RETURN_HOME_H             0x00
#define LCD_RETURN_HOME_L             0x02
#define LCD_CURSOR_UP_NO_SHIFT_H      0x00
#define LCD_CURSOR_UP_NO_SHIFT_L      0x06
#define LCD_CURSOR_UP_SHIFTED_H       0x00
#define LCD_CURSOR_UP_SHIFTED_L       0x07
#define LCD_CURSOR_DOWN_NO_SHIFT_H    0x00
#define LCD_CURSOR_DOWN_NO_SHIFT_L    0x04
#define LCD_CURSOR_DOWN_SHIFTED_H     0x00
#define LCD_CURSOR_DOWN_SHIFTED_L     0x05
#define LCD_DISPLAY_OFF_H             0x00
#define LCD_DISPLAY_OFF_L             0x08
#define LCD_DISPLAY_ON_NO_CURSOR_H    0x00
#define LCD_DISPLAY_ON_NO_CURSOR_L    0x0C
#define LCD_DISPLAY_ON_CURSOR_H       0x00
#define LCD_DISPLAY_ON_CURSOR_L       0x0E
#define LCD_DISPLAY_ON_CURSOR_BLINK_H 0x00
#define LCD_DISPLAY_ON_CURSOR_BLINK_L 0x0F
#define LCD_CURSOR_SHIFT_RIGHT_H      0x01
#define LCD_CURSOR_SHIFT_RIGHT_L      0x04
#define LCD_CURSOR_SHIFT_LEFT_H       0x01
#define LCD_CURSOR_SHIFT_LEFT_L       0x00
#define LCD_DISPLAY_SHIFT_RIGHT_H     0x01
#define LCD_DISPLAY_SHIFT_RIGHT_L     0x0C
#define LCD_DISPLAY_SHIFT_LEFT_H      0x01
#define LCD_DISPLAY_SHIFT_LEFT_L      0x08
#define LCD_SET_CGRAM_H               0x04
#define LCD_SET_DDRAM_H               0x08
#define LCD_FUNC_SET_4BIT_INIT        0x03 /* Dummy 8-bit used during INITialisation */
#define LCD_FUNC_SET_4BIT_NF18_H      0x02 /* N = 1, F = 5x8  */
#define LCD_FUNC_SET_4BIT_NF18_L      0x00 /* N = 1, F = 5x8  */
#define LCD_FUNC_SET_4BIT_NF1A_H      0x02 /* N = 1, F = 5x10 */
#define LCD_FUNC_SET_4BIT_NF1A_L      0x04 /* N = 1, F = 5x10 */
#define LCD_FUNC_SET_4BIT_NF28_H      0x02 /* N = 2, F = 5x8  */
#define LCD_FUNC_SET_4BIT_NF28_L      0x08 /* N = 2, F = 5x8  */
#define LCD_FI2C_PRELOAD              0x08
/* 4-bit data Low, BackLight On, E Low, RW Low, RS Low   */ 
#define LCD_FI2C_STANDBY_H            0xF9
/* 4-bit data High, BackLight On, E Low, RW Low, RS High */ 
#define LCD_FI2C_STANDBY_L            0x01
/* 4-bit data Low, BackLight Off, E Low, RW Low, RS High */ 
/* "and" masks */
#define LCD_FI2C_ESMASK_L             0xFB
/* BackLight oFf MaSK Low ... */
#define LCD_FI2C_BLFMSK_L             0xF7
/* "or" masks */
#define LCD_FI2C_ESMASK_H             0x04
/* BackLight oN MaSK High ... */
#define LCD_FI2C_BLNMSK_H             0x08
/* "additive" nibble "masks" */
#define LCD_FI2C_RMASK                0x08
/* 4-bit data Low, BackLight On, E Low, RW Low, RS Low   */ 
#define LCD_FI2C_DMASK                0x09
/* 4-bit data Low, BackLight On, E Low, RW Low, RS High  */

/* Similar Parallel 4-bit interface based upon (port B of)
 * a MCP23017. Data GPB4-7 --> D4-D7.
 * Control GPB0-3:
 * GPB0 --> ¬BackLightA (GPA0-7 used for an 8-bit // interface)
 * GPB1 --> ¬RegisterStrobeB (echoed to RPi GPIO #22)
 * GPB2 --> EnableB (echoed to RPi GPIO #18)
 * GPB3 --> ¬BackLightB
 */
/* "Standby mask" */
#define DUAL_LCD_B4_STANDBY 0x00 /* 4-bit data Low, BLA ON, RSB LOW,  EB LOW, BLB ON */
/* "and" masks: */
#define DUAL_LCD_BLA_ON   0xfe
#define DUAL_LCD_RSB_LOW  0xfd
#define DUAL_LCD_EB_LOW   0xfb
#define DUAL_LCD_BLB_ON   0xf7
#define DUAL_LCD_BLAB_ON  0xf6
/* "or" masks: */
#define DUAL_LCD_BLA_OFF  0x01
#define DUAL_LCD_RSB_HIGH 0x02
#define DUAL_LCD_EB_HIGH  0x04
#define DUAL_LCD_BLB_OFF  0x08
#define DUAL_LCD_BLAB_OFF 0x09
/* "additive" nibble "masks" */
#define DUAL_LCD_B4_RMASK 0x01 /* 4-bit data Low, BLA OFF, RSB LOW,  EB LOW, BLB ON */
#define DUAL_LCD_B4_DMASK 0x03 /* 4-bit data Low, BLA OFF, RSB HIGH, EB LOW, BLB ON */

/* Another Parallel 4-bit interface based upon 
 * either a MCP23S08 or a 74hc595. 
 * Data GP4-7 (QE - QH) --> D4-D7.
 * Control GP0-3 (QA - QD):
 * GP0 (QA) --> ¬RegisterStrobe
 * GP1 (QB) --> Enable (echoed to RPi GPIO #22)
 * GP2 (QC) --> ¬BackLight0
 * GP3 (QD) --> ¬BackLight1
 */
/* "and" masks: */
#define SPI4BIT_LCD_RS_LOW      0xfe
#define SPI4BIT_LCD_E_LOW       0xfd
#define SPI4BIT_LCD_BL_LVL1_ON  0xfb
#define SPI4BIT_LCD_BL_LVL2_ON  0xf7
#define SPI4BIT_LCD_BL_LVL3_ON  0xf3
/* "or" masks: */
#define SPI4BIT_LCD_RS_HIGH     0x01
#define SPI4BIT_LCD_E_HIGH      0x02
#define SPI4BIT_LCD_BL_LVL2_OFF 0x08
#define SPI4BIT_LCD_BL_LVL1_OFF 0x04
#define SPI4BIT_LCD_BL_LVL0_OFF 0x0c
/* "additive" nibble "masks" */
#define SPI4BIT_LCD_RMASK 0x04 /* 4-bit data Low, BL LVL2, RS LOW,  E LOW */
#define SPI4BIT_LCD_DMASK 0x05 /* 4-bit data Low, BL LVL2, RS HIGH, E LOW */

/* Mjkdz-style Parallel 4-bit interface (D7-D4 <==> P3-P0) ... */
/* with control lines on upper 4-bits (P7-P4) ...      */
/* P7 == Backlight (Active Low)                        */
/* P6 == RS                                            */
/* P5 == RW (Always Low for write only)                */
/* P4 == E                                             */
/* HD44780U LCD controller/driver commands & masks     */
#define LCD_MI2C_PRELOAD              0x00
/* 4-bit data Low, BackLight On, E Low, RW Low, RS Low   */ 
#define LCD_MI2C_STANDBY_H            0x4F
/* 4-bit data High, BackLight On, E Low, RW Low, RS High */ 
#define LCD_MI2C_STANDBY_L            0xC0
/* 4-bit data Low, BackLight Off, E Low, RW Low, RS High */ 
/* "and" masks */
/* Enable Signal MASK Low ... */
#define LCD_MI2C_ESMASK_L             0xEF
/* BackLight oN MaSK Low ... */
#define LCD_MI2C_BLNMSK_L             0x7F
/* "or" masks */
/* Enable Signal MASK High ... */
#define LCD_MI2C_ESMASK_H             0x10
/* BackLight oFf MaSK High ... */
#define LCD_MI2C_BLFMSK_H             0x80
/* "additive" nibble "masks" */
#define LCD_MI2C_RMASK                0x00
/* 4-bit data Low, BackLight On, E Low, RW Low, RS Low   */ 
#define LCD_MI2C_DMASK                0x40
/* 4-bit data Low, BackLight On, E Low, RW Low, RS High  */

/* PCF8574-based 4-bit interface with "Inverted" Bit-Order ie.: */
/* P3 == D4       */
/* P2 == D5       */
/* P1 == D6       */
/* P0 == D7       */
/* P4 == RS       */
/* P5 == E        */
/* P6 == Spare #1 */
/* P7 == Spare #2 */
/* HD44780U LCD controller/driver commands & masks   */
#define LCD_IBO_I2C_PRELOAD           0x00
/* 4-bit data Low, E Low, RS Low, S1 Low, S2 Low     */
#define LCD_IBO_I2C_STANDBY_H         0xDF
/* 4-bit data High, E Low, RS High, S1 High, S2 High */
#define LCD_IBO_I2C_STANDBY_L         0xD0
/* 4-bit data Low, E Low, RS High, S1 High, S2 High  */
/* "and" masks */
#define LCD_IBO_I2C_ESMASK_L          0xDF
/* "or" masks */
#define LCD_IBO_I2C_ESMASK_H          0x20
/* "additive" nibble "masks" */
#define LCD_IBO_I2C_RMASK             0x00
/* 4-bit data Low, E Low, RS Low, S1 Low, S2 Low     */
#define LCD_IBO_I2C_DMASK             0x10
/* 4-bit data Low, E Low, RS High, S1 Low, S2 Low    */
/* Default Addresses ... */
#define IBO_PCF8574P                  0x22
#define IBO_PCF8574AP                 0x3A

typedef enum
{
  LCM0802,
  LCM1602,
  LCM1604,
  LCM2004,
  LAST_LCM,
  LCMTYPES
} lcmclass;

typedef struct
{
  int pixel_row0;
  int pixel_row1;
  int pixel_row2;
  int pixel_row3;
  int pixel_row4;
  int pixel_row5;
  int pixel_row6;
  int pixel_row7;
} lcd_5x8_pixel; 

#define MAX_LCD_NL 4

typedef struct
{
  lcmclass lcd_type;
  int lcd_nc;
  int lcd_nl;
  int lcd_ddram_idx[MAX_LCD_NL];
  int lcd_rs_gpio;
  int lcd_e_gpio;
  pcf8574 lcd_data;
  int lcd_ehmin; /* Enable High MINimum time, uS  */
  int lcd_elmin; /* Enable Low MINimum time^^, uS */
  int lcd_elmax; /* Enable Low MAXimum time^^, uS */
  int lcd_wait;  /* WAIT time, uS                 */ 
  int lcd_nfb;
  int lcd_bsize;
  char lcd_buffer[LCD_DDRAM_SIZE+1];
} lcd_iface_8bit;
/* ^^IE. internal delay before next operation, typically ~41uS|1600uS */

#define HD44780U_MIN_OP_TIME   41
#define HD44780U_MAX_OP_TIME 1600 /* For LCD_RETURN_HOME(_L) */

typedef struct
{
  lcmclass lcd_type;
  int lcd_nc;
  int lcd_nl;
  int lcd_ddram_idx[MAX_LCD_NL];
  pcf8574 lcd_data;
  int lcd_ehmin; /* Enable High MINimum time, uS  */
  int lcd_elmin; /* Enable Low MINimum time^^, uS */
  int lcd_elmax; /* Enable Low MAXimum time^^, uS */
  int lcd_wait;  /* WAIT time, uS                 */ 
  int lcd_nfb;
  int lcd_bsize;
  char lcd_buffer[LCD_DDRAM_SIZE+1];
} lcd_fi2c_4bit;
/* ^^IE. internal delay before next operation, typically ~41uS|1600uS */

typedef struct
{
  lcmclass lcd_type;
  int lcd_nc;
  int lcd_nl;
  int lcd_ddram_idx[MAX_LCD_NL];
  int lcd_ehmin; /* Enable High MINimum time, uS  */
  int lcd_elmin; /* Enable Low MINimum time^^, uS */
  int lcd_elmax; /* Enable Low MAXimum time^^, uS */
  int lcd_wait;  /* WAIT time, uS                 */ 
  int lcd_nfb;
  int lcd_bsize;
  char lcd_buffer[LCD_DDRAM_SIZE+1];
} lcd_core;

typedef struct
{
  pcf8574 lcd_data;
  lcd_core M4bit;
  unsigned int lcd_M4data;
} lcd_mi2c_4bit;

typedef struct
{
  pcf8574 lcd_data;
  lcd_core IBO4bit;
  unsigned int lcd_IBO4data;
} lcd_ibo_i2c_4bit;

/* Another Parallel 4-bit interface based upon 
 * direct GPIO connections (similar to wiringPi). 
 * Data GPIO4-7 --> D4-D7.
 * Control GPIO0-3:
 * GPIO0 --> ¬RegisterStrobe
 * GPIO1 --> Enable
 * GPIO2 --> ¬BackLight0
 * GPIO3 --> ¬BackLight1
 */
/* "and" masks: */
#define GPIO4BIT_LCD_RS_LOW      0xfe
#define GPIO4BIT_LCD_E_LOW       0xfd
#define GPIO4BIT_LCD_BL_LVL1_ON  0xfb
#define GPIO4BIT_LCD_BL_LVL2_ON  0xf7
#define GPIO4BIT_LCD_BL_LVL3_ON  0xf3
/* "or" masks: */
#define GPIO4BIT_LCD_RS_HIGH     0x01
#define GPIO4BIT_LCD_E_HIGH      0x02
#define GPIO4BIT_LCD_BL_LVL2_OFF 0x08
#define GPIO4BIT_LCD_BL_LVL1_OFF 0x04
#define GPIO4BIT_LCD_BL_LVL0_OFF 0x0c
/* "additive" nibble "masks" */
#define GPIO4BIT_LCD_RMASK 0x04 /* 4-bit data Low, BL LVL2, RS LOW,  E LOW */
#define GPIO4BIT_LCD_DMASK 0x05 /* 4-bit data Low, BL LVL2, RS HIGH, E LOW */

/* Version 2 of another Parallel 4-bit interface based upon 
 * direct GPIO connections (similar to wiringPi). 
 * Data GPIO0-3 --> D4-D7.
 * Control GPIO4 & GPIO7:
 * GPIO4 --> Enable
 * GPIO7 --> ¬RegisterStrobe
 */
/* "and" masks: */
#define GPIO4BITV2_LCD_RS_LOW      0x3f
#define GPIO4BITV2_LCD_E_LOW       0xef
/* "or" masks: */
#define GPIO4BITV2_LCD_RS_HIGH     0x80
#define GPIO4BITV2_LCD_E_HIGH      0x10
/* "additive" nibble "masks" */
#define GPIO4BITV2_LCD_RMASK 0x00 /* 4-bit data Low, RS LOW,  E LOW */
#define GPIO4BITV2_LCD_DMASK 0x80 /* 4-bit data Low, RS HIGH, E LOW */

#ifndef GPIO_IFACE_NAME_LEN
#define GPIO_IFACE_NAME_LEN 32
#endif /* !GPIO_IFACE_NAME_LEN */

typedef struct
{
  int rs_gpio;
  int e_gpio;
  int bl0_gpio;
  int bl1_gpio;
  int d4_gpio;
  int d5_gpio;
  int d6_gpio;
  int d7_gpio;
  char name[GPIO_IFACE_NAME_LEN];
} gpio_4bit4;

typedef struct
{
  gpio_4bit4 lcd_condat;
  lcd_core G4bit;
  unsigned int lcd_G4data;
} lcd_gpio_4bit;

typedef struct
{
  int rs_gpio;
  int e_gpio;
  int d4_gpio;
  int d5_gpio;
  int d6_gpio;
  int d7_gpio;
  char name[GPIO_IFACE_NAME_LEN];
} gpio_4bit4V2;

typedef struct
{
  gpio_4bit4V2 lcd_condat;
  lcd_core G4bit;
  unsigned int lcd_G4data;
} lcd_gpio_4bitV2;

typedef struct
{
  mcp23017 lcd_data;
  int lcd_rs_gpioA;
  int lcd_e_gpioA;
  int lcd_rs_gpioB;
  int lcd_e_gpioB;
  lcd_core A8bit;
  lcd_core B4bit;
  unsigned int lcd_B4data;
} dual_lcd;

typedef struct
{
  mcp23S08 lcd_data;
  int lcd_e_gpio;
  lcd_core S4bit;
  unsigned int lcd_S4data;
} lcd_spi_4bit;

typedef struct
{
  hw74hc595 lcd_data;
  int lcd_e_gpio;
  lcd_core S4bit;
  unsigned int lcd_S4data;
} lcd_spi_4bitV2;

typedef struct
{
  lcmclass lcd_type;
  int lcd_nc;
  int lcd_nl;
  int lcd_ddram_idx[MAX_LCD_NL];
  int lcd_ctrl;
  int lcd_rs_gpio;
  int lcd_e_gpio;
  gc74hc595 lcd_data;
  int lcd_ehmin; /* Enable High MINimum time, uS  */
  int lcd_elmin; /* Enable Low MINimum time^^, uS */
  int lcd_elmax; /* Enable Low MAXimum time^^, uS */
  int lcd_wait;  /* WAIT time, uS                 */ 
  int lcd_nfb;
  int lcd_bsize;
  char lcd_buffer[LCD_DDRAM_SIZE+1];
} lcd_spi_8bit;
/* ^^IE. internal delay before next operation, typically ~41uS|1600uS */

/* Parallel 8-bit interface with GPIO RS, E & BL(PWM)       */
/* QC12864B GLCD, ST7920 controller/driver commands & masks */
/* NB: 8-bit mode & "Don't care" bits ALWAYS set Low        */

/* Basic instruction set ... */
#define GLCD_CLEAR                   0x01
#define GLCD_HOME                    0x02
#define GLCD_CURSOR_UP_NO_SHIFT      0x06
#define GLCD_CURSOR_UP_SHIFTED       0x07
#define GLCD_CURSOR_DOWN_NO_SHIFT    0x04
#define GLCD_CURSOR_DOWN_SHIFTED     0x05
#define GLCD_DISPLAY_OFF             0x08
#define GLCD_DISPLAY_ON_NO_CURSOR    0x0C
#define GLCD_DISPLAY_ON_CURSOR       0x0E
#define GLCD_DISPLAY_ON_CURSOR_BLINK 0x0F
#define GLCD_CURSOR_SHIFT_RIGHT      0x14
#define GLCD_CURSOR_SHIFT_LEFT       0x10
#define GLCD_DISPLAY_SHIFT_RIGHT     0x1C
#define GLCD_DISPLAY_SHIFT_LEFT      0x18
#define GLCD_FUNC_SET_8BIT_BASIC     0x30
#define GLCD_FUNC_SET_8BIT_EXTENDED  0x34 /* Switch to ... */
#define GLCD_SET_CGRAM_0000          0x40
#define GLCD_SET_CGRAM_0002          0x50
#define GLCD_SET_CGRAM_0004          0x60
#define GLCD_SET_CGRAM_0006          0x70
#define GLCD_SET_DDRAM               0x80
#define GLCD_DDRAM_ADDR_L1           0x80
#define GLCD_DDRAM_ADDR_L2           0x90
#define GLCD_DDRAM_ADDR_L3           0x80 /* + 8 * cursor shift right */
#define GLCD_DDRAM_ADDR_L4           0x90 /* + 8 * cursor shift right */
#define GLCD_DDRAM_ADDR_MASK         0x7F

/* Extended instruction set ... */
#define GLCDX_STAND_BY               0x01
#define GLCDX_IRAM_ADDR_SET          0x02 /* a */
#define GLCDX_VSCROLL_ADDR_SET       0x03 /* b */
#define GLCDX_REVERSE_L1             0x04
#define GLCDX_REVERSE_L2             0x05
#define GLCDX_REVERSE_L3             0x06
#define GLCDX_REVERSE_L4             0x07
#define GLCDX_FUNC_SET_8BIT_GD_ON    0x36
#define GLCDX_FUNC_SET_8BIT_GD_OFF   0x34
#define GLCDX_SET_IRAM_VSCROLL       0x40 /* After a or b ? */
#define GLCDX_IRAM_ADDR_MASK         0x0f
#define GLCDX_VSCROLL_ADDR_MASK      0x3f
#define GLCDX_SET_GRAM_ADDR          0x80

typedef enum
{
  HC8X16, /* GB Half-Character font      */ 
  C16X16, /* GB 16x16 symbol font        */
  G16X16, /* user-defined 16x16 Graphics */
  I16X16, /* chinese 16x16 Iconic font   */
  GLFTYPES
} glcdfonttype;

#define GLCD_NL 4

typedef struct
{
  int glcd_nc;
  int glcd_nl;
  int glcd_ddram_idx[GLCD_NL];
  int glcd_rs_gpio;
  int glcd_e_gpio;
  int glcd_bl_gpio;
  gc74hc595 glcd_data;
  int glcd_ehmin; /* Enable High MINimum time, uS  */
  int glcd_elmin; /* Enable Low MINimum time^^, uS */
  int glcd_elmax; /* Enable Low MAXimum time^^, uS */
  int glcd_wait;  /* WAIT time, uS                 */ 
} glcd_spi2p_8bit;
/* ^^IE. internal delay before next operation, typically ~72uS|1600 */

#define ST7920_MIN_OP_TIME 72
/* #define ST7920_MIN_OP_TIME 79 /* 10% tolerance added */
/* #define ST7920_MIN_OP_TIME   86  /* 20% tolerance added */
#define ST7920_MAX_OP_TIME 1600  /* For GLCD_HOME       */

/* Serial GLCD interface with two BackLight GPIOs           */
/* Main ST7920 controller/driver commands & masks as above. */
#define ST7920_SYNCH_READ_CMD   0xfc
#define ST7920_SYNCH_READ_DATA  0xfe
#define ST7920_SYNCH_WRITE_CMD  0xf8
#define ST7920_SYNCH_WRITE_DATA 0xfb
#define ST7920_GLCD_MODULE_NC      4 /* GLCD_MODULE Name Count */

typedef struct
{
  int glcd_nc;
  int glcd_nl;
  int glcd_bl0_gpio;
  int glcd_bl1_gpio;
  int glcd_ddram_idx[GLCD_NL];
  st7920sm glcd_data;
  unsigned int glcd_synch;
  unsigned int glcd_datah;
  unsigned int glcd_datal;
} glcd_spibl2;

/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
#ifndef NO_MAIN
char *glcdnamelist[ST7920_GLCD_MODULE_NC];
int current_glcd_nli;
lcmclass lcdmodel;
glcdfonttype glcdfont;
lcd_5x8_pixel hd44780u;
lcd_iface_8bit current_iface_8bit;
lcd_iface_8bit blue_1602;
lcd_iface_8bit blue_2004;
lcd_iface_8bit yellow_1602;
lcd_fi2c_4bit current_fi2c_4bit;
lcd_fi2c_4bit fun_yellow_1602;
lcd_mi2c_4bit current_mi2c_4bit;
lcd_ibo_i2c_4bit current_ibo_i2c_4bit;
dual_lcd current_dual_lcd;
lcd_gpio_4bit current_gpio_4bit;
lcd_gpio_4bitV2 current_gpio_4bitV2;
lcd_spi_4bit current_spi_4bit;
lcd_spi_4bitV2 current_spi_4bitV2;
lcd_spi_8bit current_spi_8bit;
glcd_spi2p_8bit current_spi2p_8bit;
glcd_spibl2 current_spibl2;
#else /* !NO_MAIN */
extern char *glcdnamelist[ST7920_GLCD_MODULE_NC];
extern int current_glcd_nli;
extern lcmclass lcdmodel;
extern glcdfonttype glcdfont;
extern lcd_5x8_pixel hd44780u;
extern lcd_iface_8bit current_iface_8bit;
extern lcd_iface_8bit blue_1602;
extern lcd_iface_8bit blue_2004;
extern lcd_iface_8bit yellow_1602;
extern lcd_fi2c_4bit current_fi2c_4bit;
extern lcd_fi2c_4bit fun_yellow_1602;
extern lcd_mi2c_4bit current_mi2c_4bit;
extern lcd_ibo_i2c_4bit current_ibo_i2c_4bit;
extern dual_lcd current_dual_lcd;
extern lcd_gpio_4bit current_gpio_4bit;
extern lcd_gpio_4bitV2 current_gpio_4bitV2;
extern lcd_spi_4bit current_spi_4bit;
extern lcd_spi_4bitV2 current_spi_4bitV2;
extern lcd_spi_8bit current_spi_8bit;
extern glcd_spi2p_8bit current_spi2p_8bit;
extern glcd_spibl2 current_spibl2;
#endif /* !NO_MAIN */

/* Prototypes */
void init_glcdnamelist(char qv);

void init_hd44780u(void);
/* Chessboard 5x8 pixel block */

int get_lcd_nc(lcmclass lcdtype);

int get_lcd_nl(lcmclass lcdtype);

void get_lcd_ddram_idx(lcmclass lcdtype,
		       int lcd_nl,
		       int lcd_ddram_idx[MAX_LCD_NL]);

void lcd_set_createchar_data(int id,
			     int *pixdata);

unsigned int init_any_iface_8bit(lcd_iface_8bit *any_iface_8bit,
				 lcmclass lcdtype,
				 int rs_gpio,
				 int e_gpio,
				 unsigned int addr,
				 char name[]);

unsigned int init_blue_1602(lcmclass lcdtype,
			    int rs_gpio,
			    int e_gpio,
			    unsigned int addr);

unsigned int init_blue_2004(lcmclass lcdtype,
			    int rs_gpio,
			    int e_gpio,
			    unsigned int addr);

unsigned int init_yellow_1602(lcmclass lcdtype,
			      int rs_gpio,
			      int e_gpio,
			      unsigned int addr);

unsigned int init_any_fi2c_4bit(lcd_fi2c_4bit *any_fi2c_4bit,
				lcmclass lcdtype,
				unsigned int addr,
				char name[]);

unsigned int init_fun_yellow_1602(lcmclass lcdtype,
				  unsigned int addr);

unsigned int init_any_mi2c_4bit(lcd_mi2c_4bit *any_mi2c_4bit,
				lcmclass lcdtype,
				char name[]);

unsigned int init_any_ibo_i2c_4bit(lcd_ibo_i2c_4bit *any_ibo_i2c_4bit,
				   lcmclass lcdtype,
				   unsigned int addr,
				   char name[]);

unsigned int init_any_dual_lcd(dual_lcd *any_dual_lcd,
			       unsigned int addr,
			       lcmclass lcdtypeA,
			       int rs_gpioA,
			       int e_gpioA,
			       lcmclass lcdtypeB,
			       int rs_gpioB,
			       int e_gpioB,
			       char name[]);

void init_current_gpio_4bit(lcd_gpio_4bit *any_gpio_4bit,
			    lcmclass lcdtype,
			    int rs_gpio,
			    int e_gpio,
			    int bl0_gpio,
			    int bl1_gpio,
			    int d4_gpio,
			    int d5_gpio,
			    int d6_gpio,
			    int d7_gpio,
			    char name[]);

void init_current_gpio_4bitV2(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			      lcmclass lcdtype,
			      int rs_gpio,
			      int e_gpio,
			      int d4_gpio,
			      int d5_gpio,
			      int d6_gpio,
			      int d7_gpio,
			      char name[]);

unsigned int init_current_spi_4bit(lcd_spi_4bit *any_spi_4bit,
				   lcmclass lcdtype,
				   int e_gpio,
				   unsigned int waddr,
				   char spidev,
				   char name[]);

void init_current_spi_4bitV2(lcd_spi_4bitV2 *any_spi_4bitV2,
			     lcmclass lcdtype,
			     int e_gpio,
			     char spidev,
			     char name[]);

void init_current_spi_8bit(lcd_spi_8bit *any_spi_8bit,
			   lcmclass lcdtype,
			   int rs_gpio,
			   int e_gpio,
			   int goe_gpio,
			   int scl_gpio,
			   char spidev,
			   char name[]);

void init_current_spi2p_8bit(glcd_spi2p_8bit *any_spi2p_8bit,
			     int rs_gpio,
			     int e_gpio,
			     int bl_gpio,
			     int goe_gpio,
			     int scl_gpio,
			     char spidev,
			     char name[]);

void init_current_spibl2(glcd_spibl2 *any_spibl2,
			 int bl0_gpio,
			 int bl1_gpio,
			 char spidev,
			 char name[]);

void echo_any_iface_8bit_status(lcd_iface_8bit *any_iface_8bit);

void echo_lcd_iface_8bit_status(lcd_iface_8bit iface);

void echo_any_fi2c_4bit_status(lcd_fi2c_4bit *fi2c);

void echo_any_mi2c_4bit_status(lcd_mi2c_4bit *mi2c);

void echo_any_ibo_i2c_4bit_status(lcd_ibo_i2c_4bit *ibo_i2c);

void echo_lcd_fi2c_4bit_status(lcd_fi2c_4bit fi2c);

void echo_any_dual_lcd_status(dual_lcd *any_dual_lcd);

void echo_any_gpio_4bit_status(lcd_gpio_4bit *any_gpio_4bit);

void echo_any_gpio_4bitV2_status(lcd_gpio_4bitV2 *any_gpio_4bitV2);

void echo_any_spi_4bit_status(lcd_spi_4bit *any_spi_4bit);

void echo_any_spi_4bitV2_status(lcd_spi_4bitV2 *any_spi_4bitV2);

void echo_any_spi_8bit_status(lcd_spi_8bit *any_spi_8bit);

void echo_any_spi2p_8bit_status(glcd_spi2p_8bit *any_spi2p_8bit);

void echo_any_spibl2_status(glcd_spibl2 *any_spibl2);

#endif /* !LCD_DISPTOOLS_H */

/* Change Log */
/*
 * $Log: lcd_disptools.h,v $
 * Revision 1.14  2018/04/13 16:26:25  pi
 * Comment typo. corrected
 *
 * Revision 1.13  2018/04/13 16:09:00  pi
 * Interim commit
 *
 * Revision 1.12  2018/04/11 17:45:50  pi
 * Interim commit
 *
 * Revision 1.11  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.10  2017/09/11 20:27:44  pi
 * Interim Commit
 *
 * Revision 1.9  2017/09/11 19:05:21  pi
 * GLCD module name support added
 *
 * Revision 1.8  2017/08/19 18:05:15  pi
 * Implicit definition warnings solved and code cleaned up
 *
 * Revision 1.7  2017/07/31 17:02:27  pi
 * Interim Commit
 *
 * Revision 1.6  2017/06/24 20:20:46  pi
 * Interim commit
 *
 * Revision 1.5  2017/06/24 02:47:16  pi
 * Interim commit. Testing 1604 LCD
 *
 * Revision 1.4  2016/11/10 17:53:00  pi
 * Interim commit
 *
 * Revision 1.3  2016/11/09 21:58:43  pi
 * Interim commit
 *
 * Revision 1.2  2016/11/09 19:15:03  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.24  2015/08/26 19:58:09  pi
 * Interim commit
 *
 * Revision 1.23  2015/08/23 13:57:12  pi
 * glcd_spibl2 structure etc. added
 *
 * Revision 1.22  2015/02/06 21:07:15  pi
 * Mjkdz I2C lcd1602 module support added
 *
 * Revision 1.21  2014/12/04 21:30:11  pi
 * Interim commit
 *
 * Revision 1.20  2014/11/20 22:10:03  pi
 * Interim commit
 *
 * Revision 1.19  2014/11/14 12:51:36  pi
 * Improved BackLight control
 *
 * Revision 1.18  2014/11/12 22:25:14  pi
 * Another spi-4bit lcd interface type added
 *
 * Revision 1.17  2014/11/08 19:45:03  pi
 * Mainly support for 4-bit SPI interface added
 *
 * Revision 1.16  2014/11/04 15:54:13  pi
 * Additional 4-bit interface constants etc.
 *
 * Revision 1.15  2014/10/28 23:06:39  pi
 * Interim commit
 *
 * Revision 1.14  2014/10/11 21:50:50  pi
 * Revised strobe timings & mechanism
 *
 * Revision 1.13  2014/10/07 18:24:47  pi
 * Minor tidy-ups
 *
 * Revision 1.12  2014/10/06 19:19:47  pi
 * Interim commit
 *
 * Revision 1.11  2014/10/01 20:26:27  pi
 * Initial CGRAM character handling
 *
 * Revision 1.10  2014/09/30 13:18:13  pi
 * Interim commit
 *
 * Revision 1.9  2014/09/23 20:57:01  pi
 * Interim commit
 *
 * Revision 1.8  2014/09/09 20:43:01  pi
 * Interim commit
 *
 * Revision 1.7  2014/09/03 21:40:30  pi
 * Interim commit. Some GLCDX_* constants corrected.
 *
 * Revision 1.6  2014/08/27 22:21:06  pi
 * Interim commit
 *
 * Revision 1.5  2014/08/27 18:44:25  pi
 * Interim commit
 *
 * Revision 1.4  2014/08/19 22:23:24  pi
 * Interim commit
 *
 * Revision 1.3  2014/08/19 18:53:47  pi
 * Interim commit
 *
 * Revision 1.2  2014/08/01 21:04:03  pi
 * Some time constants adjusted etc.
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.2  2014/07/13 20:16:21  pi
 * Starting work on SPI 8-bit LCD interface
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.20  2014/04/20 19:56:14  pi
 * Interim commit
 *
 * Revision 1.19  2014/04/20 15:03:17  pi
 * Interim commit
 *
 * Revision 1.18  2014/04/19 15:33:17  pi
 * Interim commit
 *
 * Revision 1.17  2014/04/18 22:28:17  pi
 * Interim commit
 *
 * Revision 1.16  2014/04/17 22:40:54  pi
 * Interim commit
 *
 * Revision 1.15  2014/04/14 21:29:31  pi
 * Interim commit
 *
 * Revision 1.14  2014/04/13 21:34:57  pi
 * Interim commit
 *
 * Revision 1.13  2014/04/13 20:45:54  pi
 * Interim commit
 *
 * Revision 1.12  2014/04/12 21:15:48  pi
 * Interim commit
 *
 * Revision 1.11  2014/04/12 19:12:01  pi
 * Interim commit
 *
 * Revision 1.10  2014/04/07 22:22:16  pi
 * Interim commit
 *
 * Revision 1.9  2014/03/28 22:45:46  pi
 * Interim commit
 *
 * Revision 1.8  2014/03/27 21:20:15  pi
 * Interim commit
 *
 * Revision 1.7  2014/03/25 20:18:15  pi
 * Interim commit
 *
 * Revision 1.6  2014/03/24 12:19:27  pi
 * Interim commit
 *
 * Revision 1.5  2014/03/21 21:48:18  pi
 * Interim commit. Preload mode working.
 *
 * Revision 1.4  2014/03/21 18:48:32  pi
 * Interim commit
 *
 * Revision 1.3  2014/03/09 21:52:46  pi
 * Interim commit
 *
 *
 *
 */
