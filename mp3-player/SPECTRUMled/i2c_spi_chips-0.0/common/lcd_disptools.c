/* C source file associated with LCD DISPlay TOOLS structures etc.
 * $Id: lcd_disptools.c,v 1.8 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "lcd_disptools.h"

void init_glcdnamelist(char qv)
{
  int i;
  glcdnamelist[0]  = "ST7920";
  glcdnamelist[1]  = "QC12864B_P";
  glcdnamelist[2]  = "12864ZW_P/S";
  glcdnamelist[3]  = "12864BV2.0_P/S";
  current_glcd_nli = 0;
  if (qv)
    {
      for (i=0; i<ST7920_GLCD_MODULE_NC; i++)
	printf("Glcd Module Name #%d: %s\n",i,glcdnamelist[i]); 
    }
}

void init_hd44780u(void)
/* Chessboard 5x8 pixel block */
{
  hd44780u.pixel_row0 = 0b10101; 
  hd44780u.pixel_row1 = 0b01010; 
  hd44780u.pixel_row2 = 0b10101; 
  hd44780u.pixel_row3 = 0b01010; 
  hd44780u.pixel_row4 = 0b10101; 
  hd44780u.pixel_row5 = 0b01010; 
  hd44780u.pixel_row6 = 0b10101; 
  hd44780u.pixel_row7 = 0b01010; 
}

void lcd_set_createchar_data(int id,
			     int *pixdata)
{
  pixdata[0] = id;
  pixdata[1] = hd44780u.pixel_row0;
  pixdata[2] = hd44780u.pixel_row1;
  pixdata[3] = hd44780u.pixel_row2;
  pixdata[4] = hd44780u.pixel_row3;
  pixdata[5] = hd44780u.pixel_row4;
  pixdata[6] = hd44780u.pixel_row5;
  pixdata[7] = hd44780u.pixel_row6;
  pixdata[8] = hd44780u.pixel_row7;
}
			      
int get_lcd_nc(lcmclass lcdtype)
{
  int lnc;
  switch(lcdtype)
    {
    case LCM0802: lnc =  8; break;
    case LCM1602: lnc = 16; break;
    case LCM1604: lnc = 16; break;
    case LCM2004: lnc = 20; break;
    default: lnc = 16; break;
    }
  return(lnc);
}

int get_lcd_nl(lcmclass lcdtype)
{
  int lnl;
  switch(lcdtype)
    {
    case LCM0802: lnl = 2; break;
    case LCM1602: lnl = 2; break;
    case LCM1604: lnl = 4; break;
    case LCM2004: lnl = 4; break;
    default: lnl = 2; break;
    }
  return(lnl);
}

void get_lcd_ddram_idx(lcmclass lcdtype,
		       int lcd_nl,
		       int lcd_ddram_idx[MAX_LCD_NL])
{
  int i,lnl;
  lnl = lcd_nl;
  if (lcd_nl > MAX_LCD_NL) lnl = MAX_LCD_NL;
  switch(lcdtype)
    {
    case LCM0802:
    case LCM1602:
      {
	lcd_ddram_idx[0] = LCD_DDRAM_ADDR_L1;
	for (i=1; i<lnl; i++) lcd_ddram_idx[i] = LCD_DDRAM_ADDR_L2;
	break;
      }
    case LCM1604:
      {
	lcd_ddram_idx[0] = LCD_DDRAM_ADDR_L1;
	lcd_ddram_idx[1] = LCD_DDRAM_ADDR_L2;
	lcd_ddram_idx[2] = LCD_DDRAM_ADDR_1604_L3;
	lcd_ddram_idx[3] = LCD_DDRAM_ADDR_1604_L4;
	break;
      }
    case LCM2004:
      {
	lcd_ddram_idx[0] = LCD_DDRAM_ADDR_L1;
	lcd_ddram_idx[1] = LCD_DDRAM_ADDR_L2;
	lcd_ddram_idx[2] = LCD_DDRAM_ADDR_2004_L3;
	lcd_ddram_idx[3] = LCD_DDRAM_ADDR_2004_L4;
	break;
      }
    default:
      {
	for (i=0; i<lnl; i++) lcd_ddram_idx[i] = LCD_DDRAM_ADDR_L1;
	break;
      }
    }
}

unsigned int init_any_iface_8bit(lcd_iface_8bit *any_iface_8bit,
				 lcmclass lcdtype,
				 int rs_gpio,
				 int e_gpio,
				 unsigned int addr,
				 char name[])
{
  unsigned int lca;
  (*any_iface_8bit).lcd_type = lcdtype;
  (*any_iface_8bit).lcd_nc = get_lcd_nc(lcdtype);
  (*any_iface_8bit).lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_iface_8bit).lcd_type, 
		    (*any_iface_8bit).lcd_nl,
		    (*any_iface_8bit).lcd_ddram_idx);
  (*any_iface_8bit).lcd_rs_gpio = rs_gpio;
  (*any_iface_8bit).lcd_e_gpio  = e_gpio;
  lca = 0x26;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&(*any_iface_8bit).lcd_data,
		     addr,
		     name);
  (*any_iface_8bit).lcd_ehmin = PCF8574_I2C_DRT;
  (*any_iface_8bit).lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_iface_8bit).lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_iface_8bit).lcd_wait  = (*any_iface_8bit).lcd_elmin;
  (*any_iface_8bit).lcd_nfb   = LCD_FUNC_SET_8BIT_NF28; 
  (*any_iface_8bit).lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_blue_1602(lcmclass lcdtype,
			    int rs_gpio,
			    int e_gpio,
			    unsigned int addr)
{
  unsigned int lca;
  blue_1602.lcd_type = lcdtype;
  blue_1602.lcd_nc = get_lcd_nc(lcdtype);
  blue_1602.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx(blue_1602.lcd_type, 
		    blue_1602.lcd_nl,
		    blue_1602.lcd_ddram_idx);
  blue_1602.lcd_rs_gpio = rs_gpio;
  blue_1602.lcd_e_gpio  = e_gpio;
  lca = 0x26;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&blue_1602.lcd_data,
		     addr,
		     "blue_1602");
  blue_1602.lcd_ehmin = PCF8574_I2C_DRT;
  blue_1602.lcd_elmin = HD44780U_MIN_OP_TIME;
  blue_1602.lcd_elmax = HD44780U_MAX_OP_TIME;
  blue_1602.lcd_wait  = blue_1602.lcd_elmin;
  blue_1602.lcd_nfb   = LCD_FUNC_SET_8BIT_NF28; 
  blue_1602.lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_blue_2004(lcmclass lcdtype,
			    int rs_gpio,
			    int e_gpio,
			    unsigned int addr)
{
  unsigned int lca;
  blue_2004.lcd_type = lcdtype;
  blue_2004.lcd_nc = get_lcd_nc(lcdtype);
  blue_2004.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx(blue_2004.lcd_type, 
		    blue_2004.lcd_nl,
		    blue_2004.lcd_ddram_idx);
  blue_2004.lcd_rs_gpio = rs_gpio;
  blue_2004.lcd_e_gpio  = e_gpio;
  lca = 0x26;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&blue_2004.lcd_data,
		     addr,
		     "blue_2004");
  blue_2004.lcd_ehmin = PCF8574_I2C_DRT;
  blue_2004.lcd_elmin = HD44780U_MIN_OP_TIME;
  blue_2004.lcd_elmax = HD44780U_MAX_OP_TIME;
  blue_2004.lcd_wait  = blue_2004.lcd_elmin;
  blue_2004.lcd_nfb   = LCD_FUNC_SET_8BIT_NF28; 
  blue_2004.lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_yellow_1602(lcmclass lcdtype,
			      int rs_gpio,
			      int e_gpio,
			      unsigned int addr)
{
  unsigned int lca;
  yellow_1602.lcd_type = lcdtype;
  yellow_1602.lcd_nc = get_lcd_nc(lcdtype);
  yellow_1602.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx(yellow_1602.lcd_type, 
		    yellow_1602.lcd_nl,
		    yellow_1602.lcd_ddram_idx);
  yellow_1602.lcd_rs_gpio = rs_gpio;
  yellow_1602.lcd_e_gpio  = e_gpio;
  lca = 0x26;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&yellow_1602.lcd_data,
		     addr,
		     "yellow_1602");
  yellow_1602.lcd_ehmin = PCF8574_I2C_DRT;
  yellow_1602.lcd_elmin = HD44780U_MIN_OP_TIME;
  yellow_1602.lcd_elmax = HD44780U_MAX_OP_TIME;
  yellow_1602.lcd_wait  = yellow_1602.lcd_elmin;
  yellow_1602.lcd_nfb  = LCD_FUNC_SET_8BIT_NF28; 
  yellow_1602.lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_any_fi2c_4bit(lcd_fi2c_4bit *any_fi2c_4bit,
				lcmclass lcdtype,
				unsigned int addr,
				char name[])
{
  unsigned int lca;
  (*any_fi2c_4bit).lcd_type = lcdtype;
  (*any_fi2c_4bit).lcd_nc = get_lcd_nc(lcdtype);
  (*any_fi2c_4bit).lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_fi2c_4bit).lcd_type, 
		    (*any_fi2c_4bit).lcd_nl,
		    (*any_fi2c_4bit).lcd_ddram_idx);
  lca = 0x27;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&(*any_fi2c_4bit).lcd_data,
		     addr,
		     name);
  /* (*any_fi2c_4bit).lcd_ehmin = PCF8574_I2C_DRT; */
  (*any_fi2c_4bit).lcd_ehmin = PCF8574_I2C_DRT * 3;
  (*any_fi2c_4bit).lcd_elmin = PCF8574_I2C_DRT * 2;
  /* (*any_fi2c_4bit).lcd_elmin = (*any_fi2c_4bit).lcd_ehmin; */
  (*any_fi2c_4bit).lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_fi2c_4bit).lcd_wait  = (*any_fi2c_4bit).lcd_elmin;
  (*any_fi2c_4bit).lcd_nfb   = LCD_FUNC_SET_4BIT_NF28_L 
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H); 
  (*any_fi2c_4bit).lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_fun_yellow_1602(lcmclass lcdtype,
				  unsigned int addr)
{
  unsigned int lca;
  fun_yellow_1602.lcd_type = lcdtype;
  fun_yellow_1602.lcd_nc = get_lcd_nc(lcdtype);
  fun_yellow_1602.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx(fun_yellow_1602.lcd_type, 
		    fun_yellow_1602.lcd_nl,
		    fun_yellow_1602.lcd_ddram_idx);
  lca = 0x27;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&fun_yellow_1602.lcd_data,
		     addr,
		     "fun_yellow_1602");
  /* fun_yellow_1602.lcd_ehmin = PCF8574_I2C_DRT; */
  fun_yellow_1602.lcd_ehmin = PCF8574_I2C_DRT * 3;
  /* fun_yellow_1602.lcd_elmin = HD44780U_MIN_OP_TIME; */
  fun_yellow_1602.lcd_elmin = PCF8574_I2C_DRT * 2;
  fun_yellow_1602.lcd_elmax = HD44780U_MAX_OP_TIME;
  fun_yellow_1602.lcd_wait  = fun_yellow_1602.lcd_elmin;
  fun_yellow_1602.lcd_nfb   = LCD_FUNC_SET_4BIT_NF28_L + (0x10 * LCD_FUNC_SET_4BIT_NF28_H); 
  fun_yellow_1602.lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_any_mi2c_4bit(lcd_mi2c_4bit *any_mi2c_4bit,
				lcmclass lcdtype,
				char name[])
{
  unsigned int lca;
  (*any_mi2c_4bit).M4bit.lcd_type = lcdtype;
  (*any_mi2c_4bit).M4bit.lcd_nc = get_lcd_nc(lcdtype);
  (*any_mi2c_4bit).M4bit.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_mi2c_4bit).M4bit.lcd_type, 
		    (*any_mi2c_4bit).M4bit.lcd_nl,
		    (*any_mi2c_4bit).M4bit.lcd_ddram_idx);
  /* I2C adress is fixed at 0x20! */
  lca = init_pcf8574(&(*any_mi2c_4bit).lcd_data,
		     0x20,
		     name);
  (*any_mi2c_4bit).M4bit.lcd_ehmin = PCF8574_I2C_DRT * 3;
  (*any_mi2c_4bit).M4bit.lcd_elmin = PCF8574_I2C_DRT * 2;
  (*any_mi2c_4bit).M4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_mi2c_4bit).M4bit.lcd_wait  = (*any_mi2c_4bit).M4bit.lcd_elmin;
  (*any_mi2c_4bit).M4bit.lcd_nfb   = LCD_FUNC_SET_4BIT_NF28_L 
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H); 
  (*any_mi2c_4bit).M4bit.lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_any_ibo_i2c_4bit(lcd_ibo_i2c_4bit *any_ibo_i2c_4bit,
				   lcmclass lcdtype,
				   unsigned int addr,
				   char name[])
{
  unsigned int lca;
  (*any_ibo_i2c_4bit).IBO4bit.lcd_type = lcdtype;
  (*any_ibo_i2c_4bit).IBO4bit.lcd_nc = get_lcd_nc(lcdtype);
  (*any_ibo_i2c_4bit).IBO4bit.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_ibo_i2c_4bit).IBO4bit.lcd_type, 
		    (*any_ibo_i2c_4bit).IBO4bit.lcd_nl,
		    (*any_ibo_i2c_4bit).IBO4bit.lcd_ddram_idx);
  lca = IBO_PCF8574AP;
  if (addr > 0) lca = addr;
  if (lca > 0xff) return(0);
  lca = init_pcf8574(&(*any_ibo_i2c_4bit).lcd_data,
		     addr,
		     name);
  (*any_ibo_i2c_4bit).IBO4bit.lcd_ehmin = PCF8574_I2C_DRT * 3;
  (*any_ibo_i2c_4bit).IBO4bit.lcd_elmin = PCF8574_I2C_DRT * 2;
  (*any_ibo_i2c_4bit).IBO4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_ibo_i2c_4bit).IBO4bit.lcd_wait  = (*any_ibo_i2c_4bit).IBO4bit.lcd_elmin;
  (*any_ibo_i2c_4bit).IBO4bit.lcd_nfb   = LCD_FUNC_SET_4BIT_NF28_L 
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H); 
  (*any_ibo_i2c_4bit).IBO4bit.lcd_bsize = LCD_DDRAM_SIZE;
  return(lca);
}

unsigned int init_any_dual_lcd(dual_lcd *any_dual_lcd,
			       unsigned int addr,
			       lcmclass lcdtypeA,
			       int rs_gpioA,
			       int e_gpioA,
			       lcmclass lcdtypeB,
			       int rs_gpioB,
			       int e_gpioB,
			       char name[])
{
  unsigned int lca;
  lca = init_mcp23017(&(*any_dual_lcd).lcd_data,
		      addr,
		      name);
  (*any_dual_lcd).A8bit.lcd_type = lcdtypeA;
  (*any_dual_lcd).lcd_rs_gpioA = rs_gpioA;
  (*any_dual_lcd).lcd_e_gpioA = e_gpioA;
  (*any_dual_lcd).B4bit.lcd_type = lcdtypeB;
  (*any_dual_lcd).lcd_rs_gpioB = rs_gpioB;
  (*any_dual_lcd).lcd_e_gpioB = e_gpioB;
  (*any_dual_lcd).A8bit.lcd_nc = get_lcd_nc(lcdtypeA);
  (*any_dual_lcd).A8bit.lcd_nl = get_lcd_nl(lcdtypeA);
  get_lcd_ddram_idx((*any_dual_lcd).A8bit.lcd_type, 
		    (*any_dual_lcd).A8bit.lcd_nl,
		    (*any_dual_lcd).A8bit.lcd_ddram_idx);
  (*any_dual_lcd).A8bit.lcd_ehmin = MCP23017_I2C_DRT;
  (*any_dual_lcd).A8bit.lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_dual_lcd).A8bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_dual_lcd).A8bit.lcd_wait = (*any_dual_lcd).A8bit.lcd_elmin;
  (*any_dual_lcd).A8bit.lcd_nfb = LCD_FUNC_SET_8BIT_NF28;
  (*any_dual_lcd).A8bit.lcd_bsize = LCD_DDRAM_SIZE;
  (*any_dual_lcd).B4bit.lcd_nc = get_lcd_nc(lcdtypeB);
  (*any_dual_lcd).B4bit.lcd_nl = get_lcd_nl(lcdtypeB);
  get_lcd_ddram_idx((*any_dual_lcd).B4bit.lcd_type, 
		    (*any_dual_lcd).B4bit.lcd_nl,
		    (*any_dual_lcd).B4bit.lcd_ddram_idx);
  /*  (*any_dual_lcd).B4bit.lcd_ehmin = MCP23017_I2C_DRT; */
  /*(*any_dual_lcd).B4bit.lcd_ehmin = MCP23017_I2C_DRT * 2; */
  (*any_dual_lcd).B4bit.lcd_ehmin = MCP23017_I2C_DRT * 3;
  (*any_dual_lcd).B4bit.lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_dual_lcd).B4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_dual_lcd).B4bit.lcd_wait = (*any_dual_lcd).B4bit.lcd_elmin;
  (*any_dual_lcd).B4bit.lcd_nfb = LCD_FUNC_SET_4BIT_NF28_L
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H);
  (*any_dual_lcd).B4bit.lcd_bsize = LCD_DDRAM_SIZE;
  (*any_dual_lcd).lcd_B4data = 0x00;
  return(lca);
}

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
			    char name[])
{
  (*any_gpio_4bit).G4bit.lcd_type = lcdtype;
  (*any_gpio_4bit).G4bit.lcd_nc = get_lcd_nc(lcdtype);
  (*any_gpio_4bit).G4bit.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_gpio_4bit).G4bit.lcd_type, 
		    (*any_gpio_4bit).G4bit.lcd_nl,
		    (*any_gpio_4bit).G4bit.lcd_ddram_idx);
  /* (*any_gpio_4bit).G4bit.lcd_ehmin = HD44780U_MIN_OP_TIME; */
  (*any_gpio_4bit).G4bit.lcd_ehmin = 1;
  (*any_gpio_4bit).G4bit.lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_gpio_4bit).G4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_gpio_4bit).G4bit.lcd_wait = (*any_gpio_4bit).G4bit.lcd_elmin;
  (*any_gpio_4bit).lcd_condat.rs_gpio  = rs_gpio;
  (*any_gpio_4bit).lcd_condat.e_gpio   = e_gpio;
  (*any_gpio_4bit).lcd_condat.bl0_gpio = bl0_gpio;
  (*any_gpio_4bit).lcd_condat.bl1_gpio = bl1_gpio;
  (*any_gpio_4bit).lcd_condat.d4_gpio  = d4_gpio;
  (*any_gpio_4bit).lcd_condat.d5_gpio  = d5_gpio;
  (*any_gpio_4bit).lcd_condat.d6_gpio  = d6_gpio;
  (*any_gpio_4bit).lcd_condat.d7_gpio  = d7_gpio;
  strncpy((*any_gpio_4bit).lcd_condat.name, name, (GPIO_IFACE_NAME_LEN-1));
  (*any_gpio_4bit).G4bit.lcd_nfb = LCD_FUNC_SET_4BIT_NF28_L
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H);
  (*any_gpio_4bit).G4bit.lcd_bsize = LCD_DDRAM_SIZE;
  (*any_gpio_4bit).lcd_G4data = 0x00;
}

void init_current_gpio_4bitV2(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			      lcmclass lcdtype,
			      int rs_gpio,
			      int e_gpio,
			      int d4_gpio,
			      int d5_gpio,
			      int d6_gpio,
			      int d7_gpio,
			      char name[])
{
  (*any_gpio_4bitV2).G4bit.lcd_type = lcdtype;
  (*any_gpio_4bitV2).G4bit.lcd_nc = get_lcd_nc(lcdtype);
  (*any_gpio_4bitV2).G4bit.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_gpio_4bitV2).G4bit.lcd_type, 
		    (*any_gpio_4bitV2).G4bit.lcd_nl,
		    (*any_gpio_4bitV2).G4bit.lcd_ddram_idx);
  /* (*any_gpio_4bitV2).G4bit.lcd_ehmin = HD44780U_MIN_OP_TIME; */
  (*any_gpio_4bitV2).G4bit.lcd_ehmin = 1;
  (*any_gpio_4bitV2).G4bit.lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_gpio_4bitV2).G4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_gpio_4bitV2).G4bit.lcd_wait = (*any_gpio_4bitV2).G4bit.lcd_elmin;
  (*any_gpio_4bitV2).lcd_condat.rs_gpio  = rs_gpio;
  (*any_gpio_4bitV2).lcd_condat.e_gpio   = e_gpio;
  (*any_gpio_4bitV2).lcd_condat.d4_gpio  = d4_gpio;
  (*any_gpio_4bitV2).lcd_condat.d5_gpio  = d5_gpio;
  (*any_gpio_4bitV2).lcd_condat.d6_gpio  = d6_gpio;
  (*any_gpio_4bitV2).lcd_condat.d7_gpio  = d7_gpio;
  strncpy((*any_gpio_4bitV2).lcd_condat.name, name, (GPIO_IFACE_NAME_LEN-1));
  (*any_gpio_4bitV2).G4bit.lcd_nfb = LCD_FUNC_SET_4BIT_NF28_L
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H);
  (*any_gpio_4bitV2).G4bit.lcd_bsize = LCD_DDRAM_SIZE;
  (*any_gpio_4bitV2).lcd_G4data = 0x00;
}

unsigned int init_current_spi_4bit(lcd_spi_4bit *any_spi_4bit,
				   lcmclass lcdtype,
				   int e_gpio,
				   unsigned int waddr,
				   char spidev,
				   char name[])
{
  unsigned int lcw;
  lcw = init_mcp23S08(&(*any_spi_4bit).lcd_data,
		      waddr,
		      spidev,
		      name);	
  (*any_spi_4bit).S4bit.lcd_type = lcdtype;
  (*any_spi_4bit).S4bit.lcd_nc = get_lcd_nc(lcdtype);
  (*any_spi_4bit).S4bit.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_spi_4bit).S4bit.lcd_type, 
		    (*any_spi_4bit).S4bit.lcd_nl,
		    (*any_spi_4bit).S4bit.lcd_ddram_idx);
  (*any_spi_4bit).S4bit.lcd_ehmin = MCP23S08_SPI_DRT;
  (*any_spi_4bit).S4bit.lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_spi_4bit).S4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_spi_4bit).S4bit.lcd_wait = (*any_spi_4bit).S4bit.lcd_elmin;
  (*any_spi_4bit).lcd_e_gpio  = e_gpio;
  (*any_spi_4bit).S4bit.lcd_nfb = LCD_FUNC_SET_4BIT_NF28_L
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H);
  (*any_spi_4bit).S4bit.lcd_bsize = LCD_DDRAM_SIZE;
  (*any_spi_4bit).lcd_S4data = 0x00;
  return(lcw);
}

void init_current_spi_4bitV2(lcd_spi_4bitV2 *any_spi_4bitV2,
			     lcmclass lcdtype,
			     int e_gpio,
			     char spidev,
			     char name[])
{
  init_hw74hc595(&(*any_spi_4bitV2).lcd_data,
		 spidev,
		 name);
  (*any_spi_4bitV2).S4bit.lcd_type = lcdtype;
  (*any_spi_4bitV2).S4bit.lcd_nc = get_lcd_nc(lcdtype);
  (*any_spi_4bitV2).S4bit.lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_spi_4bitV2).S4bit.lcd_type, 
		    (*any_spi_4bitV2).S4bit.lcd_nl,
		    (*any_spi_4bitV2).S4bit.lcd_ddram_idx);
  (*any_spi_4bitV2).S4bit.lcd_ehmin = HW74HC595_SPI_DRT;
  (*any_spi_4bitV2).S4bit.lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_spi_4bitV2).S4bit.lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_spi_4bitV2).S4bit.lcd_wait = (*any_spi_4bitV2).S4bit.lcd_elmin;
  (*any_spi_4bitV2).lcd_e_gpio  = e_gpio;
  (*any_spi_4bitV2).S4bit.lcd_nfb = LCD_FUNC_SET_4BIT_NF28_L
    + (0x10 * LCD_FUNC_SET_4BIT_NF28_H);
  (*any_spi_4bitV2).S4bit.lcd_bsize = LCD_DDRAM_SIZE;
  (*any_spi_4bitV2).lcd_S4data = 0x00;
}

void init_current_spi_8bit(lcd_spi_8bit *any_spi_8bit,
			   lcmclass lcdtype,
			   int rs_gpio,
			   int e_gpio,
			   int goe_gpio,
			   int scl_gpio,
			   char spidev,
			   char name[])
{
  (*any_spi_8bit).lcd_type = lcdtype;
  (*any_spi_8bit).lcd_nc = get_lcd_nc(lcdtype);
  (*any_spi_8bit).lcd_nl = get_lcd_nl(lcdtype);
  get_lcd_ddram_idx((*any_spi_8bit).lcd_type, 
		    (*any_spi_8bit).lcd_nl,
		    (*any_spi_8bit).lcd_ddram_idx);
  (*any_spi_8bit).lcd_rs_gpio = rs_gpio;
  (*any_spi_8bit).lcd_e_gpio  = e_gpio;
  init_gc74hc595(&(*any_spi_8bit).lcd_data,
		 goe_gpio,
		 scl_gpio,
		 spidev,
		 name);	
  /*  (*any_spi_8bit).lcd_ehmin = HD44780U_MIN_OP_TIME + GC74HC595_SPI_DRT; */
  (*any_spi_8bit).lcd_ehmin = GC74HC595_SPI_DRT;
  (*any_spi_8bit).lcd_elmin = HD44780U_MIN_OP_TIME;
  (*any_spi_8bit).lcd_elmax = HD44780U_MAX_OP_TIME;
  (*any_spi_8bit).lcd_wait  = (*any_spi_8bit).lcd_elmin;
  (*any_spi_8bit).lcd_nfb   = LCD_FUNC_SET_8BIT_NF28; 
  (*any_spi_8bit).lcd_bsize = LCD_DDRAM_SIZE;
}

void init_current_spi2p_8bit(glcd_spi2p_8bit *any_spi2p_8bit,
			     int rs_gpio,
			     int e_gpio,
			     int bl_gpio,
			     int goe_gpio,
			     int scl_gpio,
			     char spidev,
			     char name[])
{
  (*any_spi2p_8bit).glcd_nc = 16;
  (*any_spi2p_8bit).glcd_nl = 4;
  (*any_spi2p_8bit).glcd_ddram_idx[0] = GLCD_DDRAM_ADDR_L1;
  (*any_spi2p_8bit).glcd_ddram_idx[1] = GLCD_DDRAM_ADDR_L2;
  (*any_spi2p_8bit).glcd_ddram_idx[2] = GLCD_DDRAM_ADDR_L3;
  (*any_spi2p_8bit).glcd_ddram_idx[3] = GLCD_DDRAM_ADDR_L4;
  (*any_spi2p_8bit).glcd_rs_gpio = rs_gpio;
  (*any_spi2p_8bit).glcd_e_gpio  = e_gpio;
  (*any_spi2p_8bit).glcd_bl_gpio = bl_gpio;
  init_gc74hc595(&(*any_spi2p_8bit).glcd_data,
		 goe_gpio,
		 scl_gpio,
		 spidev,
		 name);	
  /* (*any_spi2p_8bit).glcd_ehmin = ST7920_MIN_OP_TIME + GC74HC595_SPI_DRT; */
  (*any_spi2p_8bit).glcd_ehmin = GC74HC595_SPI_DRT;
  (*any_spi2p_8bit).glcd_elmin = ST7920_MIN_OP_TIME;
  /* (*any_spi2p_8bit).glcd_elmin = (*any_spi2p_8bit).glcd_ehmin; */
  (*any_spi2p_8bit).glcd_elmax = ST7920_MAX_OP_TIME + (*any_spi2p_8bit).glcd_elmin;
  (*any_spi2p_8bit).glcd_wait  = (*any_spi2p_8bit).glcd_elmin;
}

void init_current_spibl2(glcd_spibl2 *any_spibl2,
			 int bl0_gpio,
			 int bl1_gpio,
			 char spidev,
			 char name[])
{
  (*any_spibl2).glcd_nc = 16;
  (*any_spibl2).glcd_nl = 4;
  (*any_spibl2).glcd_ddram_idx[0] = GLCD_DDRAM_ADDR_L1;
  (*any_spibl2).glcd_ddram_idx[1] = GLCD_DDRAM_ADDR_L2;
  (*any_spibl2).glcd_ddram_idx[2] = GLCD_DDRAM_ADDR_L3;
  (*any_spibl2).glcd_ddram_idx[3] = GLCD_DDRAM_ADDR_L4;
  (*any_spibl2).glcd_bl0_gpio = bl0_gpio;
  (*any_spibl2).glcd_bl1_gpio = bl1_gpio;
  init_st7920sm(&(*any_spibl2).glcd_data,
		spidev,
		name);	
  (*any_spibl2).glcd_synch = ST7920_SYNCH_WRITE_CMD;
  (*any_spibl2).glcd_datah = 0x00;
  (*any_spibl2).glcd_datal = 0x00;
}

void echo_any_iface_8bit_status(lcd_iface_8bit *any_iface_8bit)
{
  printf("lcd iface_8bit status:\n");
  printf("TYPE    %2d\n", (*any_iface_8bit).lcd_type);
  printf("NC      %2d\n", (*any_iface_8bit).lcd_nc);
  printf("NL      %2d\n", (*any_iface_8bit).lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_iface_8bit).lcd_ddram_idx[0],
	 (*any_iface_8bit).lcd_ddram_idx[1],
	 (*any_iface_8bit).lcd_ddram_idx[2],
	 (*any_iface_8bit).lcd_ddram_idx[3]);
  printf("RS GPIO %02d\n", (*any_iface_8bit).lcd_rs_gpio);
  printf("E  GPIO %02d\n", (*any_iface_8bit).lcd_e_gpio);
  printf("EHMIN  %3d uS\n", (*any_iface_8bit).lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_iface_8bit).lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_iface_8bit).lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_iface_8bit).lcd_wait);
  printf("NFB     %02x\n", (*any_iface_8bit).lcd_nfb);
  printf("BSIZE   %02d\n", (*any_iface_8bit).lcd_bsize);
  echo_pcf8574_status((*any_iface_8bit).lcd_data);}

void echo_lcd_iface_8bit_status(lcd_iface_8bit iface)
{
  printf("lcd iface_8bit status:\n");
  printf("TYPE    %2d\n", iface.lcd_type);
  printf("NC      %2d\n", iface.lcd_nc);
  printf("NL      %2d\n", iface.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 iface.lcd_ddram_idx[0],
	 iface.lcd_ddram_idx[1],
	 iface.lcd_ddram_idx[2],
	 iface.lcd_ddram_idx[3]);
  printf("RS GPIO %02d\n", iface.lcd_rs_gpio);
  printf("E  GPIO %02d\n", iface.lcd_e_gpio);
  printf("EHMIN  %3d uS\n", iface.lcd_ehmin);
  printf("ELMIN   %2d uS\n", iface.lcd_elmin);
  printf("ELMAX %4d uS\n", iface.lcd_elmax);
  printf("WAIT  %4d uS\n", iface.lcd_wait);
  printf("NFB     %02x\n", iface.lcd_nfb);
  printf("BSIZE   %02d\n", iface.lcd_bsize);
  echo_pcf8574_status(iface.lcd_data);
}

void echo_any_fi2c_4bit_status(lcd_fi2c_4bit *fi2c)
{
  printf("lcd fi2c_4bit status:\n");
  printf("TYPE    %2d\n", (*fi2c).lcd_type);
  printf("NC      %2d\n", (*fi2c).lcd_nc);
  printf("NL      %2d\n", (*fi2c).lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*fi2c).lcd_ddram_idx[0],
	 (*fi2c).lcd_ddram_idx[1],
	 (*fi2c).lcd_ddram_idx[2],
	 (*fi2c).lcd_ddram_idx[3]);
  printf("EHMIN  %3d uS\n", (*fi2c).lcd_ehmin);
  printf("ELMIN  %3d uS\n", (*fi2c).lcd_elmin);
  printf("ELMAX %4d uS\n", (*fi2c).lcd_elmax);
  printf("WAIT  %4d uS\n", (*fi2c).lcd_wait);
  printf("NFB     %02x\n", (*fi2c).lcd_nfb);
  printf("BSIZE   %02d\n", (*fi2c).lcd_bsize);
  echo_pcf8574_status((*fi2c).lcd_data);
}

void echo_any_mi2c_4bit_status(lcd_mi2c_4bit *mi2c)
{
  printf("lcd mi2c_4bit status:\n");
  printf("TYPE    %2d\n", (*mi2c).M4bit.lcd_type);
  printf("NC      %2d\n", (*mi2c).M4bit.lcd_nc);
  printf("NL      %2d\n", (*mi2c).M4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*mi2c).M4bit.lcd_ddram_idx[0],
	 (*mi2c).M4bit.lcd_ddram_idx[1],
	 (*mi2c).M4bit.lcd_ddram_idx[2],
	 (*mi2c).M4bit.lcd_ddram_idx[3]);
  printf("EHMIN  %3d uS\n", (*mi2c).M4bit.lcd_ehmin);
  printf("ELMIN  %3d uS\n", (*mi2c).M4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*mi2c).M4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*mi2c).M4bit.lcd_wait);
  printf("NFB     %02x\n", (*mi2c).M4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*mi2c).M4bit.lcd_bsize);
  echo_pcf8574_status((*mi2c).lcd_data);
}

void echo_any_ibo_i2c_4bit_status(lcd_ibo_i2c_4bit *ibo_i2c)
{
  printf("lcd ibo_i2c_4bit status:\n");
  printf("TYPE    %2d\n", (*ibo_i2c).IBO4bit.lcd_type);
  printf("NC      %2d\n", (*ibo_i2c).IBO4bit.lcd_nc);
  printf("NL      %2d\n", (*ibo_i2c).IBO4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*ibo_i2c).IBO4bit.lcd_ddram_idx[0],
	 (*ibo_i2c).IBO4bit.lcd_ddram_idx[1],
	 (*ibo_i2c).IBO4bit.lcd_ddram_idx[2],
	 (*ibo_i2c).IBO4bit.lcd_ddram_idx[3]);
  printf("EHMIN  %3d uS\n", (*ibo_i2c).IBO4bit.lcd_ehmin);
  printf("ELMIN  %3d uS\n", (*ibo_i2c).IBO4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*ibo_i2c).IBO4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*ibo_i2c).IBO4bit.lcd_wait);
  printf("NFB     %02x\n", (*ibo_i2c).IBO4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*ibo_i2c).IBO4bit.lcd_bsize);
  echo_pcf8574_status((*ibo_i2c).lcd_data);
}

void echo_lcd_fi2c_4bit_status(lcd_fi2c_4bit fi2c)
{
  printf("lcd fi2c_4bit status:\n");
  printf("TYPE    %2d\n", fi2c.lcd_type);
  printf("NC      %2d\n", fi2c.lcd_nc);
  printf("NL      %2d\n", fi2c.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 fi2c.lcd_ddram_idx[0],
	 fi2c.lcd_ddram_idx[1],
	 fi2c.lcd_ddram_idx[2],
	 fi2c.lcd_ddram_idx[3]);
  printf("EHMIN  %3d uS\n", fi2c.lcd_ehmin);
  printf("ELMIN  %3d uS\n", fi2c.lcd_elmin);
  printf("ELMAX %4d uS\n", fi2c.lcd_elmax);
  printf("WAIT  %4d uS\n", fi2c.lcd_wait);
  printf("NFB     %02x\n", fi2c.lcd_nfb);
  printf("BSIZE   %02d\n", fi2c.lcd_bsize);
  echo_pcf8574_status(fi2c.lcd_data);
}

void echo_any_dual_lcd_status(dual_lcd *any_dual_lcd)
{
  printf("dual lcd status:\n");
  echo_mcp23017_status((*any_dual_lcd).lcd_data);
  printf("lcd A8bit:\n");
  printf("TYPE    %2d\n", (*any_dual_lcd).A8bit.lcd_type);
  printf("NC      %2d\n", (*any_dual_lcd).A8bit.lcd_nc);
  printf("NL      %2d\n", (*any_dual_lcd).A8bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_dual_lcd).A8bit.lcd_ddram_idx[0],
	 (*any_dual_lcd).A8bit.lcd_ddram_idx[1],
	 (*any_dual_lcd).A8bit.lcd_ddram_idx[2],
	 (*any_dual_lcd).A8bit.lcd_ddram_idx[3]);
  printf("RS GPIO %02d\n", (*any_dual_lcd).lcd_rs_gpioA);
  printf("E  GPIO %02d\n", (*any_dual_lcd).lcd_e_gpioA);
  printf("EHMIN  %3d uS\n", (*any_dual_lcd).A8bit.lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_dual_lcd).A8bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_dual_lcd).A8bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_dual_lcd).A8bit.lcd_wait);
  printf("NFB     %02x\n", (*any_dual_lcd).A8bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*any_dual_lcd).A8bit.lcd_bsize);
  printf("lcd B4bit:\n");
  printf("TYPE    %2d\n", (*any_dual_lcd).B4bit.lcd_type);
  printf("NC      %2d\n", (*any_dual_lcd).B4bit.lcd_nc);
  printf("NL      %2d\n", (*any_dual_lcd).B4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_dual_lcd).B4bit.lcd_ddram_idx[0],
	 (*any_dual_lcd).B4bit.lcd_ddram_idx[1],
	 (*any_dual_lcd).B4bit.lcd_ddram_idx[2],
	 (*any_dual_lcd).B4bit.lcd_ddram_idx[3]);
  printf("RS GPIO %02d\n", (*any_dual_lcd).lcd_rs_gpioB);
  printf("E  GPIO %02d\n", (*any_dual_lcd).lcd_e_gpioB);
  printf("EHMIN  %3d uS\n", (*any_dual_lcd).B4bit.lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_dual_lcd).B4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_dual_lcd).B4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_dual_lcd).B4bit.lcd_wait);
  printf("NFB     %02x\n", (*any_dual_lcd).B4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*any_dual_lcd).B4bit.lcd_bsize);
  printf("B4DATA  %02x\n", (*any_dual_lcd).lcd_B4data);
}

void echo_any_gpio_4bit_status(lcd_gpio_4bit *any_gpio_4bit)
{
  printf("lcd gpio_4bit status:\n");
  printf("RS  GPIO %02d\n", (*any_gpio_4bit).lcd_condat.rs_gpio);
  printf("E   GPIO %02d\n", (*any_gpio_4bit).lcd_condat.e_gpio);
  printf("D4  GPIO %02d\n", (*any_gpio_4bit).lcd_condat.d4_gpio);
  printf("D5  GPIO %02d\n", (*any_gpio_4bit).lcd_condat.d5_gpio);
  printf("D6  GPIO %02d\n", (*any_gpio_4bit).lcd_condat.d6_gpio);
  printf("D7  GPIO %02d\n", (*any_gpio_4bit).lcd_condat.d7_gpio);
  printf("NAME %s\n", (*any_gpio_4bit).lcd_condat.name);
  printf("lcd G4bit:\n");
  printf("TYPE    %2d\n", (*any_gpio_4bit).G4bit.lcd_type);
  printf("NC      %2d\n", (*any_gpio_4bit).G4bit.lcd_nc);
  printf("NL      %2d\n", (*any_gpio_4bit).G4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_gpio_4bit).G4bit.lcd_ddram_idx[0],
	 (*any_gpio_4bit).G4bit.lcd_ddram_idx[1],
	 (*any_gpio_4bit).G4bit.lcd_ddram_idx[2],
	 (*any_gpio_4bit).G4bit.lcd_ddram_idx[3]);
  printf("EHMIN  %3d uS\n", (*any_gpio_4bit).G4bit.lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_gpio_4bit).G4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_gpio_4bit).G4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_gpio_4bit).G4bit.lcd_wait);
  printf("NFB     %02x\n", (*any_gpio_4bit).G4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*any_gpio_4bit).G4bit.lcd_bsize);
  printf("G4DATA  %02x\n", (*any_gpio_4bit).lcd_G4data);
}

void echo_any_gpio_4bitV2_status(lcd_gpio_4bitV2 *any_gpio_4bitV2)
{
  printf("lcd gpio_4bitV2 status:\n");
  printf("RS  GPIO %02d\n", (*any_gpio_4bitV2).lcd_condat.rs_gpio);
  printf("E   GPIO %02d\n", (*any_gpio_4bitV2).lcd_condat.e_gpio);
  printf("D4  GPIO %02d\n", (*any_gpio_4bitV2).lcd_condat.d4_gpio);
  printf("D5  GPIO %02d\n", (*any_gpio_4bitV2).lcd_condat.d5_gpio);
  printf("D6  GPIO %02d\n", (*any_gpio_4bitV2).lcd_condat.d6_gpio);
  printf("D7  GPIO %02d\n", (*any_gpio_4bitV2).lcd_condat.d7_gpio);
  printf("NAME %s\n", (*any_gpio_4bitV2).lcd_condat.name);
  printf("lcd G4bit:\n");
  printf("TYPE    %2d\n", (*any_gpio_4bitV2).G4bit.lcd_type);
  printf("NC      %2d\n", (*any_gpio_4bitV2).G4bit.lcd_nc);
  printf("NL      %2d\n", (*any_gpio_4bitV2).G4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_gpio_4bitV2).G4bit.lcd_ddram_idx[0],
	 (*any_gpio_4bitV2).G4bit.lcd_ddram_idx[1],
	 (*any_gpio_4bitV2).G4bit.lcd_ddram_idx[2],
	 (*any_gpio_4bitV2).G4bit.lcd_ddram_idx[3]);
  printf("EHMIN  %3d uS\n", (*any_gpio_4bitV2).G4bit.lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_gpio_4bitV2).G4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_gpio_4bitV2).G4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_gpio_4bitV2).G4bit.lcd_wait);
  printf("NFB     %02x\n", (*any_gpio_4bitV2).G4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*any_gpio_4bitV2).G4bit.lcd_bsize);
  printf("G4DATA  %02x\n", (*any_gpio_4bitV2).lcd_G4data);
}

void echo_any_spi_4bit_status(lcd_spi_4bit *any_spi_4bit)
{
  printf("lcd spi_4bit status:\n");
  echo_mcp23S08_status((*any_spi_4bit).lcd_data);
  printf("lcd S4bit:\n");
  printf("TYPE    %2d\n", (*any_spi_4bit).S4bit.lcd_type);
  printf("NC      %2d\n", (*any_spi_4bit).S4bit.lcd_nc);
  printf("NL      %2d\n", (*any_spi_4bit).S4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_spi_4bit).S4bit.lcd_ddram_idx[0],
	 (*any_spi_4bit).S4bit.lcd_ddram_idx[1],
	 (*any_spi_4bit).S4bit.lcd_ddram_idx[2],
	 (*any_spi_4bit).S4bit.lcd_ddram_idx[3]);
  printf("E  GPIO %02d\n", (*any_spi_4bit).lcd_e_gpio);
  printf("EHMIN  %3d uS\n", (*any_spi_4bit).S4bit.lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_spi_4bit).S4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_spi_4bit).S4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_spi_4bit).S4bit.lcd_wait);
  printf("NFB     %02x\n", (*any_spi_4bit).S4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*any_spi_4bit).S4bit.lcd_bsize);
  printf("S4DATA  %02x\n", (*any_spi_4bit).lcd_S4data);
}

void echo_any_spi_4bitV2_status(lcd_spi_4bitV2 *any_spi_4bitV2)
{
  printf("lcd spi_4bitV2 status:\n");
  echo_hw74hc595_status((*any_spi_4bitV2).lcd_data);
  printf("lcd S4bit:\n");
  printf("TYPE    %2d\n", (*any_spi_4bitV2).S4bit.lcd_type);
  printf("NC      %2d\n", (*any_spi_4bitV2).S4bit.lcd_nc);
  printf("NL      %2d\n", (*any_spi_4bitV2).S4bit.lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_spi_4bitV2).S4bit.lcd_ddram_idx[0],
	 (*any_spi_4bitV2).S4bit.lcd_ddram_idx[1],
	 (*any_spi_4bitV2).S4bit.lcd_ddram_idx[2],
	 (*any_spi_4bitV2).S4bit.lcd_ddram_idx[3]);
  printf("E  GPIO %02d\n", (*any_spi_4bitV2).lcd_e_gpio);
  printf("EHMIN  %3d uS\n", (*any_spi_4bitV2).S4bit.lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_spi_4bitV2).S4bit.lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_spi_4bitV2).S4bit.lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_spi_4bitV2).S4bit.lcd_wait);
  printf("NFB     %02x\n", (*any_spi_4bitV2).S4bit.lcd_nfb);
  printf("BSIZE   %02d\n", (*any_spi_4bitV2).S4bit.lcd_bsize);
  printf("S4DATA  %02x\n", (*any_spi_4bitV2).lcd_S4data);
}

void echo_any_spi_8bit_status(lcd_spi_8bit *any_spi_8bit)
{
  printf("lcd spi_8bit status:\n");
  printf("TYPE    %2d\n", (*any_spi_8bit).lcd_type);
  printf("NC      %2d\n", (*any_spi_8bit).lcd_nc);
  printf("NL      %2d\n", (*any_spi_8bit).lcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_spi_8bit).lcd_ddram_idx[0],
	 (*any_spi_8bit).lcd_ddram_idx[1],
	 (*any_spi_8bit).lcd_ddram_idx[2],
	 (*any_spi_8bit).lcd_ddram_idx[3]);
  printf("RS GPIO %02d\n", (*any_spi_8bit).lcd_rs_gpio);
  printf("E  GPIO %02d\n", (*any_spi_8bit).lcd_e_gpio);
  printf("EHMIN  %3d uS\n", (*any_spi_8bit).lcd_ehmin);
  printf("ELMIN   %2d uS\n", (*any_spi_8bit).lcd_elmin);
  printf("ELMAX %4d uS\n", (*any_spi_8bit).lcd_elmax);
  printf("WAIT  %4d uS\n", (*any_spi_8bit).lcd_wait);
  printf("NFB     %02x\n", (*any_spi_8bit).lcd_nfb);
  printf("BSIZE   %02d\n", (*any_spi_8bit).lcd_bsize);
  echo_gc74hc595_status((*any_spi_8bit).lcd_data);
}

void echo_any_spi2p_8bit_status(glcd_spi2p_8bit *any_spi2p_8bit)
{
  printf("glcd spi2p_8bit status:\n");
  printf("NC      %2d\n", (*any_spi2p_8bit).glcd_nc);
  printf("NL      %2d\n", (*any_spi2p_8bit).glcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_spi2p_8bit).glcd_ddram_idx[0],
	 (*any_spi2p_8bit).glcd_ddram_idx[1],
	 (*any_spi2p_8bit).glcd_ddram_idx[2],
	 (*any_spi2p_8bit).glcd_ddram_idx[3]);
  printf("RS GPIO %02d\n", (*any_spi2p_8bit).glcd_rs_gpio);
  printf("E  GPIO %02d\n", (*any_spi2p_8bit).glcd_e_gpio);
  printf("BL GPIO %02d\n", (*any_spi2p_8bit).glcd_bl_gpio);
  printf("EHMIN  %3d uS\n", (*any_spi2p_8bit).glcd_ehmin);
  printf("ELMIN  %3d uS\n", (*any_spi2p_8bit).glcd_elmin);
  printf("ELMAX %4d uS\n", (*any_spi2p_8bit).glcd_elmax);
  printf("WAIT  %4d uS\n", (*any_spi2p_8bit).glcd_wait);
  echo_gc74hc595_status((*any_spi2p_8bit).glcd_data);
}

void echo_any_spibl2_status(glcd_spibl2 *any_spibl2)
{
  printf("glcd spibl2 status:\n");
  printf("NC      %2d\n", (*any_spibl2).glcd_nc);
  printf("NL      %2d\n", (*any_spibl2).glcd_nl);
  printf("DDRAM[] %02x %02x %02x %02x\n",
	 (*any_spibl2).glcd_ddram_idx[0],
	 (*any_spibl2).glcd_ddram_idx[1],
	 (*any_spibl2).glcd_ddram_idx[2],
	 (*any_spibl2).glcd_ddram_idx[3]);
  printf("BL0 GPIO %02d\n", (*any_spibl2).glcd_bl0_gpio);
  printf("BL1 GPIO %02d\n", (*any_spibl2).glcd_bl1_gpio);
  echo_st7920sm_status((*any_spibl2).glcd_data);
}

/* Change Log */
/*
 * $Log: lcd_disptools.c,v $
 * Revision 1.8  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.7  2017/09/11 20:27:44  pi
 * Interim Commit
 *
 * Revision 1.6  2017/09/11 19:05:21  pi
 * GLCD module name support added
 *
 * Revision 1.5  2017/07/31 17:02:27  pi
 * Interim Commit
 *
 * Revision 1.4  2017/06/24 02:47:16  pi
 * Interim commit. Testing 1604 LCD
 *
 * Revision 1.3  2016/11/10 17:53:00  pi
 * Interim commit
 *
 * Revision 1.2  2016/11/09 19:15:03  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.20  2015/08/23 13:57:12  pi
 * glcd_spibl2 structure etc. added
 *
 * Revision 1.19  2015/02/06 21:07:15  pi
 * Mjkdz I2C lcd1602 module support added
 *
 * Revision 1.18  2014/12/05 19:14:13  pi
 * Interim commit
 *
 * Revision 1.17  2014/12/04 21:30:11  pi
 * Interim commit
 *
 * Revision 1.16  2014/11/17 19:54:51  pi
 * I2C ehmin timings adjusted
 *
 * Revision 1.15  2014/11/12 22:25:14  pi
 * Another spi-4bit lcd interface type added
 *
 * Revision 1.14  2014/11/08 19:45:03  pi
 * Some tidying up and improved address assignment
 *
 * Revision 1.13  2014/11/06 17:12:21  pi
 * ehmin values tweaked
 *
 * Revision 1.12  2014/11/04 20:57:12  pi
 * Interim commit
 *
 * Revision 1.11  2014/11/04 15:54:13  pi
 * Additional 4-bit interface constants etc.
 *
 * Revision 1.10  2014/10/28 23:06:39  pi
 * Interim commit
 *
 * Revision 1.9  2014/10/11 21:50:50  pi
 * Revised strobe timings & mechanism
 *
 * Revision 1.8  2014/10/09 15:30:16  pi
 * 4-bit PCF8574-based strobe timings adjusted (+ bug-fix)
 *
 * Revision 1.7  2014/08/31 19:54:12  pi
 * Minor mods to all "echo...status" routines
 *
 * Revision 1.6  2014/08/27 22:21:06  pi
 * Interim commit
 *
 * Revision 1.5  2014/08/20 19:59:20  pi
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
 * Revision 1.13  2014/04/20 19:56:14  pi
 * Interim commit
 *
 * Revision 1.12  2014/04/20 15:03:17  pi
 * Interim commit
 *
 * Revision 1.11  2014/04/18 22:28:17  pi
 * Interim commit
 *
 * Revision 1.10  2014/04/17 22:40:54  pi
 * Interim commit
 *
 * Revision 1.9  2014/04/14 21:29:31  pi
 * Interim commit
 *
 * Revision 1.8  2014/04/12 19:12:01  pi
 * Interim commit
 *
 * Revision 1.7  2014/04/07 22:22:16  pi
 * Interim commit
 *
 * Revision 1.6  2014/03/28 22:45:46  pi
 * Interim commit
 *
 * Revision 1.5  2014/03/27 21:20:15  pi
 * Interim commit
 *
 * Revision 1.4  2014/03/21 21:48:18  pi
 * Interim commit. Preload mode working.
 *
 * Revision 1.3  2014/03/21 18:48:32  pi
 * Interim commit
 *
 * Revision 1.2  2014/03/09 21:52:46  pi
 * Interim commit
 *
 * Revision 1.1  2014/03/08 18:11:16  pi
 * Initial revision
 *
 *
 *
 */
