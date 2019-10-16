// VelmanDoc.h : interface of the CVelmanDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "seisdata.h"
#include "wellclas.h"
#include "WellPoint.h"
#include "funclib.h"
#include "velmanpalette.h"
#include "contour.h"

class CProgressMonit; // dto. for pointer m_Progress;
class CWellModelFunction;	// see below
class CModelChoiceDlg;
class CCoarseModelView;
class CFineTuneView;
class CFinetuneSheet;
class CFineTuneDlg;
class CChooseAnyGrid;	// also a dialog
class CChooseManyGridsDlg;	// also a dialog

class CVelmanDoc : public CDocument
{
protected: // create from serialization only
	CVelmanDoc();
	DECLARE_DYNCREATE(CVelmanDoc)

	// Attributes
public:

	WORD ProjState;		// Where am I in the project?
	CString Title;		// Title as entered during "new project" phase
	CString BaseName;	// base name for files = name of new subdirectory
	WORD LogType;			// detail level of the log file
	char lpTempPath[4096];	// Pathname for temporary files
	CString TargetDir;	// Complete path to target directory
	CString ExportDir;	// Complete path to directory where exported files are put
	CString WellTiDp;	// Complete path well time/depth file
	CString WellSurv;	// Complete path well survey file
	CString ProjectName;	// Complete path project document file
	CStringArray RMSsource;	// Complete path to RMS velocity raw data file
	CString ZimsDir;		// Complete path to ZIMS directory (contains several data files)
	int ProjectScope;		// defines which import files are needed and what output is requested
	WORD Units;					// Bitfield on various units used; see global.h for bit defs.

	CString MFDName; //mfd grids are stored together, so name is created here, rather than by individual grids

	CHorizonList horizonlist;
	CSeismicData seismicdata;
	CFaultCube faultcube;
	CZimsCube zimscube;
	CWellPointList wellpointlist;
	CHorizonList secondaryhorizonlist;  //unused

	// distance of two time slices when splining RMS time/velocity pairs to fixed time horizons
	// ("slices"). Is read from INI file during InitInstance()
	double RMSTimeSliceInterval;

	// internal helper vars
	BOOL HaveValidDocument;	// TRUE only if complete project creation went OK
	BOOL ModelViewOpen;	// if TRUE, there are extra windows on screen to let the
	BOOL PressCancel;
	// user choose a model. In that case, the "Model/Create Model" menu item
	// grays itself. Same is FALSE for "Model/Show residual table".
	BOOL ListOfModelsOpen, TyingTableOpen;
	BOOL TimegridContourOpen;
	BOOL SeisdataMapOpen;
	int OpenPreviewViews;		// Bits 1, 2, 3 contain TRUE if the coarse model/contour
	// residual/numerical residuals preview windows are open, respectively. If
	// the user closes any of these windows, they reset that bit and therefore the
	// "Model choice" control dialog does not attempt to close them again (which
	// crashes the system)
	// this is used to build a well model function library (see constructor and
	// desctructor)

	CString VMLightMessage; // warning message about features not available in light version

	int UsePseudoVelocities; // whether we are using pseudo velocities for the time values from the time/data well file.
	bool VerticaliseWells; // whether we count wells outside the grid as vertical.

private:
	int num_model_funcs;
	CWellModelFunction *well_lib1, *well_lib2, *well_lib3, *well_lib4, *well_lib5;
	CWellModelFunction *well_lib6, *well_lib7, *well_lib8, *well_lib9, *well_lib10;
	CWellModelFunction *well_lib11;
public:
	CWellModelFunction **well_model_lib;
public:
	CProgressMonit *m_pProgress;

	CStdioFile m_logfile;	// handles the log file; opened in OnNewDocument

	// variable and function to do DOSname -> ZIMS grid name mapping
	CStringArray ListOfDOSnames;	// array of all files *.?H in zims directory
	void FillListOfDOSnames();		// clears, then fills that array using zimsdir as directory
	void FillListOfDOSnames(CString Directory);		// clears, then fills that array

private:    // holds options for raytracing properties
	static BOOL rt_azimuthgrids;    // whether azimuthal displacement grids are computed & stored.
	static BOOL rt_anglegrids;  // whether angle displacement grids are computed & stored.
	static BOOL rt_depthdiffgrids; // whether depth difference grids are computed & stored.
	static BOOL rt_xyoffsetgrids;  // whether x&y offset grids are saved.
	static double rt_limitanglevalue;  // angle to stop ray on.
	static int  rt_regridbinsize;   // index into regridding cell size array (see raytrace dialogue)
	
public:

	// Interaction between the document and its views
public:
	CModelChoiceDlg *pCMdlg;	// the choose model view needs this ptr to close the dlg
	CFrameWnd *pCoarseModelFrame;	// so the view can close its own frame
	CCoarseModelView *pCMview;	// so the dialog can access the view (set by view itself)
	CFineTuneView *pFTview;
	CChooseAnyGrid *pTCdlg;
	int TieTableChosenType;
	int GridFileType;
	CFinetuneSheet *pFTsheet;
	CFineTuneDlg *pFTdlg;
	BOOL bRebuildDepth;
	int ActiveLayer;		// number of layer currently being worked on.
	double *xpos[4], *ypos[4], *residuals[4];	// will be dynamic lists for coarse
	double *xvals[4], *yvals[4];									// model choice
	double *z1vals[4];
	int length[4];
	int model[4], weight[4];
	BOOL HaveFit[4];		// those store details on the coarse fit for the 4 funcs
	double FitA[4], FitB[4], FitChi[4];
	int *databaseindex[4];	// databaseindex[][i] = index of xval[][i] in the
	// horizonlist.GetAt(..) database. That would be equal to i, if all points
	// in the database did actually exist, but some of them are empty or invalid,
	// and are not taken into the dynamic list.
	void FillDataList(int no, int layer, int model, int weight);	// fills one of
	// these lists and sets the model[] and weight[] entries

	// Operations
public:
	void Log(CString text, WORD level);		// write a string into a logfile at a level
	BOOL Error(CString text, BOOL level);	// error msg, TRUE if error, otherwise warning
	// Implementation
public:
	virtual ~CVelmanDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
#ifdef VELMAN_UNIX
	CString GetNextFiltername(int &error, ifstream *stream, CString *type);
#else
	CString GetNextFiltername(int &error, std::ifstream *stream, CString *type);
#endif
	CString GetFilterFileDir();
	CString GetExportDir( );

	int GetNumModelFuncs() { return num_model_funcs; }
	int DepthConvertLayer(int layer);
	int DepthConvertModelLayer(int layer, double &min, double &max);
	int ComputeIntervalvelocity(int layer, double &min, double &max);

	int DepthConvertModelLayerMulti(int layer, BOOL UseResiduals, BOOL TotalDeselection, CZimsGrid *depthgrid, CZimsGrid *corrgrid, BOOL &NegativeIntervalDepth, double &minisochore, double &maxisochore, double &minVint, double &maxVint);
	int DepthConvertModelLayerAdd(int layer, BOOL UseResiduals, BOOL TotalDeselection, CZimsGrid *depthgrid, CZimsGrid *corrgrid, BOOL &NegativeIntervalDepth, double &minisochore, double &maxisochore, double &minVint, double &maxVint);

	int WarnCannotConvertLayer(int layer);
	int WarnCannotConvertSecondaryGrid(CZimsGrid *grid);
	BOOL HaveFaultData(void)
	{
		if(faultcube.GetSize() != 0) return TRUE;
		else return FALSE;
	}

	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(const char *);
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(const char *pathname);

	// Generated message map functions
protected:
	//{{AFX_MSG(CVelmanDoc)
	afx_msg void OnDepthconversionConvertsecondarytimegrid();
	afx_msg void OnDepthconversionConverttimeslice();
	afx_msg void OnDepthconversionCreatedepthgrids();
	afx_msg void OnDepthconversionVelocitycubeConvertsinglewelllocations();
	afx_msg void OnDepthconversionVelocitycubeVelocitytimeslice();
	afx_msg void OnDepthconversionVelocitycubeWellforecast();
	afx_msg void OnGridsCreateconstantgrid();
	afx_msg void OnGridsDelete();
	afx_msg void OnGridsDuplicate();
	afx_msg void OnGridsExport();
	afx_msg void OnGridsFFTSmooth();
	afx_msg void OnGridsFouriertransform();
	afx_msg void OnGridsGridarithmetic();
	afx_msg void OnGridsImport();
	afx_msg void OnGridsMatrixSmooth();
	afx_msg void OnGridsOverwritenull();
	afx_msg void OnGridsRebuild();
	afx_msg void OnGridsRename();
	afx_msg void OnGridsSpectrumanalysis();
	afx_msg void OnGridsTietowelldata();
	afx_msg void OnGridtoolsDesignate();
	afx_msg void OnGridtoolsDirectory();
	afx_msg void OnGridtoolsLimit();
	afx_msg void OnGridtoolsStatistics();
	afx_msg void OnHelpWhatnow();
	afx_msg void OnImportFiltermanagement();
	afx_msg void OnImportRmsData();
	afx_msg void OnImportTimeData();
	afx_msg void OnOpenDemoVideo();
	afx_msg void OnProcessRmsVelocities();
	afx_msg void OnProject3dvis();
	afx_msg void OnExportWellData();
	afx_msg void OnProjectViewgriddatabase();
	afx_msg void OnProjectViewlogfile();
	afx_msg void OnRaytrace();
	afx_msg void OnRaytrace3dProperties();
	afx_msg void OnSeismicdataTestrmsimport();
	afx_msg void OnUpdateDepthconversionConvertsecondarytimegrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthconversionConverttimeslice(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthconversionCreatedepthgrids(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthconversionVelocitycubeConvertsinglewelllocations(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthconversionVelocitycubeVelocitytimeslice(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthconversionVelocitycubeWellforecast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGridsContourplot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGridsTietowelldata(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImportImportrmsdata(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImportImporttimedata(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcessProcessrmsvelocities(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRaytrace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRaytrace3dProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSeismicdataMapofshotpoints(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSeismicdataTestrmsimport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVelocityvolumeComputeallintervalvelocities(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVelocityvolumeImrconverttimegrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWellmodellingImportwelldata(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWellshorizonsDefinehorizons(CCmdUI* pCmdUI);
	afx_msg void OnVelocityvolumeComputeallintervalvelocities();
	afx_msg void OnVelocityvolumeImrconverttimegrid();
	afx_msg void OnImportWellData();
	afx_msg void OnWellshorizonsDefinehorizons();
	afx_msg void OpenVideo(CString FileName);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void DesignateGrid(CZimsGrid *grid, int Horizon, bool Message);
	afx_msg void OnDemonstrationvideos3dvisualisation();
	afx_msg void OnDemonstrationvideosDepthconvertsecondarygrid();
	afx_msg void OnDemonstrationvideosDepthmigration();
	afx_msg void OnDemonstrationvideosFunctionmodelling();
	afx_msg void OnDemonstrationvideosFunctionqualitycontrol();
	afx_msg void OnDemonstrationvideosFunctionrefinement();
	afx_msg void OnDemonstrationvideosFunctionresidualcontrols();
	afx_msg void OnDemonstrationvideosGettingstartedwithrmsvelocities();
	afx_msg void OnDemonstrationvideosImportingsegydata();
	afx_msg void OnDemonstrationvideosImrii();
	afx_msg void OnDemonstrationvideosProcessvelocitydata();
	afx_msg void OnDemonstrationvideosRmsqualitycontrol();
	afx_msg void OnDemonstrationvideosSavinggrids();
	afx_msg void OnDemonstrationvideosSavingwelldata();
	afx_msg void OnDemonstrationvideosWelldataformat();
	afx_msg void OnProjectSaveprojectreport();
	afx_msg void OnSeismicdataOpensegyconverter();
	afx_msg void OnUpdateSeismicdataOpensegyconverter(CCmdUI *pCmdUI);
	afx_msg void OnVelocityvolumeImrtest();
	afx_msg void OnWelldataDeletewell();
	afx_msg void OnWelldataInterpolatefaultnullpolygons();	
	afx_msg void OnUpdateProject3dvis(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsOverwritenull(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridtoolsDirectory(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridtoolsLimit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridtoolsDesignate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridtoolsStatistics(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsSpectrumanalysis(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsDuplicate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsCreateconstantgrid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsRename(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsFFTSmooth(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsMatrixSmooth(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsRebuild(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsImport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsGridarithmetic(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsFouriertransform(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsExport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridsDelete(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImportFiltermanagement(CCmdUI *pCmdUI);
	afx_msg void OnUpdateWelldataDeletewell(CCmdUI *pCmdUI);
	afx_msg void OnUpdateWelldataInterpolatefaultnullpolygons(CCmdUI *pCmdUI);
};

//*************************************************************************
// Class:				CWellModelFunction
// Arguments:   none
// Description:	Description of a model function. Several members of this class
//							form a global array that can be accessed e.g. by dialog boxes
//							that let the user choose a particular function. Also a chosen
//							function can be stored as a single number giving the index in
//							that very array.
// Variables:		name			  = short name of function (used in lists)
//							explanation = short explanation of function (currently unused)
//							paramnameA, B    = names of the fitting parameters (e.g. "slope")
//							fit					= ptr to routine that performs fitting. Is given
//														distance weighting (0=linear, 1=quadratic),
//														number of (x, y) points, list of z1 points of layer above,
// 														data lists x, y. Returns fitting parameters a, b
//							a, b					= members that store a copy of a, b after calls
//														to storeparams(). Can be retrieved elsewhere before
//														calls to func()
//							func				= ptr to function that returns y(x) for given x, a, b
//							storeparams = member function that stores a, b as computed by fit()
//							IsLinear		= flag, TRUE if fit is linear function (faster plotting)
class CWellModelFunction
{
public:
	CString name, explanation;
	CString parnameA, parnameB;
	BOOL (*fit)(int, int, double *, double *, double *, double *, double *, double *, double *);
	double (*func)(double, double, double);
	double (*inversefunc)(double, double, double);
	double (*depth)(double, double, double, double, double, double);
	BOOL IsLinear;
	double a, b;
	BOOL valid_fit;

	void storeparams(double param_a, double param_b, BOOL correct_m)
	{ a=param_a;b=param_b; valid_fit=correct_m; }

	CWellModelFunction(CString nam, CString expl, CString pA, CString pB,
		BOOL (*p1)(int, int, double *, double *, double *, double *, double *, double *, double *),
		double (*p2)(double, double, double, double, double, double),
		double (*p3)(double, double, double), double (*p4)(double, double, double), BOOL IL)
	{ 
		name=nam;explanation=expl;parnameA=pA;parnameB=pB;
		fit=p1;depth=p2;func=p3, inversefunc=p4;
		IsLinear=IL;
		a=0.0;b=0.0;valid_fit=FALSE;
	}

};

// Used to pass around dimensions of grid. note, use this more.
struct Dimensions
{
		double MaxX;
		double MinX;
		double MaxY;
		double MinY;
};

//*************************************************************************
// These constants describe changes in the document that have to be reflected
// by associated views. See the OnUpdate() functions of the views for further info

#define DOC_CHANGE_MODELCHOICE 100
