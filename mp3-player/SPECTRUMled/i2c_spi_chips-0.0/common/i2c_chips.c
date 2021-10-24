/* C source file associated with I2C CHIP Structures etc.
 * $Id: i2c_chips.c,v 1.14 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "i2c_chips.h"

/* MCP23017 */
unsigned int init_mcp23017(mcp23017 *chip,
			   unsigned int addr,
			   char *chip_name)
{
  unsigned int lca;
  lca = 0x20;
  if (addr > 0x20) lca = addr;
  if (lca > 0x27) return(0);
  (*chip).addr     = lca;
  (*chip).iocon    = 0x0a; /* assumes IOCON.BANK = 0 on power-on reset */
  (*chip).iodira   = 0x00; /* ditto */
  (*chip).iodirb   = 0x01; /* ditto */
  (*chip).ipola    = 0x02; /* ditto */
  (*chip).ipolb    = 0x03; /* ditto */
  (*chip).gpioa    = 0x12; /* ditto */
  (*chip).gpiob    = 0x13; /* ditto */
  /* Registers not yet in use >>> */
  /* All assume IOCON.BANK = 0 on power-on reset */
  /*
   * (*chip).gpintena = 0x04;
   * (*chip).defvala  = 0x06;
   * (*chip).intcona  = 0x08;
   * (*chip).gppua    = 0x0c;
   * (*chip).intfa    = 0x0e;
   * (*chip).intcapa  = 0x10;
   * (*chip).olata    = 0x14;
   * (*chip).gpintenb = 0x05;
   * (*chip).defvalb  = 0x07;
   * (*chip).intconb  = 0x09;
   * (*chip).gppub    = 0x0d;
   * (*chip).intfb    = 0x0f;
   * (*chip).intcapb  = 0x11;
   * (*chip).olatb    = 0x15;
   */
  /* <<< */
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
  return(lca);
}

void echo_mcp23017_status(mcp23017 chip)
{
  printf("mcp23017 chip status:\n");
  printf("ADDR     %02x\n", chip.addr);
  printf("IOCON    %02x\n", chip.iocon);
  printf("IODIRA   %02x\n", chip.iodira);
  printf("IODIRB   %02x\n", chip.iodirb);
  printf("IPOLA    %02x\n", chip.ipola);
  printf("IPOLB    %02x\n", chip.ipolb);
  printf("GPIOA    %02x\n", chip.gpioa);
  printf("GPIOB    %02x\n", chip.gpiob);
  /* Registers not yet in use >>> */
  /*
   * printf("GPINTENA %02x\n", chip.gpintena);
   * printf("DEFVALA  %02x\n", chip.defvala);
   * printf("INTCONA  %02x\n", chip.intcona);
   * printf("GPPUA    %02x\n", chip.gppua);
   * printf("INTFA    %02x\n", chip.intfa);
   * printf("INTCAPA  %02x\n", chip.intcapa);
   * printf("OLATA    %02x\n", chip.olata);
   * printf("GPINTENB %02x\n", chip.gpintenb);
   * printf("DEFVALB  %02x\n", chip.defvalb);
   * printf("INTCONB  %02x\n", chip.intconb);
   * printf("GPPUB    %02x\n", chip.gppub);
   * printf("INTFB    %02x\n", chip.intfb);
   * printf("INTCAPB  %02x\n", chip.intcapb);
   * printf("OLATB    %02x\n", chip.olatb);
   */
  /* <<< */
  printf("NAME     %s\n", chip.name);
}

/* MCP23008 */
unsigned int init_mcp23008(mcp23008 *chip,
			   unsigned int addr,
			   char *chip_name)
{
  unsigned int lca;
  lca = 0x20;
  if (addr > 0x20) lca = addr;
  if (lca > 0x27) return(0);
  (*chip).addr    = lca;
  (*chip).iocon   = 0x05;
  (*chip).iodir   = 0x00;
  (*chip).iopol   = 0x01;
  (*chip).gpio    = 0x09;
  /* Registers not yet in use >>> */
  /*
   * (*chip).gpinten = 0x02;
   * (*chip).defval  = 0x03;
   * (*chip).intcon  = 0x04;
   * (*chip).gppu    = 0x06;
   * (*chip).intf    = 0x07;
   * (*chip).intcap  = 0x08;
   * (*chip).olat    = 0x0a;
   */
  /* <<< */
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
  return(lca);
}

void echo_mcp23008_status(mcp23008 chip)
{
  printf("mcp23008 chip status:\n");
  printf("ADDR    %02x\n", chip.addr);
  printf("IOCON   %02x\n", chip.iocon  );
  printf("IODIR   %02x\n", chip.iodir);
  printf("IOPOL   %02x\n", chip.iopol);
  printf("GPIO    %02x\n", chip.gpio);
  /* Registers not yet in use >>> */
  /*
   * printf("GPINTEN %02x\n", chip.gpinten);
   * printf("DEFVAL  %02x\n", chip.defval );
   * printf("INTCON  %02x\n", chip.intcon );
   * printf("GPPU    %02x\n", chip.gppu   );
   * printf("INTF    %02x\n", chip.intf   );
   * printf("INTCAP  %02x\n", chip.intcap );
   * printf("OLAT    %02x\n", chip.olat   );
   */
  /* <<< */
  printf("NAME    %s\n", chip.name);
}

/* PCF8574 */
/* PCF8574A  address range 0x20 - 0x27 */
/* PCF8574AT address range 0x38 - 0x3f */
unsigned int init_pcf8574(pcf8574 *chip,
			  unsigned int addr,
			  char *chip_name)
{
  unsigned int lca;
  lca = 0x20;
  if (addr > 0x20) lca = addr;
  if ((lca > 0x27) && (lca < 0x38)) return(0);
  if (lca > 0x3f) return(0);
  (*chip).addr = lca;
  (*chip).qiop = 0xff; /* All bits high, input possible */
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
  return(lca);
}

void echo_pcf8574_status(pcf8574 chip)
{
  printf("pcf8574 chip status:\n");
  printf("ADDR    %02x\n", chip.addr);
  printf("QIOP    %02x\n", chip.qiop);
  printf("NAME    %s\n", chip.name);
}

unsigned int init_lcdbpv2(lcdbpv2 *chip,
			  unsigned int addr,
			  char *chip_name)
{
  unsigned int lca;
  lca = 0x3a;
  if (addr > 0) lca = addr;
  if (lca > 0x7f) return(0);
  (*chip).addr        = lca;
  (*chip).printstr    = 1;
  (*chip).cursorposn  = 2;
  (*chip).clearline   = 3;
  (*chip).cleardisp   = 4;
  (*chip).lcdtype     = 5;
  (*chip).hd44780     = 6;
  (*chip).backlight   = 7;
  (*chip).writechar   = 10;
  (*chip).setbaudrate = 33;
  (*chip).createchar  = 64;
  /* Not in use by I2C */
  /*
   * (*chip).changei2caddr = 32;
   */
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
  return(lca);
}

void echo_lcdbpv2_status(lcdbpv2 chip)
{
  printf("lcdbpv2 module status:\n");
  printf("ADDR        %02x\n", chip.addr);
  printf("PRINTSTR    %2d\n", chip.printstr);
  printf("CURSORPOSN  %2d\n", chip.cursorposn);
  printf("CLEARLINE   %2d\n", chip.clearline);
  printf("CLEARDISP   %2d\n", chip.cleardisp);
  printf("LCDTYPE     %2d\n", chip.lcdtype);
  printf("HD44780     %2d\n", chip.hd44780);
  printf("BACKLIGHT   %2d\n", chip.backlight);
  printf("WRITECHAR   %2d\n", chip.writechar);
  printf("SETBAUDRATE %2d\n", chip.setbaudrate);
  printf("CREATECHAR  %2d\n", chip.createchar);
  printf("NAME        %s\n", chip.name);
}

/* I2C version of SSD1306 OLED */
unsigned init_i2c_ssd1306_oled(i2c_ssd1306_oled *chip,
			       unsigned int addr,
			       char *chip_name)
{
  unsigned int lca;
  int i,j;
  lca = 0x3c;
  if (addr < 0x3c) return(0); 
  if (addr > 0x3d) return(0); 
  if (addr != 0x3c) lca = addr;
  (*chip).addr = lca;
  (*chip).c0dc = OLED_COMMAND;
  (*chip).cmds = OLED_DISPLAY_ON;
  (*chip).cmdd[0] = OLED_SET_MEM_ADDR_MODE;
  (*chip).cmdd[1] = 0x00; /* Horizontal Addressing Mode */
  (*chip).cmdt[0] = OLED_SET_PAGE_ADDR;
  (*chip).cmdt[1] = 0; /* Page Start */
  (*chip).cmdt[2] = 7; /* Page End   */
  (*chip).cmdm[0] = 0; /* Not in use */
  /* Clear all data stores ... */
  for (i=0; i<8; i++) (*chip).cfont[i] = 0;
  for (i=0; i<8; i++)
    {
      for (j=0; j<128; j++) (*chip).page[i][j] = 0;
    }
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
  return(lca);
}

void echo_i2c_ssd1306_oled_status(i2c_ssd1306_oled chip)
{
  printf("i2c_ssd1306_oled status:\n");
  printf("ADDR    %02x\n", chip.addr);
  printf("C0DC    %02x\n", chip.c0dc);
  printf("CMDS    %02x\n", chip.cmds);
  printf("CMDD[0] %02x\n", chip.cmdd[0]);
  printf("CMDD[1] %02x\n", chip.cmdd[1]);
  printf("CMDT[0] %02x\n", chip.cmdt[0]);
  printf("CMDT[1] %02x\n", chip.cmdt[1]);
  printf("CMDT[2] %02x\n", chip.cmdt[2]);
  printf("CMDM[0] %02x\n", chip.cmdm[0]);
  printf("NAME    %s\n", chip.name);
}
			       
/* HT16K33 */

unsigned init_ht16k33(ht16k33 *chip,
		      unsigned int addr,
		      unsigned int conf,
		      char *chip_name)
{
  unsigned int lca,lcc;
  int i;
  lca = 0x70;
  if (addr < 0x70) return(0); 
  if (addr > 0x77) return(0); 
  if (addr != 0x70) lca = addr;
  (*chip).addr = lca;
  lcc = MTX_8X8;
  if ((conf > MTX_8X8) && (conf <= C4DGTRGB)) lcc = conf;
  (*chip).conf = lcc;
  (*chip).ddap = DDAP_ADDR;
  (*chip).ddcb = ALLCOLON;
  (*chip).ssr  = SSR_NORMAL;
  (*chip).risr = RISR_ROWD;
  (*chip).dspr = (DSPR_ADDR | DISPONMSK) & BLINKOFF;
  (*chip).dmsr = (DIMSETMASK | DIM08_16);
  /* Initialise data stores ... */
  for (i=0; i<8; i++) (*chip).ccfont[i] = 0;
  (*chip).crfont[0] = 0x01;
  (*chip).crfont[1] = 0x7e;
  (*chip).crfont[2] = 0xa4;
  (*chip).crfont[3] = 0x24;
  (*chip).crfont[4] = 0x24;
  (*chip).crfont[5] = 0x24;
  (*chip).crfont[6] = 0x42;
  (*chip).crfont[7] = 0;
  (*chip).digits[0] = ALLSEGOFF;
  (*chip).digits[1] = ALLSEGOFF;
  (*chip).digits[2] = ALLSEGOFF;
  (*chip).digits[3] = ALLSEGOFF;
  (*chip).rgb       = ALLRGBOFF;
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
  return(lca);
}

void echo_ht16k33_status(ht16k33 chip)
{
  int i,j;
  unsigned int lbit,mask;
  printf("ht16k33 chip status:\n");
  printf("ADDR %02x\n", chip.addr);
  printf("CONF %02d\n", chip.conf);
  printf("DDAP %02x\n", chip.ddap);
  printf("DDCB %02x\n", chip.ddcb);
  printf("SSR  %02x\n", chip.ssr);
  printf("RISR %02x\n", chip.risr);
  printf("DSPR %02x\n", chip.dspr);
  printf("DMSR %02x\n", chip.dmsr);
  printf("NAME %s\n", chip.name);
  switch (chip.conf)
    {
    case C4DGTRGB:
      {
	for (i=0; i<4; i++)
	  {
	    if (i == 0) printf("DIGIT ");
	    else printf("      ");
	    printf("%02x\n", chip.digits[i]);
	  }
	printf("RGB   %02x\n", chip.rgb);
	break;
      }	
    case MTX_8X8:
    default:
      {
	for (i=0; i<8; i++)
	  {
	    mask = 0x80; /* MSB */
	    if (i == 0) printf("CRFONT ");
	    else printf("       ");
	    for (j=0; j<8; j++)
	      {
		lbit = chip.crfont[i] & mask;
		if (lbit != 0) lbit = 1;
		printf("%1d", lbit);
		mask = mask >> 1;
		if (j == 7) printf("\n");
	      }
	  }
	for (i=0; i<8; i++)
	  {
	    mask = 0x80; /* MSB */
	    if (i == 0) printf("CCFONT ");
	    else printf("       ");
	    for (j=0; j<8; j++)
	      {
		lbit = chip.ccfont[i] & mask;
		if (lbit != 0) lbit = 1;
		printf("%1d", lbit);
		mask = mask >> 1;
		if (j == 7) printf("\n");
	      }
	  }
	break;
      }
    }
}

void convert_ht16k33_font(ht16k33 *chip)
{
  int i;
  unsigned int lbit,mask;
  for (i=0; i<8; i++)
    {
      mask = 0x80; /* MSB */
      (*chip).ccfont[i] = 0;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL0MASK);
      mask = 0x40;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL1MASK);
      mask = 0x20;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL2MASK);
      mask = 0x10;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL3MASK);
      mask = 0x08;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL4MASK);
      mask = 0x04;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL5MASK);
      mask = 0x02;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL6MASK);
      mask = 0x01;
      lbit = (*chip).crfont[i] & mask;
      if (lbit != 0) (*chip).ccfont[i] = ((*chip).ccfont[i] | COL7MASK);
     }
}

void load_ht16k33_crfont(ht16k33 *chip,
			 char *thisfont,
			 unsigned int fidx,
			 int nrow)
{
  unsigned int lfidx;
  int i,lrow;
  lfidx = fidx * nrow;
  lrow = 0;
  for (i=0; i<8; i++) (*chip).crfont[i] = 0;
  for (i=0; i<8; i++)
    {
      (*chip).crfont[i] = (unsigned int)(thisfont[lfidx]);
      lfidx++;
      lrow++;
      if (lrow == nrow) return;
    }
}

void load_ht16k33_digits(ht16k33 *chip,
			 unsigned int digits[4])
{
  int i;
  for (i=0; i<4; i++) (*chip).digits[i] = ALLSEGOFF;
  for (i=0; i<4; i++) (*chip).digits[i] = digits[i];
}

void load_ht16k33_rgb(ht16k33 *chip,
		      unsigned int rgb)
{
  (*chip).rgb = ALLRGBOFF;
  (*chip).rgb = rgb;
}

/* Change Log */
/*
 * $Log: i2c_chips.c,v $
 * Revision 1.14  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.13  2017/12/19 17:00:19  pi
 * HT16K33 I2C addr range corrected
 *
 * Revision 1.12  2017/12/13 22:46:54  pi
 * Interim commit
 *
 * Revision 1.11  2017/12/11 22:13:14  pi
 * Interim commit
 *
 * Revision 1.10  2017/08/19 11:34:58  pi
 * Interim Commit
 *
 * Revision 1.9  2017/08/08 19:11:31  pi
 * Interim commit
 *
 * Revision 1.8  2017/07/19 18:08:31  pi
 * Interim Commit
 *
 * Revision 1.7  2017/07/18 20:45:16  pi
 * Interim Commit
 *
 * Revision 1.6  2017/07/18 15:41:35  pi
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
 * Revision 1.3  2015/04/16 16:44:58  pi
 * SSD1306 oled "chip" added
 *
 * Revision 1.2  2014/11/08 19:45:03  pi
 * Some tidying up and improved address assignment
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.3  2014/03/21 18:48:32  pi
 * Interim commit
 *
 * Revision 1.2  2014/03/01 18:21:13  pi
 * New routines etc. for LCD backpack device added
 *
 * Revision 1.1  2013/06/03 20:58:09  pi
 * Initial revision
 *
 * Revision 1.4  2013/05/25 22:43:42  pi
 * Added skeleton code for other MCP23008/23017 registers
 *
 * Revision 1.3  2013/05/15 16:11:20  pi
 * Interim commit
 *
 * Revision 1.2  2013/05/15 15:36:14  pi
 * Interim commit
 *
 * Revision 1.1  2013/05/15 15:08:57  pi
 * Initial revision
 *
 *
 *
 *
 */
