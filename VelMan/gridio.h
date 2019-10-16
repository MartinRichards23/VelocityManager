
/* Prototypes for EarthVision Grid I/O routines */

extern "C" int gridRead2d(char *gridName, char **desc,
		       float **grid, int *xcol, int *yrow,
		       double *xmin, double *xmax, double *ymin, double *ymax,
		       float *nullVal,
		       char **datFile, char **datField,
		       char **vfltFile, char **nvfltFile,
		       int *projection, int *zone, int *units,
		       double *GCTPparms, int *zUnits);

extern "C" int gridWrite2d (char *gridName, char *desc,
			float *grid, int xcol, int yrow,
			double xmin, double xmax, double ymin, double ymax,
			float nullVal,
			char *datFile, char *datField,
			char *vfltFile, char *nvfltFile,
			int projection, int zone, int units,
			double *GCTPparms, int zUnits);

extern "C" int gridWrite3d (char *gridName, char *desc,
			float *grid, int xcol, int yrow, int zlev,
			double xmin, double xmax, double ymin, double ymax,
			double zmin, double zmax, float nullVal,
			char *datFile, char *datField,
			int projection, int zone, int units,
			double *GCTPparms, int zUnits, int pUnits);

extern "C" int gridRead3d (char *gridName, char **desc,
		       float **grid, int *xcol, int *yrow, int *zlev,
		       double *xmin, double *xmax, double *ymin, double *ymax,
		       double *zmin, double *zmax, float *nullVal,
		       char **datFile, char **datField,
		       int *projection, int *zone, int *units,
		       double GCTPparms[15], int *zUnits, int *pUnits);

extern "C" int xformGridWrite3d ( char *gridName, char *desc, float *grid,
                              int xcol, int yrow, int zlev,
                              double xmin, double xmax, double ymin,
                              double ymax, double zmin, double zmax,
                              float nullVal, char *datFile, char *datField,
                              int projection, int zone, int units,
                              double *GCTPparms, int zUnits, int pUnits,
                              char *type, double spacing,
                              char *topGrid, char *bottomGrid,
                              double topShift, double bottomShift,
                              int topPercent, int bottomPercent );

extern "C" int gridRead2dRot (char *gridName, char **desc,
                   float **grid, int *xcol, int *yrow,
                   double *xpivot, double *xrange,
                   double *ypivot, double *yrange,
                   double *alpha,
                   float *nullVal,
                   char **datFile, char **datField,
                   char **vfltFile, char **nvfltFile,
                   int *projection, int *zone, int *units,
		   double GCTPparms[15], int *zUnits);

extern "C" int gridWrite2dRot (char *gridName, char *desc,
                    float *grid, int xcol, int yrow,
                    double xpivot, double xrange, double ypivot, double yrange,
                    double alpha, float nullVal,
                    char *datFile, char *datField,
                    char *vfltFile, char *nvfltFile,
                    int projection, int zone, int units,
		    double *GCTPparms, int zUnits);

extern "C" int gridRead3dRot (char *gridName, char **desc,
                   float **grid, int *xcol, int *yrow, int *zlev,
                   double *xpivot, double *xrange,
                   double *ypivot, double *yrange,
                   double *zpivot, double *zrange,
                   double *alpha, float *nullVal,
                   char **datFile, char **datField,
                   int *projection, int *zone, int *units,
		   double GCTPparms[15], int *zUnits, int *pUnits);

extern "C" int gridWrite3dRot (char *gridName, char *desc,
                    float *grid, int xcol, int yrow, int zlev,
                    double xpivot, double xrange, double ypivot, double yrange,
                    double zpivot, double zrange, double alpha, float nullVal,
                    char *datFile, char *datField,
                    int projection, int zone, int units,
		    double *GCTPparms, int zUnits, int pUnits);

extern "C" int xformGridRead3d (
                      char    *gridName,
                      char   **desc,
                      float  **grid,
                      int     *xcol, int *yrow, int *zlev,
                      double  *xmin, double *xmax,
                      double  *ymin, double *ymax,
                      double  *zmin, double *zmax,
                      float   *nullVal,
                      char  **datFile, char **datField,
                      int     *projection, int *zone, int *units,
                                    double GCTPparms[15],
                      int     *zUnits, int *pUnits,
                      char   **type,         /* 3-D conformal grid
                                                transformation type:
                                                   none, both, top, bottom   */
                      double  *spacing,      /* z cell spacing in conformal
                                                space                        */
                      char   **topGrid,      /* Top conformal grid           */
                      char   **bottomGrid,   /* Bottom conformal grid        */
                      double  *topShift,     /* Top grid shift               */
                      double  *bottomShift,  /* Bottom grid shift            */
                      int     *topPercent,   /* Percentage flag for top xform*/
                      int     *bottomPercent /* Percentage flag for bottom
                                                xform                        */
		      );

extern "C" int xformGridRead3dRot (
                      char    *gridName,
                      char   **desc,
                      float  **grid,
                      int     *xcol, int *yrow, int *zlev,
                      double  *xpivot, double *xrange,
                      double  *ypivot, double *yrange,
                      double  *zpivot, double *zrange,
                      double  *alpha,
                      float   *nullVal,
                      char  **datFile, char **datField,
                      int     *projection, int *zone, int *units,
                                    double GCTPparms[15],
                      int     *zUnits, int *pUnits,
                      char   **type,         /* 3-D conformal grid
                                                transformation type:
                                                   none, both, top, bottom   */
                      double  *spacing,      /* z cell spacing in conformal
                                                space                        */
                      char   **topGrid,      /* Top conformal grid           */
                      char   **bottomGrid,   /* Bottom conformal grid        */
                      double  *topShift,     /* Top grid shift               */
                      double  *bottomShift,  /* Bottom grid shift            */
                      int     *topPercent,   /* Percentage flag for top xform*/
                      int     *bottomPercent /* Percentage flag for bottom
                                                xform                        */
		      );

extern "C" int xformGridWrite3d (
                       char *gridName,
                       char *desc,
                       float *grid,
                       int xcol,
                       int yrow,
                       int zlev,
                       double xmin,
                       double xmax,
                       double ymin,
                       double ymax,
                       double zmin,
                       double zmax,
                       float nullVal,
                       char *datFile,
                       char *datField,
                       int projection,
                       int zone,
                       int units,
                       double *GCTPparms,
                       int zUnits,
                       int pUnits,
                       char *type,         /* 3-D conformal grid
                                              transformation type:
                                                 none, both, top, bottom    */
                       double spacing,     /* z cell spacing in conformal
                                              space                         */
                       char *topGrid,      /* Top conformal grid            */
                       char *bottomGrid,   /* Bottom conformal grid         */
                       double topShift,    /* Top grid shift                */
                       double bottomShift, /* Bottom grid shift             */
                       int topPercent,     /* Percentage flag for top xform */
                       int bottomPercent   /* Percentage flag for bottom
                                              xform                         */
		       );

extern "C" int xformGridWrite3dRot (
                       char *gridName,
                       char *desc,
                       float *grid,
                       int xcol,
                       int yrow,
                       int zlev,
                       double xpivot,
                       double xrange,
                       double ypivot,
                       double yrange,
                       double zpivot,
                       double zrange,
                       double alpha,
                       float nullVal,
                       char *datFile,
                       char *datField,
                       int projection,
                       int zone,
                       int units,
                       double *GCTPparms,
                       int zUnits,
                       int pUnits,
                       char *type,         /* 3-D conformal grid
                                              transformation type:
                                                 none, both, top, bottom    */
                       double spacing,     /* z cell spacing in conformal
                                              space                         */
                       char *topGrid,      /* Top conformal grid            */
                       char *bottomGrid,   /* Bottom conformal grid         */
                       double topShift,    /* Top grid shift                */
                       double bottomShift, /* Bottom grid shift             */
                       int topPercent,     /* Percentage flag for top xform */
                       int bottomPercent   /* Percentage flag for bottom
                                              xform                         */
		       );
