/////////////////////////////////////////////////////////////////////////////
// Program version
#define VELMAN_SHORTVERSION "8.9.0"
#define VELMAN_LONGVERSION "VelocityManager Version 8.9.0"

/////////////////////////////////////////////////////////////////////////////
// Types of log files

#define LOG_BRIEF									0
#define LOG_STD										1
#define LOG_DETAIL									2

// this defines the light version of velman, disabling some menu items
// #define VelmanLight

// this defines the demo version of velman, disabling new project creation
//#define DEMO_VERSION

/////////////////////////////////////////////////////////////////////////////
// New Types

#define UBYTE			unsigned int

/////////////////////////////////////////////////////////////////////////////
// Types of error/warning messages

#define MSG_WARN 0x01   	// Warning, inlcude into detailed logs, then continue
#define MSG_ERR  0x02		// Error, always into logfile
#define MSG_CANCEL 0x80		// Set this bit to enable additional "Cancel" button

/////////////////////////////////////////////////////////////////////////////
// Possible stages in a project (bit-wise flags) to be contained in ProjStat
// note that these flags come in steps of two, so that we can easily
// insert further steps in between.

#define PRJ_HAVEWELLS		0x02	// have well depth/velocity data and survey data
#define PRJ_HAVEHORIZONS    0x04	// have names of horizons (in case no well data available)
#define PRJ_HAVESOMEMODELS  0x08	// have at least one time/depth dependence model
#define PRJ_HAVEMODEL       0x10	// have those models for every layer
#define PRJ_HAVERMSVEL		0x20	// have RMS or stacking velocity information in raw format
#define PRJ_PROCESSEDVEL	0x80	// have processed all velocities into RMS (?) format
#define PRJ_HAVETIME		0x200	// have time information from a ZIMS grid file
#define PRJ_HAVEDEPTHCONV  0x400	// have depth converted layers
#define PRJ_HAVEFAULTS     0x800	// have depth converted layers
#define PRJ_RAYTRACED      0x100    // have completed ray tracing

/////////////////////////////////////////////////////////////////////////////
// Possible project scopes
#define SCOPE_VELOCITIES 0			// velocity analysis only -- not implemented
#define SCOPE_TIMEWELL 1				// time and well data only
#define SCOPE_TIMERMS 2         // time and RMS data only
#define SCOPE_TIMEWELLRMS 3	    // time, RMS and well data

/////////////////////////////////////////////////////////////////////////////
// Special values that may occur during fitting
#include <float.h>	// needed for DBL_MAX
#define FIT_UNKNOWN DBL_MAX				// could not determine this fitting parameter

/////////////////////////////////////////////////////////////////////////////
// Hints passed to views to explain changes in the document

#define DOC_CHANGE_STATECHANGE 			200
#define DOC_CHANGE_FINETUNECHANGE 	210

/////////////////////////////////////////////////////////////////////////////
// ways of weighting wells
#define WEIGHTMODEL_DEFAULTS 1
#define WEIGHTMODEL_DECREASE 2

/////////////////////////////////////////////////////////////////////////////
// different modes for "coarse" model choice preview window
#define VIEWMODE_SCATTER 1
#define VIEWMODE_RESIDUAL 2

/////////////////////////////////////////////////////////////////////////////
// units used during the project; bits of the "Units" member variable of the
// document
#define UNIT_FEET_WELLDEPTHS 1	// bit set during "Project/New" if well depths are
//in feet. Also sets the following bit, but that may be reset by user later
#define UNIT_FEET_RMSVELOCITY 2	// bit set if RMS velocities in ft/sec
#define UNIT_OUTPUTINFEET 4			// bit set if anything done in feet

/////////////////////////////////////////////////////////////////////////////
// options for "Contour settings" dialog
#define CNTDLG_OPT_NOZOOM 1		// do not display help text on how to zoom

/////////////////////////////////////////////////////////////////////////////
// changes between ft/m etc.
#define METERS_PER_FOOT 0.3048

/////////////////////////////////////////////////////////////////////////////
// some constants regarding path name handling etc.
// we switch between win95 and unix, that's what this stuff is there for

#ifdef VELMAN_UNIX
#define ALL_FILES "*"
#define ALL_FILES_FILETYPE "All Files (*) | * ||"
#define TEXT_ALL_FILES "Text files (*.txt;*.prn;*.dat;*.vmv5)|*.txt;*.prn;*.dat;*.vmv5|All files (*)| * ||"
#else
#define ALL_FILES "*.*"
#define ALL_FILES_FILETYPE "All Files (*.*) | *.* ||"
#define TEXT_ALL_FILES "Text files (*.txt;*.prn;*.dat;*.vmv5)|*.txt;*.prn;*.dat;*.vmv5|All files (*.*)|*.*||"
#endif

