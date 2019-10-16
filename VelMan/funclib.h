// Fitting functions required by the well model function library

typedef int BOOL;
#define FALSE               0
#define TRUE                1

BOOL LinearRegress(int, int, double *, double *, double *, double *, double *, double *, double *);
double LinearFunc(double, double, double);
double InverseLinearFunc(double, double, double);
BOOL ExpRegress(int, int, double *, double *, double *, double *, double *, double *, double *);
double ExpFunc(double, double, double);
BOOL FakeRegress(int, int, double *, double *, double *, double *, double *, double *, double *);
double FakeFunc(double, double, double);

double InstVelPropDepth(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropMDepth(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropMTime(double z1, double t1, double t2, double a, double b, double deltav);
double IsoChorPropIsoChron(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropTTime(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropBTime(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropTDepth(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropBDepth(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropIsochor(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelPropIsochron(double z1, double t1, double t2, double a, double b, double deltav);
double IntVelContoured(double z1, double t1, double t2, double a, double b, double deltav);
