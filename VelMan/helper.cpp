// VelocityManager small helper functions
// mainly used for MFD file import/export

// these should be almost self-explanatory
// (ie, could not be bothered to create extensive documentation...)
// standard includes needed
#ifdef VELMAN_UNIX
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <string.h>

#include "helper.h"
#include "unixcp3.h"

#ifndef VELMAN_UNIX
using namespace std;
#endif

extern int one_word_sec[];
char *cp3int_parm;
int  cp3int_parmlen;

// **********************************************************************
// Subroutine  : CopyParm()
// Arguments   : len:	length of parameter block in grid file
//			     param: character array with parameters from file
// Use         : loads parameters into grid class
// Returns     : none
// **********************************************************************
void CopyParm(int len, char *param)
{
	// create space for saved parameter block
	// (need to add code to delete assigned space)
	delete cp3int_parm;
	cp3int_parm = new char[len];
	for (int i=0;i<len;i++)
	{
		cp3int_parm[i] = param[i];
	}

	cp3int_parmlen = len;
}

// helper functions for CP3 reading/writing
int FortranRead(ifstream &file, char *data, int size)
{
	int realsize;

	/* Read CP3 Fortran data ignoring the record headers */
	/* This will return the real size only if it differs from the projected size... */

	file.read((char *) &realsize, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *) &realsize, one_word_sec);
#endif

	file.read(data, realsize);
	file.ignore(sizeof(CP3WORD));

	if(size != realsize)
		return(realsize);
	else
		return(0);
}

void FortranWrite(ofstream &file, char *data, int size)
{
	CP3WORD isize=size;

#ifndef VELMAN_UNIX
	InvertData((char *) &isize, one_word_sec);
#endif

	/* Write CP3 Fortran data including record headers */
	file.write((char *) &isize, sizeof(CP3WORD));
	file.write(data, size);
	file.write((char *) &isize, sizeof(CP3WORD));
}

// helper functions for MFD reading/writing
void zero_mem(char *ptr, int length)
{
	for(int i=0;i<length;i++)
		ptr[i] = 0x00;
}

// remove trailing spaces
void UnPad(char *string, int length)
{
	for(int i=length-1;i>=0;i--)
	{
		if(isspace(string[i]))
			string[i] = 0x00;
		else
			break;
	}

	string[length-1] = 0x00;
}

// add trailing spaces
void Pad(char *string, int length)
{
	int i=length-1;

	while(string[i] == 0x00) {
		string[i] = 0x20;
		i--;
	}
}

// invert byte order in a data structure so we can read Intel format
// files on Unix and vice versa.
// section[] is an array, containing pairs n, i
// i is mostly 4 and 8, or 0, 0 to terminate.
// n gives number of items of length i
// inverts n items by reversing the order of 4 (or 8) bytes,
// then goes on to next data element

void InvertData(char *data, int section[])
{
	int i=0, j, k;
	char tmp;

	while(section[i] != 0) {
		for(k=0;k<section[i];k++) {
			for(j=0;j<section[i+1]/2;j++) {
				tmp = data[j];
				data[j] = data[section[i+1]-j-1];
				data[section[i+1]-j-1] = tmp;
			}
			data += section[i+1];
		}
		i+=2;
	}
}

// sections for MFD InvertData()
int meta_file_head_sec[] =
{
	5, 4,
	24, 1,
	4, 4
};

int file_head_sec[] =
{
	24, 1,
	10, 4,
	0, 0
};

int file_info_sec[] =
{
	24, 1,
	2, 4,
	44, 1,
	3, 4,
	0, 0
};

int three_numbers_sec[] =
{
	3, 4,
	0, 0
};

int zims_grid_sec[] =
{
	4, 4,
	4, 8,
	8, 4,
	0, 0
};

int zims_para_sec[] =
{
	4, 4,
	4, 8,
	20, 4,
	0, 0
};

int zims_field_sec[] =
{
	20, 1,
	1, 4,
	4, 1,
	13, 4,
	0, 0
};

int one_word_sec[]=
{
	1, 4,
	0, 0
};

int one_dword_sec[]=
{
	1, 8,
	0, 0
};

int zims_header_sec[] =
{
	20, 1,
	27, 4,
	20, 1,
	3, 4,
	20, 1,
	1, 4,
	0, 0
};

// sections for CP3 InvertData()
int cp3_file_head_sec[] =
{
	4, 4,
	0, 0
};

int cp3_grid_sec[] =
{
	10, 4,
	56, 1,
	1, 4,
	0, 0
};

int cp3_control_sec[] =
{
	256, 4,
	0, 0
};

int cp3_fault_sec[] =
{
	6, 4,
	0, 0
};

