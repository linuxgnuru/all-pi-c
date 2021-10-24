/* C source file associated with PIGPIO WRAPperS etc.
 * $Id: pigpio_wraps.c,v 1.2 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "pigpio_wraps.h"

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

void PG_i2cseti(pgi2cvb pgi2c,
		unsigned int count,
		char qx)
{
  int lflag;
  unsigned int i,lcount;
  if (count > PG_BLOCK_SIZE) lcount = PG_BLOCK_SIZE;
  else lcount = count;
  if (qx)
    {
      if ((lflag = i2cWriteI2CBlockData(pgi2c.handle,
					pgi2c.i2cReg,
					pgi2c.block_buf, 
					lcount)) != 0)
	{
	  fprintf(stderr, "i2cWriteI2CBlockData failed ... ");
	  perror("i2cWriteI2CBlockData");
	} 
    }
  else 
    {
      printf("i2cWriteI2CBlockData: handle=%d i2cReg=0x%02x count=%u...\n... buf=",
	     pgi2c.handle,
	     pgi2c.i2cReg,
	     lcount);
      for (i=0; i<lcount; i++) printf("0x%02x ", pgi2c.block_buf[i]);
      printf("\n");
    }
}

void PG_i2cout(pgi2cvb pgi2c,
	       char qx)
{
  int lflag;
  /* NB: Does not use pgi2c.i2cReg! */
  if (qx)
    {
      if ((lflag = i2cWriteByte(pgi2c.handle, pgi2c.bVal)) != 0)
	{
	  fprintf(stderr, "i2cWriteByte failed ... ");
	  perror("i2cWriteByte");
	}
    }
  else printf("i2cWriteByte: handle=%d bVal=0x%02x\n",
	      pgi2c.handle,
	      pgi2c.bVal);
}
#endif /* HAVE_PIGPIO_H */

/* Change Log */
/*
 * $Log: pigpio_wraps.c,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.5  2015/04/27 12:26:49  pi
 * Interim commit
 *
 * Revision 1.4  2015/04/26 19:36:52  pi
 * Interim commit
 *
 * Revision 1.3  2015/04/26 19:10:56  pi
 * Interim commit
 *
 * Revision 1.2  2015/04/25 16:47:30  pi
 * Interim commit
 *
 * Revision 1.1  2015/04/25 11:53:40  pi
 * Initial revision
 *
 *
 *
 */
