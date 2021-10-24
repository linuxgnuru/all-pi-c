/* C source file associated with I2C WRAPperS etc.
 * $Id: i2c_wraps.c,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "i2c_wraps.h"

void C2S_i2cdetect(int chan,
		   char qx)
{
  static char i2c_command[CMD_LEN];
  strcpy(i2c_command, "i2cdetect -y");
  switch(chan)
    {
    case PI_B1: strcat(i2c_command, " 0"); break;
    case PI_B2:
    default: strcat(i2c_command, " 1"); break;
    }
  if (!qx) printf("%s\n", i2c_command);
  else system(i2c_command);
}

void C2S_i2cset(int chan,
		unsigned int addr,
		unsigned int regr,
		unsigned int data,
		char qx)
{
  static char i2c_command[CMD_LEN];
  static char hexstr[8];
  strcpy(i2c_command, "i2cset -y");
  switch(chan)
    {
    case PI_B1: strcat(i2c_command, " 0"); break;
    case PI_B2:
    default: strcat(i2c_command, " 1"); break;
    }
  sprintf(hexstr, " 0x%02x\0", addr);
  strncat(i2c_command, hexstr, 7);
  sprintf(hexstr, " 0x%02x\0", regr);
  strncat(i2c_command, hexstr, 7);
  sprintf(hexstr, " 0x%02x\0", data);
  strncat(i2c_command, hexstr, 7);
  if (!qx) printf("%s\n", i2c_command);
  else system(i2c_command);
}

void C2S_i2cseti(int chan,
		 unsigned int addr,
		 unsigned int regr,
		 unsigned int data[16],
		 unsigned int nval,
		 char qx)
{
  static char i2c_command[CMD_LEN];
  static char hexstr[8];
  static int i,nv;
  if (nval < 17) nv = nval;
  else nval = 16;
  strcpy(i2c_command, "i2cset -y");
  switch(chan)
    {
    case PI_B1: strcat(i2c_command, " 0"); break;
    case PI_B2:
    default: strcat(i2c_command, " 1"); break;
    }
  sprintf(hexstr, " 0x%02x\0", addr);
  strncat(i2c_command, hexstr, 7);
  sprintf(hexstr, " 0x%02x\0", regr);
  strncat(i2c_command, hexstr, 7);
  for (i=0; i<nval; i++)
    {
      sprintf(hexstr, " 0x%02x\0", data[i]);
      strncat(i2c_command, hexstr, 7);
    }
  strcat(i2c_command, " i");
  if (!qx) printf("%s\n", i2c_command);
  else system(i2c_command);
}

void C2S_i2cout(int chan,
		unsigned int addr,
		unsigned int data,
		char qx)
{
  static char i2c_command[CMD_LEN];
  static char hexstr[8];
  strcpy(i2c_command, "i2cset -y");
  switch(chan)
    {
    case PI_B1: strcat(i2c_command, " 0"); break;
    case PI_B2:
    default: strcat(i2c_command, " 1"); break;
    }
  sprintf(hexstr, " 0x%02x\0", addr);
  strncat(i2c_command, hexstr, 7);
  sprintf(hexstr, " 0x%02x\0", data);
  strncat(i2c_command, hexstr, 7);
  if (!qx) printf("%s\n", i2c_command);
  else system(i2c_command);
}

#ifdef HAVE_WIRINGPI_H
void C2S_gpio(char *options,
	      char qx)
{
  static char gpio_command[CMD_LEN];
  strcpy(gpio_command, "gpio ");
  strncat(gpio_command, options, (CMD_LEN-6));
  if (!qx) printf("%s\n", gpio_command);
  else system(gpio_command);
}

void C2S_gpio_export(int gpio_pin,
		     int gpio_mode,
		     char qx)
{
  static char sub_command[CMD_LEN];
  static char pinstr[5];
  strcpy(sub_command, "export ");
  sprintf(pinstr, " %02d \0", gpio_pin);
  strcat(sub_command, pinstr);
  if (gpio_mode == GPIO_XIN) strcat(sub_command, "in");
  if (gpio_mode == GPIO_XOUT) strcat(sub_command, "out");
  /* else strcat(sub_command, "in"); */
  C2S_gpio(sub_command, qx);
}

void C2S_gpio_unexport(int gpio_pin,
		       char qx)
{
  static char sub_command[CMD_LEN];
  static char pinstr[4];
  strcpy(sub_command, "unexport ");
  sprintf(pinstr, " %02d\0", gpio_pin);
  strcat(sub_command, pinstr);
  C2S_gpio(sub_command, qx);
}

void C2S_gpio_unexportall(char qx)
{
  C2S_gpio("unexportall", qx);
}

void C2S_gpio_g_mode(int gpio_gpin,
		     int gpio_mode,
		     char qx)
{ 
  static char sub_command[CMD_LEN];
  static char pinstr[4];
  strcpy(sub_command, "-g mode");
  sprintf(pinstr, " %02d ", gpio_gpin);
  strcat(sub_command, pinstr);
  if (gpio_mode == GPIO_MODE_IN) strcat(sub_command, "in");
  if (gpio_mode == GPIO_MODE_OUT) strcat(sub_command, "out");
  C2S_gpio(sub_command, qx);
}

void C2S_gpio_mode(int gpio_wpin,
		   int gpio_mode,
		   char qx)
{
  static char sub_command[CMD_LEN];
  static char pinstr[4];
  strcpy(sub_command, "mode");
  sprintf(pinstr, " %02d ", gpio_wpin);
  strcat(sub_command, pinstr);
  if (gpio_mode == GPIO_MODE_IN) strcat(sub_command, "in");
  if (gpio_mode == GPIO_MODE_OUT) strcat(sub_command, "out");
  C2S_gpio(sub_command, qx);
}

void C2S_gpio_g_write(int gpio_gpin,
		     int gpio_level,
		     char qx)
{
  static char sub_command[CMD_LEN];
  static char pinstr[4];
  strcpy(sub_command, "-g write");
  sprintf(pinstr, " %02d", gpio_gpin);
  strcat(sub_command, pinstr);
  sprintf(pinstr, " %1d\0", gpio_level);
  strcat(sub_command, pinstr);
  C2S_gpio(sub_command, qx);
}

void C2S_gpio_write(int gpio_wpin,
		    int gpio_level,
		    char qx)
{
  static char sub_command[CMD_LEN];
  static char pinstr[4];
  strcpy(sub_command, "write");
  sprintf(pinstr, " %02d", gpio_wpin);
  strcat(sub_command, pinstr);
  sprintf(pinstr, " %1d\0", gpio_level);
  strcat(sub_command, pinstr);
  C2S_gpio(sub_command, qx);
}

void C2S_gpio_wb(unsigned int gpio_byte,
		 char qx)
{
  static char sub_command[CMD_LEN];
  static char bytestr[4];
  strcpy(sub_command, "wb");
  sprintf(bytestr, " 0x%02x", gpio_byte);
  strcat(sub_command, bytestr);
  C2S_gpio(sub_command, qx);
}

void WP_pinMode(int wpin,
		int mode,
		char qx)
{
  if (qx) pinMode(wpin, mode);
  else printf("pinMode(#%02d, %01d)\n",
	      wpin,
	      mode);
}

void WP_digitalWrite(int gpio_pin,
		     int value,
		     char qx)
{
  if (qx) digitalWrite(gpio_pin, value);
  else printf("digitalWrite(#%02d, %01d)\n",
	      gpio_pin,
	      value);
}

void WP_digitalWriteByte(int value,
			 char qx)
{
  if (qx) digitalWriteByte(value);
  else printf("digitalWriteByte(0x%02x)\n", value);
}
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_WIRINGPII2C_H

void WP_i2cset(wpi2cvb wpi2c,
	       char qx)
{  
  int lflag;
  if (qx)
    {
      if ((lflag = wiringPiI2CWriteReg8(wpi2c.fd, wpi2c.reg, wpi2c.data)) < 0)
	{
	  fprintf(stderr, "wiringPiI2CWriteReg8 failed ... ");
	  perror("wiringPiI2CWriteReg8");
	}
    }
  else printf("wiringPiI2CWriteReg8: fd=%d reg=0x%02x data=0x%02x\n",
	      wpi2c.fd,
	      wpi2c.reg,
	      wpi2c.data);
}

void WP_i2cset2(wpi2cvb wpi2c,
		char qx)
{  
  int lflag;
  if (qx)
    {
      if ((lflag = wiringPiI2CWriteReg16(wpi2c.fd, wpi2c.reg, wpi2c.data)) < 0)
	{
	  fprintf(stderr, "wiringPiI2CWriteReg16 failed ... ");
	  perror("wiringPiI2CWriteReg16");
	}
    }
  else printf("wiringPiI2CWriteReg16: fd=%d reg=0x%02x data=0x%04x\n",
	      wpi2c.fd,
	      wpi2c.reg,
	      wpi2c.data);
}

/* This may not be working correctly !!! 2015.04.19 */
void WP_i2cseti(wpi2cvb wpi2c,
		int nbytes,
		char qx)
{
  int i;
  wpi2cvb lwpi2c;
  lwpi2c.did = wpi2c.did;
  lwpi2c.fd  = wpi2c.fd;
  lwpi2c.reg = wpi2c.reg;
  lwpi2c.data = (int)(wpi2c.ibytes[0]);
  WP_i2cset(lwpi2c, qx);
  if (nbytes > WP_MAX_IBYTES) nbytes = WP_MAX_IBYTES;
  for (i=1; i<nbytes; i++)
    {
      lwpi2c.data = (int)(wpi2c.ibytes[i]);
      WP_i2cout(lwpi2c, qx);
      delay(10);
    }
}

void WP_i2cout(wpi2cvb wpi2c,
	       char qx)
{
  int lflag;
  /* NB: Does not use wpi2c.reg! */
  if (qx)
    {
      if ((lflag = wiringPiI2CWrite(wpi2c.fd, wpi2c.data)) < 0)
	{
	  fprintf(stderr, "wiringPiI2CWrite failed ... ");
	  perror("wiringPiI2CWrite");
	}
    }
  else printf("wiringPiI2CWrite: fd=%d data=0x%02x\n",
	      wpi2c.fd,
	      wpi2c.data);
}

int WP_i2cget(wpi2cvb wpi2c,
	      char qx)
{
  int ldata;
  /* NB: Does not use wpi2c.data! */
  ldata = -1;
  if (qx)
    {
      if ((ldata = wiringPiI2CReadReg8(wpi2c.fd, wpi2c.reg)) < 0)
	{
	  fprintf(stderr, "wiringPiI2CReadReg8 failed ... ");
	  perror("wiringPiI2CReadReg8");
	}
    }
  else printf("wiringPiI2CReadReg8: fd=%d reg=0x%02x ...\n",
	      wpi2c.fd,
	      wpi2c.reg);
  return(ldata);
}

#endif /* HAVE_WIRINGPII2C_H */

/*
#ifdef HAVE_PIGPIO_H

void PG_i2cset(pgi2cvb pgi2c,
	       char qx)
{
  int lflag;
  if (qx)
    {
      if ((lflag = i2cWriteByteData(pgi2c.handle, pgi2c.i2cReg, pgi2c.bVal)) != 0)
	{
	  fprintf(stderr, "i2cWriteByteData failed ... ");
	  perror("i2cWriteByteData");
	}
    }
  else printf("i2cWriteByteData: handle=%d i2cReg=0x%02x bVal=0x%02x\n",
	      pgi2c.handle,
	      pgi2c.i2cReg,
	      pgi2c.bVal);
}
  
void PG_i2cset2(pgi2cvb pgi2c,
		char qx)
{  
  int lflag;
  if (qx)
    {
      if ((lflag = i2cWriteWordData(pgi2c.handle, pgi2c.i2cReg, pgi2c.wVal)) != 0)
	{
	  fprintf(stderr, "i2cWriteWordData failed ... ");
	  perror("i2cWriteWordData");
	} 
    }
  else printf("i2cWriteWordData: handle=%d i2cReg=0x%02x wVal=0x%04x\n",
	      pgi2c.handle,
	      pgi2c.i2cReg,
	      pgi2c.wVal);
}

#endif /* HAVE_PIGPIO_H */

/* Change Log */
/*
 * $Log: i2c_wraps.c,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.3  2017/07/17 13:03:18  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/16 17:32:58  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.11  2015/04/25 11:53:40  pi
 * Moving code to pigpio_wraps.c
 *
 * Revision 1.10  2015/04/24 14:07:52  pi
 * Interim commit
 *
 * Revision 1.9  2015/04/23 22:07:36  pi
 * Beginning to use pigpio methods
 *
 * Revision 1.8  2015/04/19 18:58:51  pi
 * Interim commit
 *
 * Revision 1.7  2014/12/22 17:00:42  pi
 * Interim commit
 *
 * Revision 1.6  2014/12/21 20:05:18  pi
 * Beginning to add bit-banged spi support for nokia glcd
 *
 * Revision 1.5  2014/12/05 19:14:13  pi
 * Interim commit
 *
 * Revision 1.4  2014/12/04 23:00:48  pi
 * C2S_gpio_wb function added
 *
 * Revision 1.3  2014/10/29 18:49:32  pi
 * Minor bug-fix
 *
 * Revision 1.2  2014/07/28 22:13:48  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.7  2014/03/21 21:48:18  pi
 * More gpio system call wrappers added etc.
 *
 * Revision 1.6  2014/03/02 22:47:02  pi
 * Interim commit
 *
 * Revision 1.5  2014/03/02 15:12:56  pi
 * Interim commit
 *
 * Revision 1.4  2014/03/01 18:21:13  pi
 * New routines etc. for LCD backpack device added
 *
 * Revision 1.3  2013/06/11 20:50:52  pi
 * Interim commit
 *
 * Revision 1.2  2013/06/07 21:28:53  pi
 * Interim commit
 *
 * Revision 1.1  2013/06/03 20:58:09  pi
 * Initial revision
 *
 * Revision 1.6  2013/05/26 23:32:06  pi
 * Added WP_i2cget()
 *
 * Revision 1.5  2013/05/26 21:51:20  pi
 * Added C2S_i2cout()
 *
 * Revision 1.4  2013/05/26 21:20:41  pi
 * WP_i2cout() added
 *
 * Revision 1.3  2013/05/15 23:07:35  pi
 * Interim commit
 *
 * Revision 1.2  2013/05/15 22:30:47  pi
 * Interim commit
 *
 * Revision 1.1  2013/05/15 22:10:05  pi
 * Initial revision
 *
 *
 *
 */
