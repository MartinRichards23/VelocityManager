// well point interrogation

// allows interrogation of the velocity cube at just a single point (x, y, depth),
// returning the 2-way time to that point.

// this class handles storing and I/O of the required datapoints

// status of these single points
#define WLPT_STATUS_INVALID		0x01
#define WLPT_STATUS_HAVETIME	0x02

class CWellPointList : public CObArray
{
public:
	~CWellPointList() { ClearContents(); }
	void Init(CVelmanDoc *document) { pDoc=document; }
	int ReadFromFile(const char *filename);
	int WriteToFile(const char *filename);
	void SortByName();
	int interp_method, extrap_gradient_percent;		// passed on to zimscube
	void ClearContents();

private:
	int CompareAndSwap(int pos);
#ifdef VELMAN_UNIX
	BOOL GetNewLine(ifstream &file, CString &line);
#else
	BOOL GetNewLine(std::ifstream &file, CString &line);
#endif
	CVelmanDoc *pDoc;
};

class CWellPoint : public CObject
{
public:
	double x, y, d, t;
	int status;
	CString name;
	void TimeConvert(CWellPointList *wellpointlist, CZimsCube *zimscube);

	CWellPoint()
	{
		x=0.0;y=0.0;
		d=0.0;t=0.0;
		status=0;
		name="";
	}

	CWellPoint(double newx, double newy, double newd)
	{
		x=newx;
		y=newy;
		d=newd;
		t=0.0;
		status=0;
		name="";
	}

	CWellPoint(double newx, double newy, double newd, CString &newname)
	{
		x=newx;
		y=newy;
		d=newd;
		t=0.0;
		status=0;
		name=newname;
	}
};
