
#ifndef __EVGRIDIO_H__
#define __EVGRIDIO_H__

/* 	$Id: EVGridIO.h,v 1.2 2005/07/28 14:56:23 carl Exp $	 */

/* Shareware Disclaimer:
   This software is SHAREWARE and is provided as a benefit
   for our clients by Dynamic Graphics, Inc.; however, it is
   unsupported.  Dynamic Graphics accepts no responsibility for
   the content, maintenance, or support of SHAREWARE.
*/

#define Ev_CURRENT_VERSION 7	/* Current grid version */

/* Projections */
enum {
  Evp_LocalRect                 = -1,
  Evp_Geographic                = 0,
  Evp_UTM                       = 1,
  Evp_StatePlane                = 2,
  Evp_Albers                    = 3,
  Evp_Lambert                   = 4,
  Evp_Mercator                  = 5,
  Evp_PolarStereo               = 6,
  Evp_Polyconic                 = 7,
  Evp_TransverseMercator        = 9,
  Evp_ObliqueMercator           = 20,
  Evp_Amersfoort                = 21,
  Evp_NewAmersfoort             = 22,
  Evp_Cassini                   = 23,
  Evp_LambertTangential         = 24,
  Evp_RectifiedSkewOrthomorphic = 25,
  Evp_AzimuthalEquidistant      = 26
};

/* Ellipsoids */
enum {
  Eve_unknown                = -1,
  Eve_Clarke_1866            = 0,
  Eve_Bessel_1841            = 1,
  Eve_Airy_1830              = 2,
  Eve_Everest_1830           = 3,
  Eve_Clarke_1880            = 4,
  Eve_HayfordIntl_1924       = 5,
  Eve_Krasovsky_1940         = 6,
  Eve_Australian_1965        = 7,
  Eve_WGS_1972               = 8,
  Eve_GRS_1980               = 9,
  Eve_userDefined            = 10,
  Eve_WGS_1984               = 11,
  Eve_AiryModified_1849      = 12,
  Eve_Indonesian_Natl        = 13,
  Eve_GRS_1967               = 14,
  Eve_Helmert_1906           = 15,
  Eve_Everest_1830_1967      = 16,
  Eve_Clarke_1880_IGN        = 17,
  Eve_Everest_1830_1975      = 18,
  Eve_Everest_1830_Modified  = 19,
  Eve_Everest_1830_1962      = 20,
  Eve_GRS_1967_2dp_Flattening = 21
};

/* Determine if Z units increase upward or downward. */
#define	baseUnit(u)		((u) > 0 ? (u) & ~Evu_downward : (u))
#define	isDownward(u)		((u) > 0 && (u) & Evu_downward)

/* Units */
enum    {
  Evu_downward  = 0x1000,

  Evu_integer         = -3,
  Evu_nonNumeric      = -2,
  Evu_unknown         = -1,
  
  Evu_error           = 0,

  /* Distance */
  Evu_inches          = 1,
  Evu_feet            = 2,    /* International feet 1ft = 0.3048m exactly */
  Evu_usSurveyFeet    = 68,
  Evu_imperialFeet    = 84,
  Evu_indianFeet      = 69,
  Evu_searsFeet       = 70,
  Evu_yards           = 3,
  Evu_indianYards     = 79,
  Evu_miles           = 4,
  
  Evu_mm              = 5,
  Evu_cm              = 6,
  Evu_dm              = 7,
  Evu_meters          = 8,
  Evu_km              = 9,
  Evu_fathoms         = 23,
  
  /* Angle */
  Evu_degreesOfArc    = 10,
  Evu_minutesOfArc    = 55,
  Evu_secondsOfArc    = 11,
  Evu_radians         = 27,
  
  /* Time */
  Evu_ms              = 12,
  Evu_seconds         = 13,
  Evu_minutes         = 62,
  Evu_hours           = 63,
  Evu_days            = 64,
  Evu_weeks           = 65,
  Evu_months          = 66,
  Evu_years           = 67,
  
  /* Ratio */
  Evu_fraction        = 56,
  Evu_percent         = 26,
  Evu_partsPerThousand = 14,
  Evu_partsPerMillion = 15,
  Evu_partsPerBillion = 16,
  Evu_dB              = 24,
  Evu_millidarcies    = 25,
  Evu_specificGravity = 54,
  
  /* Temperature */
  Evu_degreesC        = 17,
  Evu_degreesF        = 18,
  Evu_kelvins         = 19,
  
  /* Velocity */
  Evu_feetPerSec      = 20,
  Evu_metersPerSec    = 21,
  Evu_kmPerSec        = 22,
  Evu_metersPerHour   = 90,
  
  /* Slowness */
  Evu_msPerFoot       = 43,
  
  /* Dogleg severity */
  Evu_degreesPer30ft  = 74,
  Evu_degreesPer100ft = 34,
  Evu_degreesPer10m   = 75,
  Evu_degreesPer30m   = 33,
  Evu_degreesPer100m  = 76,
  Evu_radiansPerMeter = 91,
  
  /* Concentration */
  Evu_mgPerLiter      = 31,
  Evu_ugPerLiter      = 32,
  Evu_poundsPerGallon = 35,
  Evu_gPerCC          = 36,
  
  /* Resistivity */
  Evu_ohmMeters       = 28,
  
  /* Conductivity */
  Evu_millimhosPerMeter = 30,
  Evu_mhosPerMeter    = 42,
  
  /* Pressure */
  Evu_psi             = 37,
  Evu_mPa             = 44,
  Evu_bars            = 87,
  Evu_millibars       = 88,
  Evu_Pa              = 89,
  
  /* Flow */
  Evu_ccPerSec        = 45,
  Evu_km3PerSec       = 46,
  
  /* Rotation velocity */
  Evu_radiansPerSec = 48,
  Evu_degreesPerSec = 49,
  Evu_degreesPerMinute = 50,
  Evu_degreesPerHour = 51,
  
  /* Rotational rate */
  Evu_rpm = 40,
  
  /* Acceleration */
  Evu_milligals       = 29,
  Evu_gravity         = 47,
  Evu_metersPerSec2   = 77,
  Evu_feetPerSec2     = 78,
  
  /* Force */
  Evu_klb             = 39,
  Evu_newtons         = 52,
  
  /* Gamma ray units */
  Evu_api             = 38,
  
  /* Torque */
  Evu_kftlb           = 41,
  Evu_newtonMeters    = 53,
  
  /* Voltage */
  Evu_volts           = 57,
  Evu_millivolts      = 58,
  
  /* Magnetic field strength */
  Evu_nanoTeslas      = 59,
  Evu_teslas          = 71,
  Evu_gauss           = 72,
  Evu_microGauss      = 73,
  
  /* Volumes */
  Evu_mcf             = 60,
  Evu_barrels         = 61,
  
  /* Extra units added for wellpath error modeling coefficients */
  Evu_degreesNanoTeslas = 80,
  Evu_perMeter        = 81,
  Evu_degreesPerHour2 = 82,
  Evu_degreesPerSqrtHour = 83,
  Evu_degreesPerMeter = 85,
  Evu_degreesPerHourPerGram = 86,
  
  Evu_next_available  = 92
};

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern int readUnrotated3DEVGrid (const char *gridName, char **desc,
				  float **grid, int *isBordered,
				  int *xcol, int *yrow, int *zlev,
				  double *xmin, double *xmax,
				  double *ymin, double *ymax,
				  double *zmin, double *zmax,
				  int *projection, int *ellipsoid,
				  int *zone, int *units,
				  double *projParms, int *zUnits, int *pUnits,
				  char **trouble);

extern int readUnrotated2DEVGrid (const char *gridName, char **desc,
				  float **grid, int *isBordered,
				  int *xcol, int *yrow,
				  double *xmin, double *xmax,
				  double *ymin, double *ymax,
				  int *projection, int *ellipsoid,
				  int *zone, int *units,
				  double *projParms, int *zUnits,
				  char **vfltFile, char **nvfltFile,
				  char **trouble);

extern int read3DEVGrid (const char *gridName, char **desc,
			 float **grid, int *isBordered,
			 int *xcol, int *yrow, int *zlev,
			 double *xpivot, double *xrange,
			 double *ypivot, double *yrange,
			 double *zpivot, double *zrange,
			 double *angle,
			 int *projection, int *ellipsoid,
			 int *zone, int *units,
			 double *projParms, int *zUnits, int *pUnits,
			 char **trouble);
     
extern int read2DEVGrid (const char *gridName, char **desc,
			 float **grid, int *isBordered,
			 int *xcol, int *yrow,
			 double *xpivot, double *xrange,
			 double *ypivot, double *yrange,
			 double *angle,
			 int *projection, int *ellipsoid,
			 int *zone, int *units,
			 double *projParms, int *zUnits,
			 char **vfltFile, char **nvfltFile,
			 char **trouble);

extern int readEVGrid (const char *gridName, int *is2D, char **descPtr,
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
		       char **trouble);

extern int writeUnrotated3DEVGrid (const char *gridName, char *desc,
				   float *grid,
				   int xcol, int yrow, int zlev,
				   double xmin, double xmax,
				   double ymin, double ymax,
				   double zmin, double zmax,
				   int projection, int ellipsoid,
				   int zone, int units,
				   double *projParms, int zUnits, int pUnits,
				   char **trouble);

extern int writeUnrotated2DEVGrid (const char *gridName, char *desc,
				   float *grid,
				   int xcol, int yrow,
				   double xmin, double xmax,
				   double ymin, double ymax,
				   int projection, int ellipsoid,
				   int zone, int units,
				   double *projParms, int zUnits,
				   char *vfltFile, char *nvfltFile,
				   char **trouble);

extern int write3DEVGrid (const char *gridName, char *desc,
			  float *grid,
			  int xcol, int yrow, int zlev,
			  double xpivot, double xrange,
			  double ypivot, double yrange,
			  double zpivot, double zrange,
			  double angle,
			  int projection, int ellipsoid, int zone, int units,
			  double *projParms, int zUnits, int pUnits,
			  char **trouble);

extern int write2DEVGrid (const char *gridName, char *desc,
			  float *grid,
			  int xcol, int yrow,
			  double xpivot, double xrange,
			  double ypivot, double yrange,
			  double angle,
			  int projection, int ellipsoid, int zone, int units,
			  double *projParms, int zUnits,
			  char *vfltFile, char *nvfltFile,
			  char **trouble);
     
extern int writeEVGrid (const char *gridName, int is2D, char *desc,
			float *grid,
			int xcol, int yrow, int zlev,
			double xpivot, double xrange,
			double ypivot, double yrange,
			double zpivot, double zrange,
			double angle,
			int projection, int ellipsoid, int zone, int units,
			double *projParms, int zUnits, int pUnits,
			char *vfltFile, char *nvfltFile,
			char **trouble);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


#endif /* __EVGRIDIO_H__ */
