/******************************************************************
CP3 HEADER FILE STRUCTURE
*******************************************************************/
#ifndef  __UNIX_CP3_REC__
#define  __UNIX_CP3_REC__

#define CP3WORD int

/* possible types for the itdatt field */
#define CP3_ITDATT_WELL     1
#define CP3_ITDATT_CONT     2
#define CP3_ITDATT_2DSEIS   3
#define CP3_ITDATT_3DSEIS   4
#define CP3_ITDATT_BATHY    5
#define CP3_ITDATT_IRREG    6
#define CP3_ITDATT_FEATURE  7
#define CP3_ITDATT_DEVWELL  8

/* possible values for the iodatt term */
#define CP3_IODATT_NOSLOPE  0
#define CP3_IODATT_2SLOPE   1
#define CP3_IODATT_CURVE    2

/* file version header */

struct cp3_file_head {
	CP3WORD  ifile;        // always 0x01
	CP3WORD  ivnum;        // data file version number
	CP3WORD  nsubs;        // number of data subsets
	CP3WORD  itype;        // integer set type
};

/* grid structure header 1024 bytes */

struct cp3_grid {
	CP3WORD  nrow;         // number of grid rows
	CP3WORD  ncol;         // number of grid columns
	float    zmin;         // grid z-minimum
	float    zmax;         // grid z-maximum
	float    xmin;         // grid x-minimum
	float    xmax;         // grid x-maximum
	float    ymin;         // grid y-minimum
	float    ymax;         // grid y-maximum
	float    xinc;         // grid x-increment
	float    yinc;         // grid y-increment
	char     unused0[56];
	float    findt;        // grid indt (null) value
	char     unused1[924];
};

/* control point header 1024 bytes */

struct cp3_control {
	CP3WORD  itdatt;       // datatype
	CP3WORD  nidatt;       // number of dependant variables
	CP3WORD  nhdatt;       // number of independant variables
	CP3WORD  iodatt;       // slope components
	float    xmin;         // point x-minimum
	float    xmax;         // point x-maximum
	float    ymin;         // point y-minimum
	float    ymax;         // point y-maximum
	float    zdata[248];   // z extremes and znon value
};

/* fault header 1024 bytes */

struct cp3_fault {
	CP3WORD  itfatt;       // elevation side for vertical faults
	CP3WORD  iafatt;       // attribute index
	float    xmin;         // set x-minimum
	float    xmax;         // set x-maximum
	float    ymin;         // set y-minimum
	float    ymax;         // set y-maximum
	char     unused0[1000];
};

/* internal CP3 item header */
struct CP3intItem {
	char item_id[24];
	char item_val[64];
};

#endif
