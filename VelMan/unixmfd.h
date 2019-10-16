/******************************************************************
ZIMS HEADER FILE STRUCTURE, Unix MFD format
*******************************************************************/
#ifndef __UNIX_ZIMS_REC__

#define  __UNIX_ZIMS_REC__

#define  ZIMS_BLOCK 0x800
#define  ZIMSWORD int

/* Real file header starting at $00 */
/* This is 128 bytes long */

struct meta_file_head {
	ZIMSWORD filenum;     /* number of valid file headers to follow */
	ZIMSWORD meta_next;   /* pointer to next metafile header if present */
	ZIMSWORD unused0;     /* always zero */
	ZIMSWORD endoff;      /* pos of next $800 block at the end of the file */
	ZIMSWORD four;        /* always four */
	char filename[24];    /* real filename on disk (space padded) */
	ZIMSWORD skipinfo0;   /* only nonzero in uncompressed files */
	ZIMSWORD skipinfo1;   /* only nonzero in uncompressed files */
	ZIMSWORD skipinfo2;   /* only nonzero in uncompressed files */
	ZIMSWORD skipinfo3;   /* only nonzero in uncompressed files */
	ZIMSWORD unused1[17]; /* seem not to be in use */
};

/* File header starting at $80 */
/* each one is 128 bytes long */

struct file_head {
	char dataname[24];    /* name of subfile (space padded) */
	ZIMSWORD datatype;    /* 1==grid, 2==shotline 5==polygon */
	ZIMSWORD res0;        /* residue in bytes of data info from header info, */
	/* normally 0 -- different in xfiles */
	ZIMSWORD block0;      /* spacing in $200 byte blocks of data info from */
	/* header info, normally 4 */
	ZIMSWORD unused0;     /* always 0 */
	ZIMSWORD off3;        /* offset of 3 number code in $800 blocks */
	/* This is always header_off + block2 + $4 */
	ZIMSWORD res2;        /* residue of data length from $800 data blocks */
	ZIMSWORD block2;      /* $800 byte data blocks */
	ZIMSWORD header_off;  /* offset of header in $800 blocks */
	ZIMSWORD five;        /* always five */
	ZIMSWORD meta_head;   /* refers back to meta_file_head of this file */
	ZIMSWORD unused1[16]; /* seem not to be in use */
};

/* File information starting  */
/* each one is 128 bytes long */

struct file_info {
	char dataname[24];    /* name of data set (space padded) */
	ZIMSWORD datatype;    /* 1==grid, 2==shotline 5==polygon */
	ZIMSWORD one;         /* usually 1 */
	char filename[24];    /* This is the name of the original file */
	char date[20];        /* last modification date in ASCII */
	ZIMSWORD ident;       /* 1 for grids & polygons, 0 for shotline */
	ZIMSWORD twoeight0;   /* usually $28  -- different in xfiles */
	ZIMSWORD twoeight1;   /* usually $28  -- different in xfiles */
	ZIMSWORD unknown;     /* usually 0  -- different in xfiles */
	ZIMSWORD unused1[9]; /* seem not to be in use */
};

/* Three numbers checksum */
/* each one is 128 bytes long */

struct three_numbers {
	ZIMSWORD off3p;       /* always off3+$2; */
	ZIMSWORD one;         /* always $1; */
	ZIMSWORD fifteen;     /* always $f */
	ZIMSWORD unused0[29]; /* seem not to be in use */
};

/* zims grid header */
/* each one is 128 bytes long */

struct zims_grid {
	ZIMSWORD  zero;       /* 0: numeric value 0.0 */
	ZIMSWORD  nrows;      /* 1: # of rows */
	ZIMSWORD  ncolumns;   /* 2: # of columns */
	ZIMSWORD  unused0;    /* 3: seem not to be in use */
	double xmin;           /* 4: xmin of grid */
	double xmax;           /* 6: xmax of grid */
	double ymin;           /* 8: ymin of grid */
	double ymax;           /* 10: ymax of grid */
	float gxinc;          /* 12: x increment */
	float gyinc;          /* 13: y increment */
	float xband;          /* 14: x expansion of collection region over AOI */
	float yband;          /* 15: y expansion of collection region over AOI */
	float reach;          /* 16: data collection radius */
	float znon;           /* 17: znon value */
	float gmax;           /* 18: zmax of grid */
	float gmin;           /* 19: zmin of grid */
	ZIMSWORD unused5[12]; /* 20-31: seem not to be in use */
};

struct zims_para {
	ZIMSWORD  nflds;         /* 1: number of fields on data record */
	ZIMSWORD  nwrds;         /* 2: number of words per data record */
	ZIMSWORD  nrecs;         /* 3: number of data records in file */
	ZIMSWORD  disttype;      /* 4: distribution type code */
	double    xmin;          /* 5-6: min x coordinate */
	double    xmax;          /* 7-8: max x coordinate */
	double    ymin;          /* 9-10: min y coordinate */
	double    ymax;          /* 11-12: max y coordinate */
	ZIMSWORD  unused1[5];
	float     znon;          /* 18: null data value for file */
	ZIMSWORD  unused2[2];
	float     separator;     /* 21: separator value for contour, seismic xsect */
	ZIMSWORD  unused3;
	ZIMSWORD  cntr;          /* 23: CNTR file mode (1, 2, or 3) */
	float     cint;          /* 24: contour interval if cntr=1 */
	ZIMSWORD  unused4[5];
	float     rotation;      /* 30: rotation angle */
	ZIMSWORD  proc_flag;     /* 31: process flag ( -1, 0, or 1) */
	ZIMSWORD  primary_sort;  /* 32: primary sort field */
};

/* file type 2 -- field descriptor record  -- length = 20 ZIMSWORD's */

struct zims_field {
	char      fldname[20];        /* 1-5: field name */
	ZIMSWORD  fldtype;    /* 6: field type */
	char      datafmt[4]; /* 7: FORTRAN edit descriptor ( A, G, F, E, I) */
	ZIMSWORD  fldwidth;   /* 8: total field display width */
	ZIMSWORD  numdec;     /* 9: floating point precision */
	ZIMSWORD  fldndx;     /* 10: index position for first word in each record */
	ZIMSWORD  nwords;     /* 11: number of words for field */
	ZIMSWORD  unused1;
	float     fldznon;    /* 13: field znon value */
	float     fldmin;     /* 14: field min value */
	float     fldmax;     /* 15: field max value */
	ZIMSWORD  unused2[4];
	ZIMSWORD  ndxsort;    /* 20: points to next secondary sort field */
};
#endif
