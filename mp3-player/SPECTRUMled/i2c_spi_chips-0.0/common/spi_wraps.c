/* C source file associated with SPI WRAPperS etc.
 * $Id: spi_wraps.c,v 1.5 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "spi_wraps.h"

void reset_cascade(cascade thiscascade[])
{
  int l;
  for (l=0; l<MAX_CASCADE_LEN; l++)
    {
      thiscascade[l].next = EMPTY_CASCADE;
      thiscascade[l].regr = 0x00;
      thiscascade[l].data = 0x00;
    }
}

void dump_cascade(cascade thiscascade[],
		  int caslen)
{
  int l,n;
  if (caslen < MAX_CASCADE_LEN) n = caslen;
  else n=MAX_CASCADE_LEN;
  for (l=0; l<n; l++)
    {
      printf("cascade[%03d] %03d:0x%02x:0x%02x\n",
	     l,
	     thiscascade[l].next,
	     thiscascade[l].regr,
	     thiscascade[l].data);
      if (thiscascade[l].next == EMPTY_CASCADE)break;
    }
}

int add_to_cascade(cascade thiscascade[],
		   unsigned int regr,
		   unsigned int data,
		   int caslen)	
{
  int l,n,next;
  next = EMPTY_CASCADE;
  if (caslen < MAX_CASCADE_LEN) n = caslen;
  else n=MAX_CASCADE_LEN;
  for (l=0; l<n; l++)
    {
      if (thiscascade[l].next == next)
	{
	  thiscascade[l].next = l + 1;
	  thiscascade[l].regr = regr;
	  thiscascade[l].data = data;
	  next = thiscascade[l].next;
	  break;
	}
    }
  return(next);
}

int fill_cascade(cascade thiscascade[],
		 unsigned int regr,
		 unsigned int data,
		 int caslen)
{
  int l,n,next;
  if (caslen < MAX_CASCADE_LEN) n = caslen;
  else n=MAX_CASCADE_LEN;
  for (l=0; l<n; l++)
    {
      /* Only fill empty slots! */
      if (thiscascade[l].next == EMPTY_CASCADE)
	{
	  thiscascade[l].next = l + 1;
	  thiscascade[l].regr = regr;
	  thiscascade[l].data = data;
	  next = thiscascade[l].next;
	}
      else break; /* Early return at 1st. non-empty slot */
    }
  return(next);
}

int insert_into_cascade(cascade thiscascade[],
			unsigned int regr,
			unsigned int data,
			int casposn,
			int caslen)
{
  int l,n,next;
  next = EMPTY_CASCADE;
  if (caslen < MAX_CASCADE_LEN) n = caslen;
  else n=MAX_CASCADE_LEN;
  if (casposn >= n) return(next);
  if (casposn < 0) return(next);
  next = casposn + 1;
  thiscascade[casposn].next = next;
  thiscascade[casposn].regr = regr;
  thiscascade[casposn].data = data;  
  return(next);
}

int load_cascade_buffer(cascade thiscascade[])
{
  int k,l,n;
  /* Early (error) exit for empty cascade ... */
  if (thiscascade[0].next == EMPTY_CASCADE) return(-1);
  k = l = 0;
  while(thiscascade[l].next != EMPTY_CASCADE)
    {
      cascade_buffer[k] = (char)(thiscascade[l].regr);
      k++;
      cascade_buffer[k] = (char)(thiscascade[l].data);
      k++;
      l++;
      if (l == MAX_CASCADE_LEN) break;
      if (k >= CASCADE_BUF_LEN) break;
    }
  return(k);
}   
	     
void dump_cascade_buffer(int nbytes)
{
  int i;
  for (i=0; i<nbytes; i++)
    printf("%02x.", cascade_buffer[i]);
  printf("\n");
}   

/* NB: 2018.03.08 cascade mode with "printf" appears not to work! */
/* Forcing wiringPi method in main code for now!                  */

void C2S_cascade2spidev(char *spidev,
			cascade thiscascade[],
			char qx)
{
  static char spi_command[CASCADE_CMD_LEN];
  static char hexstr[6];
  int l;
  unsigned int regr,data;
#ifndef NO_SUDO_ECHO_TO_SPI
  strcpy(spi_command, "sudo echo -ne \"");
#else /* NO_SUDO_ECHO_TO_SPI */
  /* strcpy(spi_command, "printf \"\\"); */
  strcpy(spi_command, "printf \"");
#endif /* NO_SUDO_ECHO_TO_SPI */
  l = 0;
  while (thiscascade[l].next != EMPTY_CASCADE)
    {
      regr = thiscascade[l].regr;
      data = thiscascade[l].data;
      strcat(spi_command, "\\");
#ifndef NO_SUDO_ECHO_TO_SPI
      sprintf(hexstr, "x%02x\0", regr);
      strncat(spi_command, hexstr, 5);
      strcat(spi_command, "\\");
      sprintf(hexstr, "x%02x\0", data);
      strncat(spi_command, hexstr, 5);
#else /* NO_SUDO_ECHO_TO_SPI */
      sprintf(hexstr, "%03o\0", regr);
      strncat(spi_command, hexstr, 6);
      strcat(spi_command, "\\");
      sprintf(hexstr, "%03o\0", data);
      strncat(spi_command, hexstr, 6);
#endif /* NO_SUDO_ECHO_TO_SPI */
      l++;
    }
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}
		      
#ifdef HAVE_WIRINGPISPI_H
void WP_write_cascade(int spifd,
		      cascade thiscascade[],
		      char qx)
{
  int nbytes;
  ssize_t k1,k2;
  nbytes = load_cascade_buffer(thiscascade);
  if (nbytes > 0)
    {
      if (!qx)
	{
	  printf("SPIfd[%1d] >> ", spifd);
	  dump_cascade_buffer(nbytes);
	}
      else
	{
	  k1 = nbytes;
	  k2 = write(spifd, cascade_buffer, k1);
	  if (k2 != k1) printf("SPI write error: k2=%d cf. k1=%d\n", k2, k1);
	}
    }
}		      
#endif /* HAVE_WIRINGPISPI_H */

void C2S_sechob2spidev(char *spidev,
		       unsigned int sbyte,
		       char qx)
{
  static char spi_command[CMD_LEN];
  static char hexstr[6];
#ifndef NO_SUDO_ECHO_TO_SPI
  strcpy(spi_command, "sudo echo -ne \"\\");
  sprintf(hexstr, "x%02x\0", sbyte);
#else /* NO_SUDO_ECHO_TO_SPI */
  strcpy(spi_command, "printf \"\\");
  sprintf(hexstr, "%03o\0", sbyte);
#endif /* NO_SUDO_ECHO_TO_SPI */
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}
	
void C2S_secho2spidev(char *spidev,
		      unsigned int regr,
		      unsigned int data,
		      char qx)
{
  static char spi_command[CMD_LEN];
  static char hexstr[6];
#ifndef NO_SUDO_ECHO_TO_SPI
  strcpy(spi_command, "sudo echo -ne \"\\");
  sprintf(hexstr, "x%02x\0", regr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "x%02x\0", data);
#else /* NO_SUDO_ECHO_TO_SPI */
  strcpy(spi_command, "printf \"\\");
  sprintf(hexstr, "%03o\0", regr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "%03o\0", data);
#endif /* NO_SUDO_ECHO_TO_SPI */
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}
	      
void C2S_secho3b2spidev(char *spidev,
			unsigned int waddr,
			unsigned int regr,
			unsigned int data,
			char qx)
{
  static char spi_command[CMD_LEN];
  static char hexstr[6];
#ifndef NO_SUDO_ECHO_TO_SPI
  strcpy(spi_command, "sudo echo -ne \"\\");
  sprintf(hexstr, "x%02x\0", waddr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "x%02x\0", regr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "x%02x\0", data);
#else /* NO_SUDO_ECHO_TO_SPI */
  strcpy(spi_command, "printf \"\\");
  sprintf(hexstr, "%03o\0", waddr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "%03o\0", regr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "%03o\0", data);
#endif /* NO_SUDO_ECHO_TO_SPI */
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}
		      
#ifdef HAVE_WIRINGPI_H
void C2S_echob2spidev(char *spidev,
		      unsigned int sbyte,
		      char qx)
{
  static char spi_command[CMD_LEN];
  static char hexstr[6];
  strcpy(spi_command, "echo -ne \"\\");
  sprintf(hexstr, "x%02x\0", sbyte);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}	
	      
void C2S_echo2spidev(char *spidev,
		     unsigned int regr,
		     unsigned int data,
		     char qx)
{
  static char spi_command[CMD_LEN];
  static char hexstr[6];
  strcpy(spi_command, "echo -ne \"\\");
  sprintf(hexstr, "x%02x\0", regr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "x%02x\0", data);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}

void C2S_echo3b2spidev(char *spidev,
		       unsigned int waddr,
		       unsigned int regr,
		       unsigned int data,
		       char qx)
{
  static char spi_command[CMD_LEN];
  static char hexstr[6];
  strcpy(spi_command, "echo -ne \"\\");
  sprintf(hexstr, "x%02x\0", waddr);
  strncat(spi_command, hexstr, 5);		      
  sprintf(hexstr, "x%02x\0", regr);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\\");
  sprintf(hexstr, "x%02x\0", data);
  strncat(spi_command, hexstr, 5);
  strcat(spi_command, "\" > ");
  strcat(spi_command, spidev);
  if (!qx)   printf("%s\n", spi_command);
  else system(spi_command);
}
#endif /* HAVE_WIRINGPI_H */
	      
#ifdef HAVE_WIRINGPISPI_H
void WP_write_byte(int spifd,
		   unsigned int sbyte,
		   char qx)
{
  ssize_t k1,k2;
  char lbuf[1];
  k1 = 1;
  lbuf[0] = sbyte;
  if (!qx) printf("SPIfd[%1d] >> 0x%02x\n", spifd, lbuf[0]);
  else 
    {
      k2 = write(spifd, lbuf, k1);
      if (k2 != k1) printf("SPI write error: k2=%d cf. k1=%d\n", k2, k1);
    }
}

void WP_write2bytes(int spifd,
		    unsigned int regr,
		    unsigned int data,
		    char qx)
{
  ssize_t k1,k2;
  char lbuf[2];
  k1 = 2;
  lbuf[0] = regr;
  lbuf[1] = data;
  if (!qx) printf("SPIfd[%1d] >> 0x%02x 0x%02x\n", spifd, lbuf[0], lbuf[1]);
  else 
    {
      k2 = write(spifd, lbuf, k1);
      if (k2 != k1) printf("SPI write error: k2=%d cf. k1=%d\n", k2, k1);
    }
}		      

void WP_write3bytes(int spifd,
		    unsigned int waddr,
		    unsigned int regr,
		    unsigned int data,
		    char qx)
{
  ssize_t k1,k2;
  char lbuf[3];
  k1 = 3;
  lbuf[0] = waddr;
  lbuf[1] = regr;
  lbuf[2] = data;
  if (!qx) printf("SPIfd[%1d] >> 0x%02x 0x%02x 0x%02x\n",
		  spifd, 
		  lbuf[0], 
		  lbuf[1], 
		  lbuf[2]);
  else 
    {
      k2 = write(spifd, lbuf, k1);
      if (k2 != k1) printf("SPI write error: k2=%d cf. k1=%d\n", k2, k1);
    }
}		      
#endif /* HAVE_WIRINGPISPI_H */

/* Change Log */
/*
 * $Log: spi_wraps.c,v $
 * Revision 1.5  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.2  2018/03/08 22:01:00  pi
 * Interim commit
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.4  2017/08/31 15:50:59  pi
 * Minor bug-fix to cascade mode (system calls)
 *
 * Revision 1.3  2017/08/11 16:40:25  pi
 * Interim commit
 *
 * Revision 1.2  2017/08/11 13:32:39  pi
 * Interim Commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.3  2014/11/08 19:45:03  pi
 * Support for 3-byte writes added
 *
 * Revision 1.2  2014/07/23 21:53:01  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.7  2014/06/14 21:55:03  pi
 * Interim commit
 *
 * Revision 1.6  2014/06/13 20:50:33  pi
 * Interim commit
 *
 * Revision 1.5  2014/06/12 23:28:30  pi
 * Interim commit
 *
 * Revision 1.4  2014/06/11 20:29:54  pi
 * Beginning to cascade
 *
 * Revision 1.3  2014/06/04 20:17:58  pi
 * Interim commit
 *
 * Revision 1.2  2014/06/03 21:02:37  pi
 * Interim commit
 *
 * Revision 1.1  2014/06/03 14:54:32  pi
 * Initial revision
 *
 *
 *
 *
 */
