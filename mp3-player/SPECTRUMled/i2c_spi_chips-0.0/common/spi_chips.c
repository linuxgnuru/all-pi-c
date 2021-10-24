/* C source file associated with SPI CHIP Structures etc.
 * $Id: spi_chips.c,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "spi_chips.h"

unsigned int init_mcp23S08(mcp23S08 *chip,
			   unsigned int waddr,
			   char spidev,
			   char *chip_name)
{
  unsigned int lcw;
  lcw = 0x40;
  if (waddr > 0x40) lcw = waddr;
  if (lcw > 0x46) return(0);
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  (*chip).waddr = lcw;
  (*chip).iocon = 0x05;
  (*chip).iodir = 0x00;
  (*chip).iopol = 0x01;
  (*chip).gpio  = 0x09;
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
  return(lcw);
}

void echo_mcp23S08_status(mcp23S08 chip)
{
  printf("mcp23S08 chip status:\n");
  printf("SPI_DEV %s\n", chip.spidev);
  printf("WADDR   %02x\n", chip.waddr);
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

void init_max7219(max7219 *chip,
		  char spidev,
		  char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  /* NB: Register "don't care bits" always set = 0 ...*/
  (*chip).NoOp        = 0x00;
  (*chip).Digit0      = 0x01;
  (*chip).Digit1      = 0x02;
  (*chip).Digit2      = 0x03;
  (*chip).Digit3      = 0x04;
  (*chip).Digit4      = 0x05;
  (*chip).Digit5      = 0x06;
  (*chip).Digit6      = 0x07;
  (*chip).Digit7      = 0x08;
  (*chip).DecodeMode  = 0x09;
  (*chip).Intensity   = 0x0A;
  (*chip).ScanLimit   = 0x0B;
  (*chip).Shutdown    = 0x0C;
  (*chip).DisplayTest = 0x0F;
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}

void echo_max7219_status(max7219 chip)
{
  printf("max7219 chip status:\n");
  printf("SPI_DEV     %s\n", chip.spidev);
  printf("NOOP        %02x\n", chip.NoOp);
  printf("DIGIT0      %02x\n", chip.Digit0);
  printf("DIGIT1      %02x\n", chip.Digit1);
  printf("DIGIT2      %02x\n", chip.Digit2);
  printf("DIGIT3      %02x\n", chip.Digit3);
  printf("DIGIT4      %02x\n", chip.Digit4);
  printf("DIGIT5      %02x\n", chip.Digit5);
  printf("DIGIT6      %02x\n", chip.Digit6);
  printf("DIGIT7      %02x\n", chip.Digit7);
  printf("DECODEMODE  %02x\n", chip.DecodeMode);
  printf("INTENSITY   %02x\n", chip.Intensity);
  printf("SCANLIMIT   %02x\n", chip.ScanLimit);
  printf("SHUTDOWN    %02x\n", chip.Shutdown);
  printf("DISPLAYTEST %02x\n", chip.DisplayTest);
  printf("NAME        %s\n", chip.name);
}

void init_max7221(max7221 *chip,
		  char spidev,
		  char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  /* NB: Register "don't care bits" always set = 0 ...*/
  (*chip).NoOp        = 0x00;
  (*chip).Digit0      = 0x01;
  (*chip).Digit1      = 0x02;
  (*chip).Digit2      = 0x03;
  (*chip).Digit3      = 0x04;
  (*chip).Digit4      = 0x05;
  (*chip).Digit5      = 0x06;
  (*chip).Digit6      = 0x07;
  (*chip).Digit7      = 0x08;
  (*chip).DecodeMode  = 0x09;
  (*chip).Intensity   = 0x0A;
  (*chip).ScanLimit   = 0x0B;
  (*chip).Shutdown    = 0x0C;
  (*chip).DisplayTest = 0x0F;
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}

void echo_max7221_status(max7221 chip)
{
  printf("max7221 chip status:\n");
  printf("SPI_DEV     %s\n", chip.spidev);
  printf("NOOP        %02x\n", chip.NoOp);
  printf("DIGIT0      %02x\n", chip.Digit0);
  printf("DIGIT1      %02x\n", chip.Digit1);
  printf("DIGIT2      %02x\n", chip.Digit2);
  printf("DIGIT3      %02x\n", chip.Digit3);
  printf("DIGIT4      %02x\n", chip.Digit4);
  printf("DIGIT5      %02x\n", chip.Digit5);
  printf("DIGIT6      %02x\n", chip.Digit6);
  printf("DIGIT7      %02x\n", chip.Digit7);
  printf("DECODEMODE  %02x\n", chip.DecodeMode);
  printf("INTENSITY   %02x\n", chip.Intensity);
  printf("SCANLIMIT   %02x\n", chip.ScanLimit);
  printf("SHUTDOWN    %02x\n", chip.Shutdown);
  printf("DISPLAYTEST %02x\n", chip.DisplayTest);
  printf("NAME        %s\n", chip.name);
}

void init_pcd8544(pcd8544 *chip,
		  int rst_gpio,
		  int dc_gpio,
		  int bl_gpio,
		  char spidev,
		  char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  if (rst_gpio > 0) (*chip).rst_gpio = rst_gpio;
  else (*chip).rst_gpio = GPIO_UNASSIGNED;
  if (dc_gpio > 0) (*chip).dc_gpio = dc_gpio;
  else (*chip).dc_gpio = GPIO_UNASSIGNED;
  if (bl_gpio > 0) (*chip).bl_gpio = bl_gpio;
  else (*chip).bl_gpio = GPIO_UNASSIGNED;
  (*chip).dcval = 0x00;
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}

void echo_pcd8544_status(pcd8544 chip)
{
  printf("pcd8544 chip status:\n");
  printf("SPI_DEV  %s\n", chip.spidev);
  printf("RST GPIO %02d\n", chip.rst_gpio);
  printf("DC  GPIO %02d\n", chip.dc_gpio);
  printf("BL  GPIO %02d\n", chip.bl_gpio);
  printf("DCVAL    0x%02x\n", chip.dcval);
  printf("NAME     %s\n", chip.name);
}

void init_bbpcd8544(bbpcd8544 *chip,
		    int rst_wpin,
		    int ce_wpin,
		    int dc_wpin,
		    int din_wpin,
		    int clk_wpin,
		    int bl_wpin,
		    unsigned int speed,
		    char *chip_name)
{
  if (rst_wpin >= 0) (*chip).rst_wpin = rst_wpin;
  else (*chip).rst_wpin = GPIO_UNASSIGNED;
  if (ce_wpin >= 0) (*chip).ce_wpin = ce_wpin;
  else (*chip).ce_wpin = GPIO_UNASSIGNED;
  if (dc_wpin >= 0) (*chip).dc_wpin = dc_wpin;
  else (*chip).dc_wpin = GPIO_UNASSIGNED;
  if (din_wpin >= 0) (*chip).din_wpin = din_wpin;
  else (*chip).din_wpin = GPIO_UNASSIGNED;
  if (clk_wpin >= 0) (*chip).clk_wpin = clk_wpin;
  else (*chip).clk_wpin = GPIO_UNASSIGNED;
  if (bl_wpin >= 0) (*chip).bl_wpin = bl_wpin;
  else (*chip).bl_wpin = GPIO_UNASSIGNED;
  (*chip).dcval = 0x00;
  if (speed >= PCD8544_SLOW_CLOCK) (*chip).speed = PCD8544_SLOW_CLOCK;
  else 
    if (speed <= PCD8544_VFAST_CLOCK) (*chip).speed = PCD8544_VFAST_CLOCK;
    else
      {
	switch(speed)
	  {
	  case PCD8544_FAST_CLOCK: (*chip).speed = PCD8544_FAST_CLOCK; break;
	  case PCD8544_MEDIUM_CLOCK: 
	  default: (*chip).speed = PCD8544_MEDIUM_CLOCK; break;
	  }
      }
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}

void echo_bbpcd8544_status(bbpcd8544 chip)
{
  printf("Bit-Banged pcd8544 chip status:\n");
  printf("RST WPIN %02d\n", chip.rst_wpin);
  printf("CE  WPIN %02d\n", chip.ce_wpin);
  printf("DC  WPIN %02d\n", chip.dc_wpin);
  printf("DIN WPIN %02d\n", chip.din_wpin);
  printf("CLK WPIN %02d\n", chip.clk_wpin);
  printf("BL  WPIN %02d\n", chip.bl_wpin);
  printf("DCVAL    0x%02x\n", chip.dcval);
  printf("SPEED    %3d uS\n", chip.speed);
  printf("NAME     %s\n", chip.name);
}

/*
 * void init_bbxpcd8544(bbxpcd8544 *chip,
 * 		     int rst_gpio,
 * 		     int ce_gpio,
 * 		     int dc_gpio,
 * 		     int din_gpio,
 * 		     int clk_gpio,
 * 		     int bl_gpio,
 * 		     unsigned int speed,
 * 		     char *chip_name)
 * {
 *   if (rst_gpio >= 0) (*chip).rst_gpio = rst_gpio;
 *   else (*chip).rst_gpio = GPIO_UNASSIGNED;
 *   if (ce_gpio >= 0) (*chip).ce_gpio = ce_gpio;
 *   else (*chip).ce_gpio = GPIO_UNASSIGNED;
 *   if (dc_gpio >= 0) (*chip).dc_gpio = dc_gpio;
 *   else (*chip).dc_gpio = GPIO_UNASSIGNED;
 *   if (din_gpio >= 0) (*chip).din_gpio = din_gpio;
 *   else (*chip).din_gpio = GPIO_UNASSIGNED;
 *   if (clk_gpio >= 0) (*chip).clk_gpio = clk_gpio;
 *   else (*chip).clk_gpio = GPIO_UNASSIGNED;
 *   if (bl_gpio >= 0) (*chip).bl_gpio = bl_gpio;
 *   else (*chip).bl_gpio = GPIO_UNASSIGNED;
 *   (*chip).dcval = 0x00;
 *   if (speed >= PCD8544_SLOW_CLOCK) (*chip).speed = PCD8544_SLOW_CLOCK;
 *   else 
 *     if (speed <= PCD8544_VFAST_CLOCK) (*chip).speed = PCD8544_VFAST_CLOCK;
 *     else
 *       {
 * 	switch(speed)
 * 	  {
 * 	  case PCD8544_FAST_CLOCK: (*chip).speed = PCD8544_FAST_CLOCK; break;
 * 	  case PCD8544_MEDIUM_CLOCK: 
 * 	  default: (*chip).speed = PCD8544_MEDIUM_CLOCK; break;
 * 	  }
 *       }
 *   strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
 * }
 * 
 * void echo_bbxpcd8544_status(bbxpcd8544 chip)
 * {
 *   printf("Bit-Banged, eXported gpio, pcd8544 chip status:\n");
 *   printf("RST GPIO %02d\n", chip.rst_gpio);
 *   printf("CE  GPIO %02d\n", chip.ce_gpio);
 *   printf("DC  GPIO %02d\n", chip.dc_gpio);
 *   printf("DIN GPIO %02d\n", chip.din_gpio);
 *   printf("CLK GPIO %02d\n", chip.clk_gpio);
 *   printf("BL  GPIO %02d\n", chip.bl_gpio);
 *   printf("DCVAL    0x%02x\n", chip.dcval);
 *   printf("SPEED    %3d uS\n", chip.speed);
 *   printf("NAME     %s\n", chip.name);
 * }
 */

void init_gc74hc595(gc74hc595 *chip,
		    int goe_gpio,
		    int scl_gpio,
		    char spidev,
		    char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  if (goe_gpio > 0) (*chip).goe_gpio = goe_gpio;
  else (*chip).goe_gpio = GPIO_UNASSIGNED;
  if (scl_gpio > 0) (*chip).scl_gpio = scl_gpio;
  else (*chip).scl_gpio = GPIO_UNASSIGNED;
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}
		    
void echo_gc74hc595_status(gc74hc595 chip)
{
  printf("gc74hc595 chip status:\n");
  printf("SPI_DEV  %s\n", chip.spidev);
  printf("GOE GPIO %02d\n", chip.goe_gpio);
  printf("SCL GPIO %02d\n", chip.scl_gpio);
  printf("NAME     %s\n", chip.name);
}

void init_hw74hc595(hw74hc595 *chip,
		    char spidev,
		    char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}
		    
void echo_hw74hc595_status(hw74hc595 chip)
{
  printf("hw74hc595 chip status:\n");
  printf("SPI_DEV  %s\n", chip.spidev);
  printf("NAME     %s\n", chip.name);
}

void init_hw74hc164(hw74hc164 *chip,
		    int chainlen,
		    char spidev,
		    char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  if (chainlen > 1) (*chip).chainlen = chainlen;
  else (*chip).chainlen = 1;
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}
		    
void echo_hw74hc164_status(hw74hc164 chip)
{
  printf("hw74hc164 chip status:\n");
  printf("SPI_DEV   %s\n", chip.spidev);
  printf("CHAIN LEN %02d\n", chip.chainlen);
  printf("NAME      %s\n", chip.name);
}

void init_st7920sm(st7920sm *chip,
		   char spidev,
		   char *chip_name)
{
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  strncpy((*chip).name, chip_name, (CHIP_NAME_LEN-1));
}

void echo_st7920sm_status(st7920sm chip)
{
  printf("st7920sm chip status:\n");
  printf("SPI_DEV %s\n", chip.spidev);
  printf("NAME    %s\n", chip.name);
}

void init_spi_ssd1306_oled(spi_ssd1306_oled *chip,
			   char spidev,
			   int rst_gpio,
			   int dc_gpio,
			   char *chip_name)
{
  int i,j;
  (*chip).spidev = SPIdevNul;
  if (spidev == SPI_DEV00) (*chip).spidev = SPIdev0;
  if (spidev == SPI_DEV01) (*chip).spidev = SPIdev1;
  (*chip).spifd = NUL; /* Assigned to a wiringPi generated fd later */
  if (rst_gpio > 0) (*chip).rst_gpio = rst_gpio;
  else (*chip).rst_gpio = GPIO_UNASSIGNED;
  if (dc_gpio > 0) (*chip).dc_gpio = dc_gpio;
  else (*chip).dc_gpio = GPIO_UNASSIGNED;
  (*chip).cmds    = OLED_DISPLAY_ON;
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
}

void echo_spi_ssd1306_oled_status(spi_ssd1306_oled chip)
{
  printf("spi_ssd1306_oled status:\n");
  printf("SPI_DEV  %s\n", chip.spidev);
  printf("RST GPIO %02d\n", chip.rst_gpio);
  printf("DC  GPIO %02d\n", chip.dc_gpio);
  printf("CMDS     %02x\n", chip.cmds);
  printf("CMDD[0]  %02x\n", chip.cmdd[0]);
  printf("CMDD[1]  %02x\n", chip.cmdd[1]);
  printf("CMDT[0]  %02x\n", chip.cmdt[0]);
  printf("CMDT[1]  %02x\n", chip.cmdt[1]);
  printf("CMDT[2]  %02x\n", chip.cmdt[2]);
  printf("CMDM[0]  %02x\n", chip.cmdm[0]);
  printf("NAME     %s\n", chip.name);
}

/* Change Log */
/*
 * $Log: spi_chips.c,v $
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
 * Revision 1.10  2015/08/23 13:57:12  pi
 * st7920sm chip added
 *
 * Revision 1.9  2014/12/30 20:37:11  pi
 * Commented out unusable code
 *
 * Revision 1.8  2014/12/30 15:02:26  pi
 * Interim commit
 *
 * Revision 1.7  2014/12/22 17:00:42  pi
 * Interim commit
 *
 * Revision 1.6  2014/12/21 21:54:52  pi
 * Interim commit
 *
 * Revision 1.5  2014/12/21 20:05:18  pi
 * Beginning to add bit-banged spi support for nokia glcd
 *
 * Revision 1.4  2014/11/12 22:25:14  pi
 * Hard-wired 74hc595 defined
 *
 * Revision 1.3  2014/11/08 19:45:03  pi
 * Some tidying up and improved address assignment
 *
 * Revision 1.2  2014/07/28 22:13:48  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.6  2014/07/13 20:16:21  pi
 * Starting work on SPI 8-bit LCD interface
 *
 * Revision 1.5  2014/06/20 20:33:01  pi
 * Interim commit
 *
 * Revision 1.4  2014/06/19 21:57:05  pi
 * Interim commit
 *
 * Revision 1.3  2014/06/19 21:40:05  pi
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
