
/* 	$Id: writeEVGrid.c,v 1.2 2005/07/28 14:59:12 carl Exp $	 */

/* Shareware Disclaimer:
   This software is SHAREWARE and is provided as a benefit
   for our clients by Dynamic Graphics, Inc.; however, it is
   unsupported.  Dynamic Graphics accepts no responsibility for
   the content, maintenance, or support of SHAREWARE.
*/

#include <stdio.h>
#include <string.h>

#include "EVGridIO.h"


/* Grid header tokens. */
#define Evg_version  2
#define Evg_xcol     9
#define Evg_yrow    10
#define Evg_zlev    11
#define Evg_vflt    14
#define Evg_nvflt   15
#define Evg_proj    17
#define Evg_zone    18
#define Evg_units   19
#define Evg_projParms 20
#define Evg_zUnits  21
#define Evg_pUnits  22
#define Evg_desc    23
#define Evg_geometry 25
#define Evg_values  30
#define Evg_space   73

extern int binary_write_int (FILE *fp, int *data, unsigned int n);

extern int binary_write_float (FILE *fp, float *data, unsigned int n);
                
extern int binary_write_double (FILE *fp, double *data, unsigned int n);

/* --------------------------------------------------------------------*/

int writeEVGrid (const char *gridName, int is2D, char *desc,
		 float *grid,
		 int xcol, int yrow, int zlev,
		 double xpivot, double xrange,
		 double ypivot, double yrange,
		 double zpivot, double zrange,
		 double angle,
		 int projection, int ellipsoid, int zone, int units,
		 double *projParms, int zUnits, int pUnits,
		 char *vfltFile, char *nvfltFile,
		 char **trouble)
{
  int token, length, ival, spaceFlag;
  unsigned int size;
  unsigned char geometry = 0;
  double xmin, xmax, ymin, ymax, zmin, zmax;
  
  FILE *fp = fopen (gridName, "wb");
  if (!fp) {
    if (trouble)
      *trouble = strdup ("Error opening grid file");
    return -1;
  }

  /* First word is magic number.  It depends upon type. */
  ival = is2D ? 0x2d2d2d2d : 0x3d3d3d3d;
  binary_write_int (fp, &ival, 1);
  if (ferror (fp)) {
    if (trouble)
      *trouble = strdup ("Error writing to grid file");
    return -2;
  }
  
  /* Write grid version */
  token = Evg_version;
  length = sizeof (int);
  ival = Ev_CURRENT_VERSION;
  binary_write_int (fp, &token, 1);
  binary_write_int (fp, &length, 1);
  binary_write_int (fp, &ival, 1);

  /* Grid geometry is always uniform. */
  token = Evg_geometry;
  length = sizeof (geometry);
  binary_write_int (fp, &token, 1);
  binary_write_int (fp, &length, 1);
  fwrite (&geometry, sizeof (geometry), 1, fp);

  /* Space */
  token	= Evg_space;
  /* Length depends on space format and how much projection info there is. */
  length = 5 * sizeof (int);
  if (projection != Evp_LocalRect)
    length += 2 * sizeof (int) + 15 * sizeof (double);
  
  if (is2D) {
    if (angle == 0.0) {
      /* 2D unrotated */
      spaceFlag = 1;
      length += 4 * sizeof (double);
    }
    else {
      /* 2D rotated */
      spaceFlag = 4;
      length += 5 * sizeof (double);
    }
  }
  else {
    if (angle == 0.0) {
      /* 3D unrotated */
      spaceFlag = 2;
      length += 6 * sizeof (double);
    }
    else {
      /* 3D rotated */
      spaceFlag = 5;
      length += 7 * sizeof (double);
    }
  }

  binary_write_int (fp, &token, 1);
  binary_write_int (fp, &length, 1);
     
  ival = 1;			
  binary_write_int (fp, &ival, 1); /* Space version */
  binary_write_int (fp, &spaceFlag, 1);

  /* Write grid extents */
  switch (spaceFlag) {
  case 1:			/* Unrotated, 2D range */
    xmin = xpivot;
    xmax = xpivot + xrange;
    ymin = ypivot;
    ymax = ypivot + yrange;
    binary_write_double (fp, &xmin, 1);
    binary_write_double (fp, &xmax, 1);
    binary_write_double (fp, &ymin, 1);
    binary_write_double (fp, &ymax, 1);
    break;   
  case 2:			/* Unrotated, 3D range */
    xmin = xpivot;
    xmax = xpivot + xrange;
    ymin = ypivot;
    ymax = ypivot + yrange;
    zmin = zpivot;
    zmax = zpivot + zrange;
    binary_write_double (fp, &xmin, 1);
    binary_write_double (fp, &xmax, 1);
    binary_write_double (fp, &ymin, 1);
    binary_write_double (fp, &ymax, 1);
    binary_write_double (fp, &zmin, 1);
    binary_write_double (fp, &zmax, 1);
    break;   
  case 4:			/* Rotated in XY plane, 2D range */
    binary_write_double (fp, &xpivot, 1);
    binary_write_double (fp, &ypivot, 1);
    binary_write_double (fp, &xrange, 1);
    binary_write_double (fp, &yrange, 1);
    binary_write_double (fp, &angle, 1);
    break;
  case 5:			/* Rotated in XY plane, 3D range */
    binary_write_double (fp, &xpivot, 1);
    binary_write_double (fp, &ypivot, 1);
    binary_write_double (fp, &zpivot, 1);
    binary_write_double (fp, &xrange, 1);
    binary_write_double (fp, &yrange, 1);
    binary_write_double (fp, &zrange, 1);
    binary_write_double (fp, &angle, 1);
    break;
  }
      
  /* Write projection */
  binary_write_int (fp, &projection, 1);
  if (projection != Evp_LocalRect) {
    binary_write_int (fp, &zone, 1);
    binary_write_int (fp, &ellipsoid, 1);
    if (projParms)
      binary_write_double (fp, projParms, 15);
    else {
      double localProjParms[15] = { 0.0, 0.0, 0.0, 0.0, 0.0,
				    0.0, 0.0, 0.0, 0.0, 0.0,
				    0.0, 0.0, 0.0, 0.0, 0.0, };
      binary_write_double (fp, localProjParms, 15);
    }
  }
      
  /* Write units */
  binary_write_int (fp, &units, 1);
  binary_write_int (fp, &zUnits, 1);


  token = Evg_xcol;
  length = sizeof (int);
  binary_write_int (fp, &token, 1);
  binary_write_int (fp, &length, 1);
  binary_write_int (fp, &xcol, 1);

  token = Evg_yrow;
  binary_write_int (fp, &token, 1);
  binary_write_int (fp, &length, 1);
  binary_write_int (fp, &yrow, 1);

  if (!is2D) {
    token = Evg_zlev;
    binary_write_int (fp, &token, 1);
    binary_write_int (fp, &length, 1);
    binary_write_int (fp, &zlev, 1);
  }

  if (is2D && vfltFile) {
    token = Evg_vflt;
    length = (int) strlen (vfltFile);
    binary_write_int (fp, &token, 1);
    binary_write_int (fp, &length, 1);
    fwrite (vfltFile, sizeof (char), length, fp);
  }

  if (is2D && nvfltFile) {
    token = Evg_nvflt;
    length = (int) strlen (nvfltFile);
    binary_write_int (fp, &token, 1);
    binary_write_int (fp, &length, 1);
    fwrite (nvfltFile, sizeof (char), length, fp);
  }

  if (!is2D) {
    token = Evg_pUnits;
    length = sizeof (int);
    binary_write_int (fp, &token, 1);
    binary_write_int (fp, &length, 1);
    binary_write_int (fp, &pUnits, 1);
  }
 
  token = Evg_values;
  size = xcol * yrow * (is2D ? 1 : zlev);
  length = size * sizeof (float);
  binary_write_int (fp, &token, 1);
  binary_write_int (fp, &length, 1);
  binary_write_float (fp, grid, size);

  /* Check one last time if there is a write error. */
  if (ferror (fp)) {
    if (trouble)
      *trouble = strdup ("Error writing grid to file");
    return -3;
  }

  /* Close grid file. */
  fclose (fp);

  return 0;
}
/* --------------------------------------------------------------------*/
/* Write a (possibly) rotated 2D grid. */

int write2DEVGrid (const char *gridName, char *desc,
		  float *grid,
		  int xcol, int yrow,
		  double xpivot, double xrange,
		  double ypivot, double yrange,
		  double angle,
		  int projection, int ellipsoid, int zone, int units,
		  double *projParms, int zUnits,
		  char *vfltFile, char *nvfltFile,
		  char **trouble)
{
  return writeEVGrid (gridName, 1, desc,
		      grid,
		      xcol, yrow, 0,
		      xpivot, xrange,
		      ypivot, yrange,
		      0.0, 0.0,
		      angle,
		      projection, ellipsoid, zone, units,
		      projParms, zUnits, Evu_unknown,
		      vfltFile, nvfltFile,
		      trouble);
}
/* --------------------------------------------------------------------*/
/* Write a (possibly) rotated 3D grid. */

int write3DEVGrid (const char *gridName, char *desc,
		   float *grid,
		   int xcol, int yrow, int zlev,
		   double xpivot, double xrange,
		   double ypivot, double yrange,
		   double zpivot, double zrange,
		   double angle,
		   int projection, int ellipsoid, int zone, int units,
		   double *projParms, int zUnits, int pUnits,
		   char **trouble)
{
  return writeEVGrid (gridName, 0, desc,
		      grid,
		      xcol, yrow, zlev,
		      xpivot, xrange,
		      ypivot, yrange,
		      zpivot, zrange,
		      angle,
		      projection, ellipsoid, zone, units,
		      projParms, zUnits, pUnits,
		      NULL, NULL,
		      trouble);
}
/* --------------------------------------------------------------------*/
/* Write an unrotated 2D grid */

int writeUnrotated2DEVGrid (const char *gridName, char *desc,
			    float *grid,
			    int xcol, int yrow,
			    double xmin, double xmax,
			    double ymin, double ymax,
			    int projection, int ellipsoid,
			    int zone, int units,
			    double *projParms, int zUnits,
			    char *vfltFile, char *nvfltFile,
			    char **trouble)
{
  return write2DEVGrid (gridName, desc,
			grid,
			xcol, yrow,
			xmin, xmax - xmin,
			ymin, ymax - ymin,
			0.0,
			projection, ellipsoid, zone, units,
			projParms, zUnits,
			vfltFile, nvfltFile,
			trouble);
}
/* --------------------------------------------------------------------*/
/* Write an unrotated 3D grid. */

int writeUnrotated3DEVGrid (const char *gridName, char *desc,
			    float *grid,
			    int xcol, int yrow, int zlev,
			    double xmin, double xmax,
			    double ymin, double ymax,
			    double zmin, double zmax,
			    int projection, int ellipsoid,
			    int zone, int units,
			    double *projParms, int zUnits, int pUnits,
			    char **trouble)
{
  return write3DEVGrid (gridName, desc,
			grid,
			xcol, yrow, zlev,
			xmin, xmax - xmin,
			ymin, ymax - ymin,
			zmin, zmax - zmin,
			0.0,
			projection, ellipsoid, zone, units,
			projParms, zUnits, pUnits,
			trouble);
}
