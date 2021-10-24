/* LCD STRUCTURES LISTing program
 * $Id: lcd_structures_list.c,v 1.3 2018/03/31 21:32:45 pi Exp $
 */

#define NO_ECHO  1

#define IS_MAIN           1
#define BLK_LEN          83  /* 80 + CR, LF & NUL */
#define BUF_LEN         512
#define CMD_LEN         138  /* 2 x FN_LEN + 10   */
#define FN_LEN           64

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/utsname.h>
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
#include <wiringPiSPI.h>
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "spi_chips.h"
#include "spi_wraps.h"
#include "i2c_wraps.h"
#include "lcd_disptools.h"
/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: lcd_structures_list.c,v $";
static char RCSId[]       = "$Id";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.3 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.06 $";

void setup_RCSlist(void)
{
  RCSlist[AUTHOR_RCS] = RCSAuthor;
  RCSlist[AUTHOR_SCM] = SCMAuthor;
  RCSlist[DATE]       = RCSDate;
  RCSlist[FILE_RCS]   = RCSFile;
  RCSlist[ID]         = RCSId;
  RCSlist[NAME]       = RCSName;
  RCSlist[REVISION]   = RCSRevision;
  RCSlist[STATE]      = RCSState;
  RCSlist[SVERSION]   = SCMVersion;
  RCSlist[LAST_RCS]   = "$$";
}

/* Main body of code */
int main(int argc,
	 char **argv)
{
  int mca,mcw;
  i2c_ssd1306_oled local_oled;
  setup_RCSlist();

  /* printf("getRCSVersion reports %s\n", getRCSVersion()); */

  /* Software Configuration Management Info. & Usage */
  print_scm(getRCSFile(FALSE),
	    getSCMAuthor(),
	    getSCMVersion(),
	    getRCSState(),
	    getRCSDate());
  /* Optional package info. etc. ... */
#ifdef HAVE_CONFIG_H
  print_pkg_info(getPKGName(), getPKGVersion());
#endif /* HAVE_CONFIG_H */

  printf("lcd_iface_8bit:\n");
  mca = init_any_iface_8bit(&current_iface_8bit,
			    LCM1602,
			    27,
			    22,
			    0x26,
			    "HD44780U");
  echo_any_iface_8bit_status(&current_iface_8bit);
  printf("\nlcd_fi2c_4bit:\n");
  mca = init_any_fi2c_4bit(&current_fi2c_4bit,
			   LCM1602,
			   0x27,
			   "HD44780U");
  echo_any_fi2c_4bit_status(&current_fi2c_4bit);
  printf("\nlcd_mi2c_4bit:\n");
  mca = init_any_mi2c_4bit(&current_mi2c_4bit,
			   LCM1602,
			   "HD44780U");
  echo_any_mi2c_4bit_status(&current_mi2c_4bit);
  printf("\nlcd_ibo_i2c_4bit:\n");
  mca = init_any_ibo_i2c_4bit(&current_ibo_i2c_4bit,
			   LCM1602,
			   IBO_PCF8574AP,
			   "HD44780U");
  echo_any_ibo_i2c_4bit_status(&current_ibo_i2c_4bit);
  printf("\ndual_lcd:\n");
  mca = init_any_dual_lcd(&current_dual_lcd,
			  0x20,
			  LCM1602,
			  22,
			  17,
			  LCM1602,
			  23,
			  18,
			  "HD44780U");
  echo_any_dual_lcd_status(&current_dual_lcd);
  printf("\nlcd_spi_8bit:\n");
  init_current_spi_8bit(&current_spi_8bit,
			LCM1602,
			22,
			27,
			17,
			18,
			SPI_DEV00,
			"HD44780U");
  echo_any_spi_8bit_status(&current_spi_8bit);
  printf("\nlcd_spi_4bit:\n");
  mcw = init_current_spi_4bit(&current_spi_4bit,
			      LCM1602,
			      22,
			      0x40,
			      SPI_DEV01,
			      "HD44780U");
  echo_any_spi_4bit_status(&current_spi_4bit);
  printf("\nlcd_spi_4bitV2:\n");
  init_current_spi_4bitV2(&current_spi_4bitV2,
			  LCM1602,
			  27,
			  SPI_DEV00,
			  "HD44780U");
  echo_any_spi_4bitV2_status(&current_spi_4bitV2);
  printf("\nlcd_gpio_4bit:\n");
  init_current_gpio_4bit(&current_gpio_4bit,
			 LCM1602,
			 17,
			 18,
			 27,
			 22,
			 23,
			 24,
			 25,
			 4,
			 "HD44780U");
  echo_any_gpio_4bit_status(&current_gpio_4bit);
  printf("\nglcd_spi2p_8bit:\n");
  init_current_spi2p_8bit(&current_spi2p_8bit,
			  25,
			  24,
			  23,
			  27,
			  22,
			  SPI_DEV01,
			  "QC12864B/ST7920");
  echo_any_spi2p_8bit_status(&current_spi2p_8bit);
  printf("\ni2c_ssd1306_oled:\n");
  init_i2c_ssd1306_oled(&local_oled,
			0x3c,
			"KEYES");
  echo_i2c_ssd1306_oled_status(local_oled);
  printf("\nglcd_spibl2:\n");
  init_current_spibl2(&current_spibl2,
		      4,
		      17,
		      SPI_DEV01,
		      "12864ZW/ST7920");		      
  echo_any_spibl2_status(&current_spibl2);
  exit(EXIT_SUCCESS);
}

/*
 * $Log: lcd_structures_list.c,v $
 * Revision 1.3  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.2  2016/11/12 11:31:48  pi
 * SCMVersion 0.06
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.7  2015/08/23 21:09:08  pi
 * glcd spibl2 structure added
 *
 * Revision 1.6  2015/04/16 16:46:06  pi
 * SSD1306 oled "chip" added
 * SCMVersion 0.04
 *
 * Revision 1.5  2015/02/06 21:08:39  pi
 * Mjkdz I2C lcd1602 module support added
 *
 * Revision 1.4  2014/12/04 21:29:28  pi
 * Interim commit
 *
 * Revision 1.3  2014/11/12 22:28:08  pi
 * SCMVersion 0.01
 *
 * Revision 1.2  2014/11/11 18:54:34  pi
 * SCMVersion 0.00
 *
 * Revision 1.1  2014/11/11 13:59:13  pi
 * Initial revision
 *
 *
 *
 *
 */
