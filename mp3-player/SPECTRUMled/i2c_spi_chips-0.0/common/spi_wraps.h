/* Header file associated with SPI WRAPperS etc.
 * $Id: spi_wraps.h,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef SPI_WRAP_H
#define SPI_WRAP_H 1

/* Comment out this #define if using Raspbian Wheezy */
#define NO_SUDO_ECHO_TO_SPI 1
/* Above primarily valid for Raspbian Jessie onwards */
/* But, as of 11.08.2017, sudo is still needed for system() calls using echo ... */
/* or printf but NOT from the command line direct!!! */
/* Suggested workaround is printf with octal data ... */
/* ... see https://www.raspberrypi.org/forums/viewtopic.php?f=44&t=190475 */

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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
#endif /* NO_MAIN */

#ifndef CMD_LEN
#define CMD_LEN         138  /* 2 x FN_LEN + 10   */
#endif /* CMD_LEN */

typedef struct
{
  int next; /* NEXT index */
  unsigned int regr;
  unsigned int data;
} cascade;

#define EMPTY_CASCADE    -1
#define MAX_CASCADE_LEN 128
#define CASCADE_BUF_LEN 256 /* 2 x MAX_CASCADE_LEN */
#define CASCADE_CMD_LEN 512 /* 4 x MAX_CASCADE_LEN */

/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
#ifndef NO_MAIN
cascade led_matrix[MAX_CASCADE_LEN];
cascade led_4digit[MAX_CASCADE_LEN];
char cascade_buffer[CASCADE_BUF_LEN];
#else /* !NO_MAIN */
extern cascade led_matrix[MAX_CASCADE_LEN];
extern cascade led_4digit[MAX_CASCADE_LEN];
extern char cascade_buffer[CASCADE_BUF_LEN];
#endif /* !NO_MAIN */

void reset_cascade(cascade thiscascade[]);

void dump_cascade(cascade thiscascade[],
		  int caslen);

int add_to_cascade(cascade thiscascade[],
		   unsigned int regr,
		   unsigned int data,
		   int caslen);

int fill_cascade(cascade thiscascade[],
		 unsigned int regr,
		 unsigned int data,
		 int caslen);

int insert_into_cascade(cascade thiscascade[],
			unsigned int regr,
			unsigned int data,
			int casposn,
			int caslen);

int load_cascade_buffer(cascade thiscascade[]);

void dump_cascade_buffer(int nbytes);   

void C2S_cascade2spidev(char *spidev,
			cascade thiscascade[],
			char qx);
		      
#ifdef HAVE_WIRINGPISPI_H
void WP_write_cascade(int spifd,
		      cascade thiscascade[],
		      char qx);		      
#endif /* HAVE_WIRINGPISPI_H */

void C2S_sechob2spidev(char *spidev,
		       unsigned int sbyte,
		       char qx);	

void C2S_secho2spidev(char *spidev,
		      unsigned int regr,
		      unsigned int data,
		      char qx);		      

void C2S_secho3b2spidev(char *spidev,
			unsigned int waddr,
			unsigned int regr,
			unsigned int data,
			char qx);		      

#ifdef HAVE_WIRINGPI_H
void C2S_echob2spidev(char *spidev,
		      unsigned int sbyte,
		      char qx);	
	      
void C2S_echo2spidev(char *spidev,
		     unsigned int regr,
		     unsigned int data,
		     char qx);	
	      
void C2S_echo3b2spidev(char *spidev,
		       unsigned int waddr,
		       unsigned int regr,
		       unsigned int data,
		       char qx);		      
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_WIRINGPISPI_H
void WP_write_byte(int spifd,
		   unsigned int sbyte,
		   char qx);
		      
void WP_write2bytes(int spifd,
		    unsigned int regr,
		    unsigned int data,
		    char qx);
		      
void WP_write3bytes(int spifd,
		    unsigned int waddr,
		    unsigned int regr,
		    unsigned int data,
		    char qx);		      
#endif /* HAVE_WIRINGPISPI_H */

#endif /* !SPI_WRAP_H */

/* Change Log */
/*
 * $Log: spi_wraps.h,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
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
 * Revision 1.4  2015/06/26 18:46:12  pi
 * cascade "led4digit" added
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
 * Revision 1.6  2014/06/14 21:55:03  pi
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
