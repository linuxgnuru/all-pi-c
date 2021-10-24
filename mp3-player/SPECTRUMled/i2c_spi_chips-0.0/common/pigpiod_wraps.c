/* C source file associated with PIGPIOD WRAPperS etc.
 * $Id: pigpiod_wraps.c,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "pigpiod_wraps.h"

#ifdef HAVE_PIGPIOD_IF2_H

void PGD_i2cset(pgdi2cvb pgdi2c,
		char qx)
{
  int lflag;
  if (qx)
    {
      if ((lflag = i2c_write_byte_data(pgdi2c.thispi,
				       pgdi2c.handle,
				       pgdi2c.i2cReg,
				       pgdi2c.bVal)) != 0)
	{
	  fprintf(stderr, "i2c_write_byte_data failed ... ");
	  perror("i2c_write_byte_data");
	}
   }
  else printf("i2c_write_byte_data: pi=%d handle=%d i2cReg=0x%02x bVal=0x%02x\n",
	      pgdi2c.thispi,
	      pgdi2c.handle,
	      pgdi2c.i2cReg,
	      pgdi2c.bVal);
}

void PGD_i2cset2(pgdi2cvb pgdi2c,
		 char qx)
{
  int lflag;
  if (qx)
    {
      if ((lflag = i2c_write_word_data(pgdi2c.thispi,
				       pgdi2c.handle,
				       pgdi2c.i2cReg,
				       pgdi2c.wVal)) != 0)
	{
	  fprintf(stderr, "i2c_write_word_data failed ... ");
	  perror("i2c_write_word_data");
	} 
    }
  else printf("i2cwriteworddata: handle=%d i2cReg=0x%02x wVal=0x%04x\n",
	      pgdi2c.handle,
	      pgdi2c.i2cReg,
	      pgdi2c.wVal);
}

/*
  void PGD_i2cseti(pgdi2cvb pgdi2c,
  unsigned int count,
  char qx)
  {
  int lflag;
  if (qx)
  {
  }
  else
  {
  }
  }
*/

void PGD_i2cout(pgdi2cvb pgdi2c,
		char qx)
{
  int lflag;
  /* NB: Does not use pgdi2c.i2cReg! */
  if (qx)
    {
      if ((lflag = i2c_write_byte(pgdi2c.thispi, pgdi2c.handle, pgdi2c.bVal)) != 0)
	{
	  fprintf(stderr, "i2c_write_byte failed ... ");
	  perror("i2c_write_byte");
	}
    }
  else printf("i2c_write_byte: pi=%d handle=%d bVal=0x%02x\n",
	      pgdi2c.thispi,
	      pgdi2c.handle,
	      pgdi2c.bVal);
}

#endif /* HAVE_PIGPIOD_IF2_H */

/* Change Log */
/*
 * $Log: pigpiod_wraps.c,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.3  2017/12/22 23:32:51  pi
 * Interim Commit
 *
 * Revision 1.2  2017/12/21 20:02:15  pi
 * Interim commit
 *
 * Revision 1.1  2017/12/21 19:07:13  pi
 * Initial revision
 *
 *
 *
 *
 */
