
/* 	$Id: testEVGridIO.c,v 1.2 2005/07/28 15:01:37 carl Exp $	 */

/* Shareware Disclaimer:
   This software is SHAREWARE and is provided as a benefit
   for our clients by Dynamic Graphics, Inc.; however, it is
   unsupported.  Dynamic Graphics accepts no responsibility for
   the content, maintenance, or support of SHAREWARE.
*/

/* Test program for EVGridIO functions. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EVGridIO.h"

/* --------------------------------------------------------------------*/

int main (int argc, char **argv)
{
  char *desc;
  float *grid;
  int isBordered;
  int xcol, yrow, zlev;
  double xpivot, xrange, ypivot, yrange, zpivot, zrange, angle;
  char *vfltFile, *nvfltFile;
  int status;
  int projection, ellipsoid, zone, units, zUnits, pUnits;
  double projParms[15];
  char *trouble;

  if (argc < 3) {
    fprintf (stderr, "Usage: %s <input_grid> <output_grid>\n", argv[0]);
    exit (0);
  }
  
  if (strstr (argv[1], ".2grd")) {
    if (!strstr (argv[2], ".2grd")) {
      fprintf (stderr, "%s: Wrong suffix on 2nd grid name\n", argv[0]);
      exit (1);
    }

    status = read2DEVGrid (argv[1], &desc,
			   &grid, &isBordered,
			   &xcol, &yrow,
			   &xpivot, &xrange,
			   &ypivot, &yrange,
			   &angle,
			   &projection, &ellipsoid, &zone, &units,
			   projParms, &zUnits,
			   &vfltFile, &nvfltFile,
			   &trouble);

    if (status == 0) {
      if (isBordered == 0) {
	status = write2DEVGrid (argv[2], desc,
				grid,
				xcol, yrow,
				xpivot, xrange,
				ypivot, yrange,
				angle,
				projection, ellipsoid, zone, units,
				projParms, zUnits,
				vfltFile, nvfltFile,
				&trouble);

	if (status != 0)
	  fprintf (stderr, "Error %d writing %s: %s\n",
		   status, argv[2], trouble);
      }
      else 
	fprintf (stderr, "Not rewriting bordered 2D grid\n");
    }
    else
      fprintf (stderr, "Error %d reading %s: %s\n",
	       status, argv[1], trouble);
  }
  else if (strstr (argv[1], ".3grd")) {
    if (!strstr (argv[2], ".3grd")) {
      fprintf (stderr, "%s: Wrong suffix on 2nd grid name\n", argv[0]);
      exit (1);
    }

    status = read3DEVGrid (argv[1], &desc,
			   &grid, &isBordered,
			   &xcol, &yrow, &zlev,
			   &xpivot, &xrange,
			   &ypivot, &yrange,
			   &zpivot, &zrange,
			   &angle,
			   &projection, &ellipsoid,
			   &zone, &units,
			   projParms, &zUnits, &pUnits,
			   &trouble);

    if (status == 0) {
      if (isBordered == 0) {
	status = write3DEVGrid (argv[2], desc,
				grid,
				xcol, yrow, zlev,
				xpivot, xrange,
				ypivot, yrange,
				zpivot, zrange,
				angle,
				projection, ellipsoid, zone, units,
				projParms, zUnits, pUnits,
				&trouble);

	if (status != 0)
	  fprintf (stderr, "Error %d writing %s: %s\n",
		   status, argv[2], trouble);
      }
      else 
	fprintf (stderr, "Not rewriting bordered 3D grid\n");
    }
    else
      fprintf (stderr, "Error %d reading %s: %s\n",
	       status, argv[1], trouble);
  }
  else {
    fprintf (stderr, "%s: Unrecognized grid suffix on file %s\n",
	     argv[0], argv[1]);
    exit (2);
  }

  if (!status)
    fprintf (stderr, "Rewrote %s as %s\n", argv[1], argv[2]);

  return status;
}
