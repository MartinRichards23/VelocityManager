
/* 	$Id: ioEVGrid.c,v 1.2 2005/07/28 15:00:19 carl Exp $	 */

/* Shareware Disclaimer:
   This software is SHAREWARE and is provided as a benefit
   for our clients by Dynamic Graphics, Inc.; however, it is
   unsupported.  Dynamic Graphics accepts no responsibility for
   the content, maintenance, or support of SHAREWARE.
*/


/* Binary read and write routines.  On Linux, _WIN32, swap bytes.
   EarthVision grids are always written in "Big Endian" format.
 */

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(linux)
#define	SWAP_BYTES 1
#endif

/* --------------------------------------------------------------------*/

static void swapBytes (void *data,
		       unsigned int length,
		       unsigned int size)
{
  unsigned int *word, swap, i;

  switch (size) {
  case 1 : return;
  case 2 :			/* short */
    for(i=0; i<length; i+=2) {
      word = (unsigned int *)((unsigned char *) data + i*2);
      word[0] =
	(0xff000000 & (word[0] << 8)) |
	(0x00ff0000 & (word[0] >> 8)) |
	(0x0000ff00 & (word[0] << 8)) |
	(0x000000ff & (word[0] >> 8));
    }
    break;
    
  case 4 :			/* int, float */
    for(i=0; i<length; i++) {
      word = (unsigned int *)((unsigned char *) data + i*4);
      word[0] =
	(0xff000000 & (word[0] << 24)) |
	(0x00ff0000 & (word[0] << 8)) |
	(0x0000ff00 & (word[0] >> 8)) |
	(0x000000ff & (word[0] >> 24));
    }
    break;
    
  case 8 :			/* long (64), double */
    for(i=0; i<length; i++) {
      word = (unsigned int *)((unsigned char *) data + i*8);
      swap =
	(0xff000000 & (word[0] << 24)) |
	(0x00ff0000 & (word[0] << 8)) |
	(0x0000ff00 & (word[0] >> 8)) |
	(0x000000ff & (word[0] >> 24));
      word[0] =
	(0xff000000 & (word[1] << 24)) |
	(0x00ff0000 & (word[1] << 8)) |
	(0x0000ff00 & (word[1] >> 8)) |
	(0x000000ff & (word[1] >> 24));
      word[1] = swap;
    }
    break;
  }
}
/* --------------------------------------------------------------------*/
/* Read n integers from a binary file. */

int binary_read_int (FILE *fp, int *data, unsigned int n)
{
  size_t nread = 0, new;

  /* Read until we've got everything. */
  do {
    new = fread ((void *) (data+nread), sizeof (int), n-nread, fp);
    nread += new;
  } while (nread < n && !ferror (fp) && !feof (fp));
    
#if SWAP_BYTES
  if (n == nread)
    swapBytes (data, n, sizeof(int));
#endif

  return (n == nread);
}
/* --------------------------------------------------------------------*/
/* Read n floats from a binary file. */

int binary_read_float (FILE *fp, float *data, unsigned int n)
{
  size_t nread = 0, new;

  /* Read until we've got everything. */
  do {
    new = fread ((void *) (data+nread), sizeof (float), n-nread, fp);
    nread += new;
  } while (nread < n && !ferror (fp) && !feof (fp));

#if SWAP_BYTES
  if (n == nread)
    swapBytes (data, n, sizeof(float));
#endif

  return (n == nread);
}
/* --------------------------------------------------------------------*/
/* Read n doubles from a binary file. */

int binary_read_double (FILE *fp, double *data, unsigned int n)
{
  size_t nread = 0, new;

  /* Read until we've got everything. */
  do {
    new = fread ((void *) (data+nread), sizeof (double), n-nread, fp);
    nread += new;
  } while (nread < n && !ferror (fp) && !feof (fp));

#if SWAP_BYTES
  if (n == nread)
    swapBytes (data, n, sizeof(double));
#endif

  return (n == nread);
}
/* --------------------------------------------------------------------*/
/* Write n integers to a binary file. */

int binary_write_int (FILE *fp, int *data, unsigned int n)
{
  int status;

#if SWAP_BYTES
  swapBytes (data, n, sizeof(int));
#endif

  status = fwrite ((char *) data, sizeof (int), n, fp) == n;

#if SWAP_BYTES
  /* Swap back when done so original data is unchanged. */
  swapBytes (data, n, sizeof(int));
#endif

  return (status);
}

/* --------------------------------------------------------------------*/
/* Write n floats to a binary file. */

int binary_write_float (FILE *fp, float *data, unsigned int n)
{
  int status;

#if SWAP_BYTES
  swapBytes (data, n, sizeof(float));
#endif

  status = fwrite ((char *) data, sizeof (float), n, fp) == n;

#if SWAP_BYTES
  /* Swap back when done so original data is unchanged. */
  swapBytes (data, n, sizeof(float));
#endif

  return (status);
}
/* --------------------------------------------------------------------*/
/* Write n doubles to a binary file. */

int binary_write_double (FILE *fp, double *data, unsigned int n)
{
  int status;

#if SWAP_BYTES
  swapBytes (data, n, sizeof(double));
#endif

  status = fwrite ((char *) data, sizeof (double), n, fp) == n;

#if SWAP_BYTES
  /* Swap back when done so original data is unchanged. */
  swapBytes (data, n, sizeof(double));
#endif

  return (status);
}
