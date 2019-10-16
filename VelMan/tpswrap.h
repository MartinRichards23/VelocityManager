#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include "nrutil.h"
#include "distmat.h"
#include "fetps.h"

double **GEvaluateGridTPS(double **p, double *c, int n, Point *zz, int rs, int cs, int fast);
