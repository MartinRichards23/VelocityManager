
/* 	$Id: readEVGrid.c,v 1.2 2005/07/28 14:57:40 carl Exp $	 */

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
#define Evg_xmin     3
#define Evg_xmax     4
#define Evg_ymin     5
#define Evg_ymax     6
#define Evg_zmin     7
#define Evg_zmax     8
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
#define Evg_xformType 52
#define Evg_space   73
#define Evg_8bitValues 83

extern int binary_read_int (FILE *fp, int *data, unsigned int n);

extern int binary_read_float (FILE *fp, float *data, unsigned int n);
                
extern int binary_read_double (FILE *fp, double *data, unsigned int n);
                
/* --------------------------------------------------------------------*/

int readEVGrid (const char *gridName, int *is2D, char **descPtr,
		float **gridPtr, int *isBorderedPtr,
		int *xcolPtr, int *yrowPtr, int *zlevPtr,
		double *xpivotPtr, double *xrangePtr,
		double *ypivotPtr, double *yrangePtr,
		double *zpivotPtr, double *zrangePtr,
		double *anglePtr,
		int *projectionPtr, int *ellipsoidPtr,
		int *zonePtr, int *unitsPtr,
		double *projectionParms, int *zUnitsPtr, int *pUnitsPtr,
		char **vfltFilePtr, char **nvfltFilePtr,
		char **trouble)
{
  char *desc = 0;
  char *vflt = 0;
  char *nvflt = 0;
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double xpivot, ypivot, zpivot, xrange, yrange, zrange;
  double angle;
  double projParms[15] = { 0.0, 0.0, 0.0, 0.0, 0.0,
			   0.0, 0.0, 0.0, 0.0, 0.0,
			   0.0, 0.0, 0.0, 0.0, 0.0, };
  int magic;
  int xcol = 0, yrow = 0, zlev = 0;
  int isBordered = 0;
  int projection = Evp_LocalRect;
  int ellipsoid = Eve_unknown;
  int zone = 0;
  int units = Evu_unknown, zUnits = Evu_unknown, pUnits = Evu_unknown;
  int spaceVersion = 0, spaceFlag = -1;
  int transformType;
  int token, length, status;
  unsigned char geometry;
  
  FILE *fp = fopen (gridName, "rb");
  if (!fp) {
    if (trouble)
      *trouble = strdup ("Error opening grid file");
    return -4;
  }

  if (!binary_read_int (fp, &magic, 1)) {
    if (trouble)
      *trouble = strdup ("File is apparently empty");
    fclose (fp);
    return -5;
  }

  if (magic == 0x2d2d2d2d)
    *is2D = 1;
  else if (magic == 0x3d3d3d3d)
    *is2D = 0;
  else {
    if (trouble)
      *trouble = strdup ("File is not an EarthVision grid");
    fclose (fp);
    return -6;
  }

  status = 0;
  
  while (binary_read_int (fp, &token, 1) &&
	 binary_read_int (fp, &length, 1)) {
    
    switch (token) {
    case Evg_desc:
      if (length > 0) {
	desc = (char *) (int *) malloc (length + 1);
	fread (desc, sizeof (char), length, fp);
	desc[length] = '\0';
      }
      break;

    case Evg_space:		/* New style grid header */
      binary_read_int (fp, &spaceVersion, 1);
      if (spaceVersion != 1) {
	if (trouble)
	  *trouble = strdup ("Grid file has space version which is too new");
	fclose (fp);
	return -7;
      }

      /* Read grid extents */
      binary_read_int (fp, &spaceFlag, 1);
      switch (spaceFlag) {
      case 1:			/* Unrotated, 2D range */
	binary_read_double (fp, &xmin, 1);
	binary_read_double (fp, &xmax, 1);
	binary_read_double (fp, &ymin, 1);
	binary_read_double (fp, &ymax, 1);
	break;   
      case 2:			/* Unrotated, 3D range */
	binary_read_double (fp, &xmin, 1);
	binary_read_double (fp, &xmax, 1);
	binary_read_double (fp, &ymin, 1);
	binary_read_double (fp, &ymax, 1);
	binary_read_double (fp, &zmin, 1);
	binary_read_double (fp, &zmax, 1);
	break;   
      case 4:			/* Rotated in XY plane, 2D range */
	binary_read_double (fp, &xpivot, 1);
	binary_read_double (fp, &ypivot, 1);
	binary_read_double (fp, &xrange, 1);
	binary_read_double (fp, &yrange, 1);
	binary_read_double (fp, &angle, 1);
	break;
      case 5:			/* Rotated in XY plane, 3D range */
	binary_read_double (fp, &xpivot, 1);
	binary_read_double (fp, &ypivot, 1);
	binary_read_double (fp, &zpivot, 1);
	binary_read_double (fp, &xrange, 1);
	binary_read_double (fp, &yrange, 1);
	binary_read_double (fp, &zrange, 1);
	binary_read_double (fp, &angle, 1);
	break;
      case 8:			/* Rotated in 3D, 3D range */
	if (trouble)
	  *trouble = strdup ("Grid is rotated in 3D which is not supported");
	fclose (fp);
	return -8;
      default:
	if (trouble)
	  *trouble = strdup ("Grid file has unrecognized space");
	fclose (fp);
	return -9;
      }
      
      /* Read projection */
      binary_read_int (fp, &projection, 1);
      if (projection == -1)
	zone = 0;
      else {
	binary_read_int (fp, &zone, 1);
	binary_read_int (fp, &ellipsoid, 1);
	binary_read_double (fp, projParms, 15);
      }
      
      /* Read units */
      binary_read_int (fp, &units, 1);
      binary_read_int (fp, &zUnits, 1);
      break;

    case Evg_xmin:		/* Old style grid header */
      binary_read_double (fp, &xmin, 1);
      break;
    case Evg_xmax:		/* Old style grid header */
      binary_read_double (fp, &xmax, 1);
      break;
    case Evg_ymin:		/* Old style grid header */
      binary_read_double (fp, &ymin, 1);
      break;
    case Evg_ymax:		/* Old style grid header */
      binary_read_double (fp, &ymax, 1);
      break;
    case Evg_zmin:		/* Old style grid header */
      binary_read_double (fp, &zmin, 1);
      break;
    case Evg_zmax:		/* Old style grid header */
      binary_read_double (fp, &zmax, 1);
      break;

    case Evg_proj:		/* Old style grid header */
      binary_read_int (fp, &projection, 1);
      break;
    case Evg_zone:		/* Old style grid header */
      binary_read_int (fp, &zone, 1);
      break;
    case Evg_units:		/* Old style grid header */
      binary_read_int (fp, &units, 1);
      break;
    case Evg_zUnits:		/* Old style grid header */
      binary_read_int (fp, &zUnits, 1);
      break;
    case Evg_projParms:		/* Old style grid header */
      binary_read_double (fp, projParms, 15);
      break;

    case Evg_xcol:
      binary_read_int (fp, &xcol, 1);
      break;
    case Evg_yrow:
      binary_read_int (fp, &yrow, 1);
      break;
    case Evg_zlev:
      binary_read_int (fp, &zlev, 1);
      break;

    case Evg_vflt:
      if (length > 0) {
	vflt = (char *) malloc (length + 1);
	fread (vflt, sizeof (char), length, fp);
	vflt[length] = '\0';
      }
      break;
    case Evg_nvflt:
      if (length > 0) {
	nvflt = (char *) malloc (length + 1);
	fread (nvflt, sizeof (char), length, fp);
	nvflt[length] = '\0';
      }
      break;

    case Evg_pUnits:
      binary_read_int (fp, &pUnits, 1);
      break;

    case Evg_geometry:
      fread (&geometry, sizeof (unsigned char), 1, fp);
      isBordered = (geometry & 8);
      break;

    case Evg_xformType:
      binary_read_int (fp, &transformType, 1);
      if (transformType != 0) {
	/* This function does not read conformal surfaces, etc. */
	if (trouble)
	  *trouble = strdup ("Grid is conformal and cannot be read correctly");
	fclose (fp);
	status = -11;
      }
      break;

    case Evg_values:
      if (gridPtr) {
	unsigned int size = (*is2D ? xcol * yrow : xcol * yrow * zlev);
	*gridPtr = (float *) malloc (size * sizeof (float));
	binary_read_float (fp, *gridPtr, size);
      }
      else
	/* Don't want the grid nodes.  Skip ahead. */
	fseek (fp, length, SEEK_CUR);
      break;

    case Evg_8bitValues:
      if (gridPtr) {
	/* This function does not read 8-bit values.
	   Only a problem when trying to read grid values. */
	if (trouble)
	  *trouble = strdup ("8-bit grids cannot be read correctly");
	status = -12;
      }
      /* Skip 8-bit values */
      fseek (fp, length, SEEK_CUR);
      break;

    default:
      /* Unknown grid item. Skip. */
      fseek (fp, length, SEEK_CUR);
    }
  }

  /* Close grid file. */
  fclose (fp);

  /* Fill in return values */
  if (descPtr)
    *descPtr = desc;
  else
    free (desc);

  if (isBorderedPtr)
    *isBorderedPtr = isBordered;

  if (xcolPtr)
    *xcolPtr = xcol;
  if (yrowPtr)
    *yrowPtr = yrow;
  if (zlevPtr)
    *zlevPtr = zlev;
  
  switch (spaceFlag) {
  case -1:			/* Old grid format */
  case 1:			/* Unrotated 2D grid */
  case 2:			/* Unrotated 3D grid */
    if (xpivotPtr)
      *xpivotPtr = xmin;
    if (xrangePtr)
      *xrangePtr = xmax - xmin;

    if (ypivotPtr)
      *ypivotPtr = ymin;
    if (yrangePtr)
      *yrangePtr = ymax - ymin;
    
    if (zpivotPtr)
      *zpivotPtr = zmin;
    if (zrangePtr)
      *zrangePtr = zmax - zmin;
    
    if (anglePtr)
      *anglePtr = 0.0;
    break;
    
  case 4:			/* Rotated 2D grid */
  case 5:			/* Rotated 3D grid */
    if (xpivotPtr)
      *xpivotPtr = xpivot;
    if (xrangePtr)
      *xrangePtr = xrange;

    if (ypivotPtr)
      *ypivotPtr = ypivot;
    if (yrangePtr)
      *yrangePtr = yrange;
    
    if (zpivotPtr)
      *zpivotPtr = zpivot;
    if (zrangePtr)
      *zrangePtr = zrange;
    
    if (anglePtr)
      *anglePtr = angle;
    break;

  default:			/* This should never happen */
    if (trouble)
      *trouble = strdup ("readEVGrid bug #1 found.  Please send grid to DGI");
    return -10;
  }
      
  if (projectionPtr)
    *projectionPtr = projection;
  if (ellipsoidPtr)
    *ellipsoidPtr = ellipsoid;
  if (zonePtr)
    *zonePtr = zone;
  if (projectionParms) {
    int i;
    for (i=0; i<15; i++)
      projectionParms[i] = projParms[i];
  }
  if (unitsPtr)
    *unitsPtr = units;
  if (zUnitsPtr)
    *zUnitsPtr = zUnits;
  if (pUnitsPtr)
    *pUnitsPtr = pUnits;

  if (vfltFilePtr)
    *vfltFilePtr = vflt;
  else
    free (vflt);

  if (nvfltFilePtr)
    *nvfltFilePtr = nvflt;
  else
    free (nvflt);
                
  return status;
}
/* --------------------------------------------------------------------*/
/* Read a (possibly) rotated 2D grid. */

int read2DEVGrid (const char *gridName, char **desc,
		  float **grid, int *isBordered,
		  int *xcol, int *yrow,
		  double *xpivot, double *xrange,
		  double *ypivot, double *yrange,
		  double *angle,
		  int *projection, int *ellipsoid,
		  int *zone, int *units,
		  double *projParms, int *zUnits,
		  char **vfltFile, char **nvfltFile,
		  char **trouble)
{
  int is2D;

  int status = readEVGrid (gridName, &is2D, desc,
			   grid, isBordered,
			   xcol, yrow, NULL,
			   xpivot, xrange,
			   ypivot, yrange,
			   NULL, NULL,
			   angle,
			   projection, ellipsoid, zone, units,
			   projParms, zUnits, NULL,
			   vfltFile, nvfltFile,
			   trouble);

  if (status != 0)
    return status;

  if (!is2D) {
    if (trouble)
      *trouble = strdup ("Grid is 3D.  Use read3DEVGrid instead.");

    return -3;
  }

  return 0;
}
/* --------------------------------------------------------------------*/
/* Read a (possibly) rotated 3D grid. */

int read3DEVGrid (const char *gridName, char **desc,
		  float **grid, int *isBordered,
		  int *xcol, int *yrow, int *zlev,
		  double *xpivot, double *xrange,
		  double *ypivot, double *yrange,
		  double *zpivot, double *zrange,
		  double *angle,
		  int *projection, int *ellipsoid,
		  int *zone, int *units,
		  double *projParms, int *zUnits, int *pUnits,
		  char **trouble)
{
  int is2D;

  int status = readEVGrid (gridName, &is2D, desc,
			   grid, isBordered,
			   xcol, yrow, zlev,
			   xpivot, xrange,
			   ypivot, yrange,
			   zpivot, zrange,
			   angle,
			   projection, ellipsoid, zone, units,
			   projParms, zUnits, pUnits,
			   NULL, NULL,
			   trouble);

  if (status != 0)
    return status;

  if (is2D) {
    if (trouble)
      *trouble = strdup ("Grid is 2D.  Use read2DEVGrid instead.");

    return -2;
  }

  return 0;
}
/* --------------------------------------------------------------------*/
/* Read an unrotated 2D grid */

int readUnrotated2DEVGrid (const char *gridName, char **desc,
			   float **grid, int *isBordered,
			   int *xcol, int *yrow,
			   double *xmin, double *xmax,
			   double *ymin, double *ymax,
			   int *projection, int *ellipsoid,
			   int *zone, int *units,
			   double *projParms, int *zUnits,
			   char **vfltFile, char **nvfltFile,
			   char **trouble)
{
  double xpivot, xrange, ypivot, yrange, angle;

  int status = read2DEVGrid (gridName, desc,
			     grid, isBordered,
			     xcol, yrow,
			     &xpivot, &xrange,
			     &ypivot, &yrange,
			     &angle,
			     projection, ellipsoid, zone, units,
			     projParms, zUnits,
			     vfltFile, nvfltFile,
			     trouble);

  if (status != 0)
    return status;

  if (angle != 0.0) {
    if (trouble)
      *trouble = strdup ("Grid is rotated.  Use read2DEVGrid instead.");

    return -1;
  }

  if (xmin)
    *xmin = xpivot;

  if (xmax)
    *xmax = xpivot + xrange;

  if (ymin)
    *ymin = ypivot;

  if (ymax)
    *ymax = ypivot + yrange;

  return 0;
}
/* --------------------------------------------------------------------*/
/* Read an unrotated 3D grid. */

int readUnrotated3DEVGrid (const char *gridName, char **desc,
			   float **grid, int *isBordered,
			   int *xcol, int *yrow, int *zlev,
			   double *xmin, double *xmax,
			   double *ymin, double *ymax,
			   double *zmin, double *zmax,
			   int *projection, int *ellipsoid,
			   int *zone, int *units,
			   double *projParms, int *zUnits, int *pUnits,
			   char **trouble)
{
  double xpivot, xrange, ypivot, yrange, zpivot, zrange, angle;

  int status = read3DEVGrid (gridName, desc,
			     grid, isBordered,
			     xcol, yrow, zlev,
			     &xpivot, &xrange,
			     &ypivot, &yrange,
			     &zpivot, &zrange,
			     &angle,
			     projection, ellipsoid, zone, units,
			     projParms, zUnits, pUnits,
			     trouble);

  if (status != 0)
    return status;

  if (angle != 0.0) {
    if (trouble)
      *trouble = strdup ("Grid is rotated.  Use read3DEVGrid instead.");

    return -1;
  }

  if (xmin)
    *xmin = xpivot;

  if (xmax)
    *xmax = xpivot + xrange;

  if (ymin)
    *ymin = ypivot;

  if (ymax)
    *ymax = ypivot + yrange;

  if (zmin)
    *zmin = zpivot;

  if (zmax)
    *zmax = zpivot + zrange;

  return 0;
}
