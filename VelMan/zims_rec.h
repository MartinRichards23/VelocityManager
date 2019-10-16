/******************************************************************
ZIMS HEADER FILE STRUCTURE
*******************************************************************/

#define  MAX_MSG_LEN     256            /* max error message length */
#define  MAX_LEN_IN      80             /* max len internal file name */
#define  MAX_LEN_FN      80             /* max len DOS file name */

#define  ZIMS_OK     0
#define  ZIMS_ERROR  1
#define  ZIMS_RECORD_LENGTH  128

typedef struct _zims_grid    ZIMSGRID;
typedef struct _zims_para    ZIMSPARA;
typedef struct _zims_field   ZIMSFIELD;
typedef struct _zims_header  ZIMSHEADER;
typedef long int             ZIMSWORD;

/* ZIMS file -- header record -- length = 64 ZIMSWORD's */

struct _zims_header
{
	char      filename[20];          /* 1-5: file's long name */
	long int  filetype;   /* 6: file's type */
	long int  ndxdat_day; /* 7: file creation - day */
	long int  ndxdat_mon; /* 8: file creation - month */
	long int  ndxdat_yr;  /* 9: file creation - year */
	long int  ndxdat_hr;  /* 10: file creation - hour */
	long int  ndxdat_min; /* 11: file creation - minute */
	long int  ndxnxd;     /* 12: rec # for next data record */
	long int  ndxnxh;     /* 13: rec # for next data header record */
	long int  ndxprm;     /* 14: rec # of parameter record */
	long int  ndxfld;     /* 15: rec # of data fields beginning position */
	long int  msc;        /* 16: */
	long int  open;       /* 17: */
	long int  ndxlen;     /* 18: */
	ZIMSWORD  unused1[14];
	char      filename1[20];      /* 33-37: support filename #1 */
	long int  filetype1;          /* 38: support file #1's type */
	ZIMSWORD  unused2[2];
	char      filename2[20];      /* 33-37: support filename #2 */
	long int  filetype2;          /* 38: support file #2's type */
	ZIMSWORD  unused3[18];
};

/* file type 1 -- parameter record for grids -- length = 32 ZIMSWORD's */

struct _zims_grid
{
	long int  zero;       /* 1: numeric value 0.0 */
	long int  nrows;      /* 2: # of rows */
	long int  ncolumns;   /* 3: # of columns */
	ZIMSWORD  unused1;
	double    xmin;       /* 5-6: xmin of grid */
	double    xmax;       /* 7-8: xmax of grid */
	double    ymin;       /* 9-10: ymin of grid */
	double    ymax;       /* 11-12: ymax of grid */
	float     gxinc;      /* 13: x increment */
	float     gyinc;      /* 14: y increment */
	float     xband;      /* 15: x expansion of collection region over AOI */
	float     yband;      /* 16: y expansion of collection region over AOI */
	float     reach;      /* 17: data collection radius */
	float     znon;       /* 18: znon value */
	float     gmax;       /* 19: zmax of grid */
	float     gmin;       /* 20: zmin of grid */
	ZIMSWORD  unused2[12];
};

/* file type 2 -- parameter record for data  -- length = 32 ZIMSWORD's */

struct _zims_para
{
	long int  nflds;      /* 1: number of fields on data record */
	long int  nwrds;      /* 2: number of words per data record */
	long int  nrecs;      /* 3: number of data records in file */
	long int  disttype;   /* 4: distribution type code */
	double    xmin;       /* 5-6: min x coordinate */
	double    xmax;       /* 7-8: max x coordinate */
	double    ymin;       /* 9-10: min y coordinate */
	double    ymax;       /* 11-12: max y coordinate */
	ZIMSWORD  unused1[5];
	float     znon;       /* 18: null data value for file */
	ZIMSWORD  unused2[2];
	float     separator;  /* 21: separator value for contour, seismic xsect */
	ZIMSWORD  unused3;
	long int  cntr;       /* 23: CNTR file mode (1, 2, or 3) */
	float     cint;       /* 24: contour interval if cntr=1 */
	ZIMSWORD  unused4[5];
	float     rotation;   /* 30: rotation angle */
	long int  proc_flag;  /* 31: process flag ( -1, 0, or 1) */
	long int  primary_sort;       /* 32: primary sort field */
};

/* file type 2 -- field descriptor record  -- length = 32 ZIMSWORD's */

struct _zims_field
{
	char      fldname[20];        /* 1-5: field name */
	long int  fldtype;    /* 6: field type */
	char      datafmt[4]; /* 7: FORTRAN edit descriptor ( A, G, F, E, I) */
	long int  fldwidth;   /* 8: total field display width */
	long int  numdec;     /* 9: floating point precision */
	long int  fldndx;     /* 10: index position for first word in each record */
	long int  nwords;     /* 11: number of words for field */
	ZIMSWORD  unused1;
	float     fldznon;    /* 13: field znon value */
	float     fldmin;     /* 14: field min value */
	float     fldmax;     /* 15: field max value */
	ZIMSWORD  unused2[4];
	long int  ndxsort;    /* 20: points to next secondary sort field */
	ZIMSWORD  unused3[12];
};

typedef struct _zdf     ZDF;            /* ZIMS Data File structure */
struct _zdf
{
	int      ft;                          /* file type (1 - 6) */
	char     fn_h[MAX_LEN_FN];            /* file name for header */
	char     fn_d[MAX_LEN_FN];            /* file name for data */
	int      fh_h;                        /* file handle for header */
	int      fh_d;                        /* file handle for data */
};

