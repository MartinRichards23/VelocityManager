// velmadoc.cpp : implementation of the CVelmanDoc class

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "2dsuppDlg.h"
#include "ChooseAGridDlg.h"
#include "ChooseManyGridsDlg.h"
#include "ChooseWellDlg.h"
#include "ConstantGridDlg.h"
#include "dctsDlg.h"
#include "DefineHorDlg.h"
#include "DesignateGridDlg.h"
#include "fft_Dlg.h"
#include "FiltManDlg.h"
#include "ft_sheet.h"
#include "gdblDlg.h"
#include "global.h"
#include "GridArithDlg.h"
#include "GridExpDlg.h"
#include "GridImpDlg.h"
#include "GridRenameDlg.h"
#include "GridStatDlg.h"
#include "httDlg.h"
#include "ImportErrorDlg.h"
#include "ImpWellHorDataDlg.h"
#include "IMRDlg.h"
#include "LimitGridDlg.h"
#include "LineCodeDlg.h"
#include "mainfrm.h"
#include "NewProj1Dlg.h"
#include "NewProj2Dlg.h"
#include "NewProj3Dlg.h"
#include "openfilebrowser.h"
#include "OverwriteNullDlg.h"
#include "plogDlg.h"
#include "ProgressDlg.h"
#include "ProjGuideDlg.h"
#include "raytracepropsDlg.h"
#include "rebsDlg.h"
#include "RmsImportDlg.h"
#include "sdcoDlg.h"
#include "sgo_Dlg.h"
#include "SpecAnalysisDlg.h"
#include "swoDlg.h"
#include "tcdptDlg.h"
#include "TestImpResDlg.h"
#include "tieimDlg.h"
#include "WellForecastDlg.h"
#include "WellhorzDlg.h"

#include <shlwapi.h>
#include <direct.h>
#include <dos.h>
#include <io.h>
#include <process.h>
#include <tchar.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVelmanDoc

// Raytracing static variables (holds values of raytracing properties)

BOOL CVelmanDoc::rt_azimuthgrids = FALSE;
BOOL CVelmanDoc::rt_anglegrids = FALSE;
BOOL CVelmanDoc::rt_depthdiffgrids = FALSE;
BOOL CVelmanDoc::rt_xyoffsetgrids = FALSE;
double CVelmanDoc::rt_limitanglevalue = 35.0;
int CVelmanDoc::rt_regridbinsize = 3;

// For 3D viz
bool IsNetfx40ClientInstalled();

IMPLEMENT_DYNCREATE(CVelmanDoc, CDocument)

#define theDerivedClass CVelmanDoc ::
	BEGIN_MESSAGE_MAP(CVelmanDoc, CDocument)
		//{{AFX_MSG_MAP(CVelmanDoc)
		ON_COMMAND(ID_HELP_WHATNOW, OnHelpWhatnow)
		ON_COMMAND(ID_EXPORT_WELLDATA, OnExportWellData)
		ON_COMMAND(ID_PROJECT_SAVEPROJECTREPORT, &CVelmanDoc::OnProjectSaveprojectreport)
		ON_COMMAND(ID_PROJECT_VIEWLOGFILE, OnProjectViewlogfile)
		// well data
		ON_COMMAND(ID_WELLMODELLING_IMPORTWELLDATA, OnImportWellData)
		ON_UPDATE_COMMAND_UI(ID_WELLMODELLING_IMPORTWELLDATA, OnUpdateWellmodellingImportwelldata)	
		ON_COMMAND(ID_IMPORT_IMPORTTIMEDATA, OnImportTimeData)
		ON_UPDATE_COMMAND_UI(ID_IMPORT_IMPORTTIMEDATA, OnUpdateImportImporttimedata)
		ON_COMMAND(ID_WELLDATA_DELETEWELL, &CVelmanDoc::OnWelldataDeletewell)
		ON_UPDATE_COMMAND_UI(ID_WELLDATA_DELETEWELL, &CVelmanDoc::OnUpdateWelldataDeletewell)
		ON_COMMAND(ID_WELLDATA_INTERPOLATEFAULTNULLPOLYGONS, &CVelmanDoc::OnWelldataInterpolatefaultnullpolygons)
		ON_UPDATE_COMMAND_UI(ID_WELLDATA_INTERPOLATEFAULTNULLPOLYGONS, &CVelmanDoc::OnUpdateWelldataInterpolatefaultnullpolygons)
		// seismic data
		ON_COMMAND(ID_WELLSHORIZONS_DEFINEHORIZONS, OnWellshorizonsDefinehorizons)
		ON_UPDATE_COMMAND_UI(ID_WELLSHORIZONS_DEFINEHORIZONS, OnUpdateWellshorizonsDefinehorizons)
		ON_COMMAND(ID_SEISMICDATA_OPENSEGYCONVERTER, &CVelmanDoc::OnSeismicdataOpensegyconverter)
		ON_UPDATE_COMMAND_UI(ID_SEISMICDATA_OPENSEGYCONVERTER, &CVelmanDoc::OnUpdateSeismicdataOpensegyconverter)	
		ON_COMMAND(ID_SEISMICDATA_TESTRMSIMPORT, OnSeismicdataTestrmsimport)
		ON_UPDATE_COMMAND_UI(ID_SEISMICDATA_TESTRMSIMPORT, OnUpdateSeismicdataTestrmsimport)
		ON_COMMAND(ID_IMPORT_IMPORTRMSDATA, OnImportRmsData)
		ON_UPDATE_COMMAND_UI(ID_IMPORT_IMPORTRMSDATA, OnUpdateImportImportrmsdata)
		ON_UPDATE_COMMAND_UI(ID_SEISMICDATA_MAPOFSHOTPOINTS, OnUpdateSeismicdataMapofshotpoints)	
		ON_COMMAND(ID_PROCESS_PROCESSRMSVELOCITIES, OnProcessRmsVelocities)
		ON_UPDATE_COMMAND_UI(ID_PROCESS_PROCESSRMSVELOCITIES, OnUpdateProcessProcessrmsvelocities)
		ON_COMMAND(ID_DEPTHCONVERSION_CREATEDEPTHGRIDS, OnDepthconversionCreatedepthgrids)
		ON_UPDATE_COMMAND_UI(ID_DEPTHCONVERSION_CREATEDEPTHGRIDS, OnUpdateDepthconversionCreatedepthgrids)
		ON_COMMAND(ID_IMPORT_FILTERMANAGEMENT, OnImportFiltermanagement)
		ON_UPDATE_COMMAND_UI(ID_IMPORT_FILTERMANAGEMENT, &CVelmanDoc::OnUpdateImportFiltermanagement)
		// velocity volume
		ON_COMMAND(ID_DEPTHCONVERSION_CONVERTSECONDARYTIMEGRID, OnDepthconversionConvertsecondarytimegrid) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_DEPTHCONVERSION_CONVERTSECONDARYTIMEGRID, OnUpdateDepthconversionConvertsecondarytimegrid)
		ON_COMMAND(ID_DEPTHCONVERSION_CONVERTTIMESLICE, OnDepthconversionConverttimeslice)
		ON_UPDATE_COMMAND_UI(ID_DEPTHCONVERSION_CONVERTTIMESLICE, OnUpdateDepthconversionConverttimeslice)
		ON_COMMAND(ID_DEPTHCONVERSION_VELOCITYCUBE_VELOCITYTIMESLICE, OnDepthconversionVelocitycubeVelocitytimeslice)
		ON_UPDATE_COMMAND_UI(ID_DEPTHCONVERSION_VELOCITYCUBE_VELOCITYTIMESLICE, OnUpdateDepthconversionVelocitycubeVelocitytimeslice)
		ON_COMMAND(ID_DEPTHCONVERSION_VELOCITYCUBE_CONVERTSINGLEWELLLOCATIONS, OnDepthconversionVelocitycubeConvertsinglewelllocations) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_DEPTHCONVERSION_VELOCITYCUBE_CONVERTSINGLEWELLLOCATIONS, OnUpdateDepthconversionVelocitycubeConvertsinglewelllocations)
		ON_COMMAND(ID_DEPTHCONVERSION_VELOCITYCUBE_WELLFORECAST, OnDepthconversionVelocitycubeWellforecast)
		ON_UPDATE_COMMAND_UI(ID_DEPTHCONVERSION_VELOCITYCUBE_WELLFORECAST, OnUpdateDepthconversionVelocitycubeWellforecast)
		ON_COMMAND(ID_VELOCITYVOLUME_COMPUTEALLINTERVALVELOCITIES, OnVelocityvolumeComputeallintervalvelocities)
		ON_UPDATE_COMMAND_UI(ID_VELOCITYVOLUME_COMPUTEALLINTERVALVELOCITIES, OnUpdateVelocityvolumeComputeallintervalvelocities)
		ON_COMMAND(ID_VELOCITYVOLUME_IMRCONVERTTIMEGRID, OnVelocityvolumeImrconverttimegrid) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_VELOCITYVOLUME_IMRCONVERTTIMEGRID, OnUpdateVelocityvolumeImrconverttimegrid)
		// depth migration 3d
		ON_COMMAND(ID_RAYTRACE, OnRaytrace) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_RAYTRACE, OnUpdateRaytrace)
		ON_COMMAND(ID_RAYTRACE3D_PROPERTIES, OnRaytrace3dProperties) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_RAYTRACE3D_PROPERTIES, OnUpdateRaytrace3dProperties)	
		//grid tools
		ON_COMMAND(ID_GRIDTOOLS_DIRECTORY, OnGridtoolsDirectory)
		ON_UPDATE_COMMAND_UI(ID_GRIDTOOLS_DIRECTORY, &CVelmanDoc::OnUpdateGridtoolsDirectory)
		ON_COMMAND(ID_GRIDS_EXPORT, OnGridsExport) 
		ON_UPDATE_COMMAND_UI(ID_GRIDS_EXPORT, &CVelmanDoc::OnUpdateGridsExport)
		ON_COMMAND(ID_GRIDS_IMPORT, OnGridsImport)
		ON_UPDATE_COMMAND_UI(ID_GRIDS_IMPORT, &CVelmanDoc::OnUpdateGridsImport)
		ON_COMMAND(ID_GRIDS_DELETE, OnGridsDelete) 
		ON_UPDATE_COMMAND_UI(ID_GRIDS_DELETE, &CVelmanDoc::OnUpdateGridsDelete)
		ON_COMMAND(ID_GRIDS_DUPLICATE, OnGridsDuplicate) 
		ON_UPDATE_COMMAND_UI(ID_GRIDS_DUPLICATE, &CVelmanDoc::OnUpdateGridsDuplicate)
		ON_COMMAND(ID_GRIDS_RENAME, OnGridsRename) 
		ON_UPDATE_COMMAND_UI(ID_GRIDS_RENAME, &CVelmanDoc::OnUpdateGridsRename)
		ON_COMMAND(ID_GRIDTOOLS_DESIGNATE, OnGridtoolsDesignate) 
		ON_UPDATE_COMMAND_UI(ID_GRIDTOOLS_DESIGNATE, &CVelmanDoc::OnUpdateGridtoolsDesignate)
		ON_COMMAND(ID_GRIDTOOLS_STATISTICS, OnGridtoolsStatistics) 
		ON_UPDATE_COMMAND_UI(ID_GRIDTOOLS_STATISTICS, &CVelmanDoc::OnUpdateGridtoolsStatistics)	
		ON_COMMAND(ID_GRIDS_FOURIERTRANSFORM, OnGridsFouriertransform) 
		ON_UPDATE_COMMAND_UI(ID_GRIDS_FOURIERTRANSFORM, &CVelmanDoc::OnUpdateGridsFouriertransform)		
		ON_COMMAND(ID_GRIDS_TIETOWELLDATA, OnGridsTietowelldata) 
		ON_UPDATE_COMMAND_UI(ID_GRIDS_TIETOWELLDATA, OnUpdateGridsTietowelldata)
		ON_COMMAND(ID_GRIDS_SMOOTH, OnGridsFFTSmooth) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDS_SMOOTH, &CVelmanDoc::OnUpdateGridsFFTSmooth)
		ON_COMMAND(ID_GRIDTOOLS_SMOOTH, OnGridsMatrixSmooth)  // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDTOOLS_SMOOTH, &CVelmanDoc::OnUpdateGridsMatrixSmooth)
		ON_COMMAND(ID_GRIDS_PATCH, OnGridsRebuild)  // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDS_PATCH, &CVelmanDoc::OnUpdateGridsRebuild)
		ON_COMMAND(ID_GRIDTOOLS_LIMIT, OnGridtoolsLimit) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDTOOLS_LIMIT, &CVelmanDoc::OnUpdateGridtoolsLimit)
		ON_COMMAND(ID_GRIDS_SPECTRUMANALYSIS, OnGridsSpectrumanalysis)
		ON_UPDATE_COMMAND_UI(ID_GRIDS_SPECTRUMANALYSIS, &CVelmanDoc::OnUpdateGridsSpectrumanalysis)
		ON_COMMAND(ID_GRIDS_CREATECONSTANTGRID, OnGridsCreateconstantgrid) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDS_CREATECONSTANTGRID, &CVelmanDoc::OnUpdateGridsCreateconstantgrid)
		ON_COMMAND(ID_GRIDS_GRIDARITHMETIC, OnGridsGridarithmetic) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDS_GRIDARITHMETIC, &CVelmanDoc::OnUpdateGridsGridarithmetic)
		ON_COMMAND(ID_GRIDS_OVERWRITENULL, OnGridsOverwritenull) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_GRIDS_OVERWRITENULL, &CVelmanDoc::OnUpdateGridsOverwritenull)
		// 3d vis
		ON_COMMAND(ID_PROJECT_3DVIS, OnProject3dvis) // Not in light version.
		ON_UPDATE_COMMAND_UI(ID_PROJECT_3DVIS, &CVelmanDoc::OnUpdateProject3dvis)	
		// demo videos
		ON_COMMAND(ID_HELP_DEMONSTRATIONVIDEOS, OnOpenDemoVideo)		
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_FUNCTIONMODELLING, &CVelmanDoc::OnDemonstrationvideosFunctionmodelling)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_FUNCTIONREFINEMENT, &CVelmanDoc::OnDemonstrationvideosFunctionrefinement)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_FUNCTIONQUALITYCONTROL, &CVelmanDoc::OnDemonstrationvideosFunctionqualitycontrol)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_SAVINGWELLDATA, &CVelmanDoc::OnDemonstrationvideosSavingwelldata)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_GETTINGSTARTEDWITHRMSVELOCITIES, &CVelmanDoc::OnDemonstrationvideosGettingstartedwithrmsvelocities)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_PROCESSVELOCITYDATA, &CVelmanDoc::OnDemonstrationvideosProcessvelocitydata)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_RMSQUALITYCONTROL, &CVelmanDoc::OnDemonstrationvideosRmsqualitycontrol)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_IMPORTINGSEGYDATA, &CVelmanDoc::OnDemonstrationvideosImportingsegydata)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_SAVINGGRIDS, &CVelmanDoc::OnDemonstrationvideosSavinggrids)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_WELLDATAFORMAT, &CVelmanDoc::OnDemonstrationvideosWelldataformat)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_IMRII, &CVelmanDoc::OnDemonstrationvideosImrii)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_3DVISUALISATION, &CVelmanDoc::OnDemonstrationvideos3dvisualisation)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_DEPTHMIGRATION, &CVelmanDoc::OnDemonstrationvideosDepthmigration)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_DEPTHCONVERTSECONDARYGRID, &CVelmanDoc::OnDemonstrationvideosDepthconvertsecondarygrid)
		ON_COMMAND(ID_DEMONSTRATIONVIDEOS_FUNCTIONRESIDUALCONTROLS, &CVelmanDoc::OnDemonstrationvideosFunctionresidualcontrols)		
		//}}AFX_MSG_MAP		
	END_MESSAGE_MAP()
#undef theDerivedClass

	/////////////////////////////////////////////////////////////////////////////
	// CVelmanDoc construction/destruction
	// !!! Note the code on fitting function in the constructor of CVelmanDoc !!!

	CVelmanDoc::CVelmanDoc()
	{
		ModelViewOpen=FALSE;
		ListOfModelsOpen=FALSE;
		TimegridContourOpen=FALSE;
		TyingTableOpen=FALSE;
		SeisdataMapOpen=FALSE;
		VMLightMessage = "This feature is disabled in this version of Velocitymanager.";

		wellpointlist.Init(this);
		zimscube.Init(this);
		faultcube.Init(this);
		horizonlist.SetPrimaryDatabase();
		secondaryhorizonlist.SetSecondaryDatabase();

		// the well model function library
		// do not simply change the order here!!! We store the actual model chosen as
		// a simple number which is the index in the list of functions!!!

		// Furthermore, there is the CMeasurePt member function ReadValuePair()
		// that also relies on the order given here! (as do comparisons, see "whatmodel==4" in this file
		well_lib1=new CWellModelFunction("Instantaneous vel. ~ depth",
			"noexpl",
			"Coeff. Vo", "Coeff. K",
			&ExpRegress, InstVelPropDepth, &ExpFunc, NULL, FALSE);

		well_lib2=new CWellModelFunction("Isochore ~ Isochronochore",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IsoChorPropIsoChron, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib3=new CWellModelFunction("Interval velocity ~ mid depth",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropMDepth, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib4=new CWellModelFunction("Interval velocity ~ mid time",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropMTime, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib5=new CWellModelFunction("Interval velocity ~ top time",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropTTime, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib6=new CWellModelFunction("Interval velocity ~ bottom time",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropBTime, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib7=new CWellModelFunction("Interval velocity ~ top depth",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropTDepth, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib8=new CWellModelFunction("Interval velocity ~ base depth",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropBDepth, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib9=new CWellModelFunction("Interval velocity ~ isochore",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropIsochor, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib10=new CWellModelFunction("Interval velocity ~ isochronochore",
			"noexpl",
			"Slope", "Intercept",
			&LinearRegress, IntVelPropIsochron, &LinearFunc, &InverseLinearFunc, TRUE);

		well_lib11=new CWellModelFunction("Contoured interval velocities",
			"noexpl",
			"n/a", "n/a",
			&FakeRegress, IntVelContoured, &FakeFunc, NULL, TRUE);

		num_model_funcs=11;
		well_model_lib=new ( CWellModelFunction (*[11]) );
		well_model_lib[0]=well_lib1;
		well_model_lib[1]=well_lib2;
		well_model_lib[2]=well_lib3;
		well_model_lib[3]=well_lib4;
		well_model_lib[4]=well_lib5;
		well_model_lib[5]=well_lib6;
		well_model_lib[6]=well_lib7;
		well_model_lib[7]=well_lib8;
		well_model_lib[8]=well_lib9;
		well_model_lib[9]=well_lib10;
		well_model_lib[10]=well_lib11;

		// read RMS time slice interval from INI file. For more info, see ZimsCube::DoDix
		CString buf=AfxGetApp()->GetProfileString("ArithmeticParams", "RMSalphaValue", "200.0");
		RMSTimeSliceInterval=atof((const char *)buf);

		m_pProgress=NULL;
	}

	CVelmanDoc::~CVelmanDoc()
	{
		delete well_lib1;
		delete well_lib2;
		delete well_lib3;
		delete well_lib4;
		delete well_lib5;
		delete well_lib6;
		delete well_lib7;
		delete well_lib8;
		delete well_lib9;
		delete well_lib10;
		delete well_lib11;
		delete well_model_lib;
	}

	BOOL CVelmanDoc::OnNewDocument()
	{
		CNewProjStep1 step1dlg;
		CNewProjStep2 step2dlg;
		CProjectScopeDlg scopedlg;
		/* Delete all old swapfiles */
		CStringArray swapfile;
		long hfile;
		struct _finddata_t c_file;
		char buffer[512];

		CString s;
		int i, j, buflen;

		/*
		If we have a demo version that does not allow for the creation of new
		projects, warn user and exit
		*/
#ifdef DEMO_VERSION
		AfxMessageBox(
			"Demonstration Version\n\n"
			"This is only a demo version of the VelocityManager package.\n\n"
			"You cannot create new projects with this version.\n\n"
			"To obtain the complete package, please contact your vendor",
			MB_ICONSTOP | MB_OK);
		HaveValidDocument=FALSE;
		return FALSE;
#endif

		// initialize base class internally
		HaveValidDocument=FALSE;
		ProjState=0;

		seismicdata.Init(this);

		if (!CDocument::OnNewDocument())
			return FALSE;

		// step 1: define project directory etc.
		if(step1dlg.DoModal()==IDCANCEL)
			return FALSE;
		Title=step1dlg.m_ProjTitle;
		TargetDir=step1dlg.TargetDir;
		ExportDir = TargetDir + "VM_exported_data\\";
		BaseName=step1dlg.m_ProjTitle;
		LogType=(WORD)step1dlg.m_LogType;
		Units=(step1dlg.m_Unitchoice==0 ? 0 : UNIT_OUTPUTINFEET);

		/* check if a project file exists in this directory */
		strncpy(buffer, TargetDir+"*.prj", 512);
		if((hfile=_findfirst(buffer, &c_file )) != -1L)
		{
			if(Error("There is already a project, " +CString(c_file.name)+
				", in this directory.\nThis project will be overwritten. "
				"Do you want to continue?", MSG_WARN|MSG_CANCEL))
				return(FALSE);
		}

		// step 2: define ZIMS directory
		if(_access(TargetDir, 0)==0)
			step2dlg.ZimsDir=TargetDir;
		else
#ifdef VELMAN_UNIX
			step2dlg.ZimsDir="./";
#else
			step2dlg.ZimsDir="C:\\";
#endif

		// different default grid file types on different platforms
#ifdef VELMAN_UNIX
		GridFileType=AfxGetApp()->GetProfileInt("Grid Import", "GridFileType", 2); // MFD
#else
		GridFileType=AfxGetApp()->GetProfileInt("Grid Import", "GridFileType", 1); // Zims
#endif

		step2dlg.pDoc=this;
		if(step2dlg.DoModal()==IDCANCEL)
			return FALSE;

		ZimsDir=step2dlg.ZimsDir; // Set the Zims directory

#ifdef VELMAN_UNIX
		strcpy(lpTempPath, "/var/tmp");
#else
		buflen = 4096;
		GetTempPath(buflen, lpTempPath); // get path for temporary files
#endif

		/* find first grid file in current directory with *.swp */
		strncpy(buffer, ZimsDir+"*.swp", 512);
		if((hfile=_findfirst(buffer, &c_file ))!=-1L)
		{
			swapfile.Add(c_file.name);
			/* Find the rest of the files */
			while( _findnext(hfile, &c_file ) == 0)
				swapfile.Add(c_file.name);
		}

		/* Delete all swapfiles */
		for(i=0;i<swapfile.GetSize();i++)
			_unlink(ZimsDir+swapfile[i]);

		// Do the same for swapfiles in the temporary directory
		strncpy(buffer, lpTempPath, 512);
		strcat(buffer, "*.swp");
		if ((hfile=_findfirst(buffer, &c_file))!=-1L)
		{
			swapfile.Add(c_file.name);
			while(_findnext(hfile, &c_file) == 0)
				swapfile.Add(c_file.name);
		}
		for ( i=0;i<swapfile.GetSize();i++)
			_unlink(lpTempPath+swapfile[i]);

		// step 3: project scope
		scopedlg.m_PrjScope=0;
		do
		{
			if(scopedlg.DoModal()==IDCANCEL)
				return FALSE;
			ProjectScope=scopedlg.m_PrjScope+1;
		}while(ProjectScope==0 && AfxMessageBox("Please choose another scope -- this one is not supported yet."));

#ifdef VELMAN_UNIX
		if(TargetDir.Right(1)!="/")
			TargetDir+="/";
#else
		if(TargetDir.Right(1)!="\\")
			TargetDir+="\\";
#endif

		// create log file and talk a bit about the project in it
		if(!m_logfile.Open((const char *)(TargetDir+BaseName+".log"), CFile::modeCreate | CFile::modeReadWrite | CFile::typeText ))
		{
			AfxMessageBox("Could not open log file. Check you have permission to access this directory.\n\nProject creation aborted.", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
		// add title line and underline it
		AfxFormatString1(s, LOG_NEWPROJ, Title);
		j=s.GetLength()-1; // AfxForm appends a newline automatically, do not count it
		for(i=0;i<j;i++)
			s+='=';
		s+="\n";
		Log(s, LOG_BRIEF);

		s="Log file detail is ";
		switch(LogType)
		{
		case 0:s+="\"Brief\" (low detail)";break;
		case 1:s+="\"Standard\" (medium detail)";break;
		case 2:s+="\"Detailed\" (high detail)";break;
		}
		Log(s, LOG_BRIEF);

		CTime t = CTime::GetCurrentTime();
		AfxFormatString2(s, LOG_NEWPROJ2, t.Format( "%A, %B %d, %Y" ), TargetDir);
		Log(s, LOG_STD);

		s="The scope of the project is to compute ";
		s+=(ProjectScope==SCOPE_VELOCITIES ? "velocity models only" : "depths.");
		Log(s, LOG_STD);
		Log("Input data required:", LOG_STD);
		if(ProjectScope!=SCOPE_VELOCITIES)
			Log(" - Time grid files", LOG_STD);
		if(ProjectScope==SCOPE_TIMEWELL || ProjectScope==SCOPE_TIMEWELLRMS)
			Log(" - Well time/depth and survey data", LOG_STD);
		if(ProjectScope==SCOPE_TIMERMS || ProjectScope==SCOPE_TIMEWELLRMS)
			Log(" - RMS seismic data", LOG_STD);

		s="Depths are given in "+CString((Units & UNIT_OUTPUTINFEET) ? "feet":"metres");
		s+=", velocities are given in "+CString((Units & UNIT_OUTPUTINFEET) ? "ft":"m");
		s+="/sec.";
		Log(s, LOG_STD);

		// set the name of the document file to be saved/opened by the application framework
		ProjectName=TargetDir+BaseName+".prj";

		OnSaveDocument(ProjectName);

		SetPathName((const char *)(ProjectName), TRUE);

		return (HaveValidDocument=TRUE);
	}

	void CVelmanDoc::OnCloseDocument()
	{
		// cannot close if "fine tune" or other modelling dialog is open
		if(ModelViewOpen)
			return;
		CTime t = CTime::GetCurrentTime();
		CString s;
		AfxFormatString1(s, LOG_CLOSEPROJ, t.Format( "%H:%M on %A, %B %d, %Y" ));
		Log(s, LOG_DETAIL);

		if(HaveValidDocument)
		{
			OnSaveDocument(ProjectName);
			OnProjectSaveprojectreport();
		}

		if(m_logfile.m_hFile!=CFile::hFileNull)
		{
			m_logfile.Close();
		}
		CDocument::OnCloseDocument();
	}

	BOOL CVelmanDoc::OnOpenDocument(const char *path)
	{
		BOOL result;
		HaveValidDocument=FALSE;
		char msg[1024];
		CString c_path;
		int buflen;

		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return FALSE;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Loading project");
		m_pProgress->ChangeGeneralDescription("Please wait while the internal database is opened.");
		m_pProgress->ChangeText2("");
		m_pProgress->InitializePercentage();

		seismicdata.Init(this);

		result=CDocument::OnOpenDocument(path);
		if(!result)
		{
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
			return FALSE;
		}

		//set export dir, it is not stored
		ExportDir = TargetDir + "VM_exported_data\\";

		// check to see if project directory has moved. If it has, offer the user the
		// choice of updating the project file.
		c_path = path;
#ifdef VELMAN_UNIX
		int id = c_path.ReverseFind('/');
#else
		int id = c_path.ReverseFind('\\');
#endif

		CString a = TargetDir.Left(id);
		CString b = c_path.Left(id);
		if ( _stricmp((LPCTSTR)a, (LPCTSTR)b) )
		{
			int answer;

			sprintf( msg,
				"The project file was found in a different directory to which it was created.\n"
				"By clicking on OK you can have the project file updated for the new location\n"
				"and continue loading the project. If you click on Cancel, the project will\n"
				"will not be loaded and you will need to move the project directory to\n"
				"its original location in order to be able to use it.\n\n"
				"Original project directory: %s\n"
				"Current project directory: %s\n",
				a, b );
			answer = AfxMessageBox(msg, MB_OKCANCEL | MB_ICONEXCLAMATION);
			if ( answer == IDCANCEL )
			{
				HaveValidDocument=FALSE;
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;

				return FALSE;
			}

			// now update the project file with the new location
			TargetDir = c_path.Left(id) + "\\";
			ExportDir = TargetDir + "VM_exported_data\\";
			ProjectName = c_path;

			ZimsDir = TargetDir;

			SetPathName((LPCTSTR) c_path ); // sets path of document disk file

			//this doesn't work, comes up with errors when re-saving.
			//OnSaveDocument(c_path); // saves project file
		}

		// change into proper directory
		if ( _chdir((LPCTSTR)TargetDir) == -1 )
		{
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress = NULL;
			AfxMessageBox("Problem: project directory not found! This is probably because\n"
				"the project directory has been moved from its original location.\n"
				"Either move the directory back to its original location or create\n"
				"a link to the new location of the project directory.\n"
				"Could not continue loading the project.",
				MB_OK | MB_ICONEXCLAMATION );
			return FALSE;
		}

		// Delete all old swapfiles
		CStringArray swapfile;
		long hfile;
		struct _finddata_t c_file;
		int i = 0;
		char buffer[512];

#ifdef VELMAN_UNIX
		strcpy(lpTempPath, "/var/tmp");
#else
		buflen = 4096;
		GetTempPath(buflen, lpTempPath); // get path for temporary files
#endif

		// find first grid file in current directory with *.swp
		strncpy(buffer, TargetDir+"*.swp", 512);
		if((hfile=_findfirst(buffer, &c_file ))!=-1L)
		{
			swapfile.Add(c_file.name);
			// Find the rest of the files
			while( _findnext(hfile, &c_file ) == 0)
				swapfile.Add(c_file.name);
		}

		// Delete all swapfiles
		for(i=0;i<swapfile.GetSize();i++)
			_unlink(TargetDir+swapfile[i]);

		/* Do the same for swapfiles in the temporary directory
		* NOTE!!! It is dangerous to do this on the unix system
		* in case multiple users have active projects in /var/tmp
		* in which case we would end up deleting their files!
		* Assume system admin will tidy up our swap files.
		* On a PC we assume it's single user only - which might
		* not be true if it's a windows server.
		* 5/08
		*/
		strncpy(buffer, lpTempPath, 512);
		strcat(buffer, "*.swp");
		if ((hfile=_findfirst(buffer, &c_file))!=-1L)
		{
			swapfile.Add(c_file.name);
			while(_findnext(hfile, &c_file) == 0)
				swapfile.Add(c_file.name);
		}
		for ( i=0;i<swapfile.GetSize();i++)
			_unlink(lpTempPath+swapfile[i]);

		// also open log file and report the continuing work
		result=m_logfile.Open((const char *)(TargetDir+BaseName+".log"),
			CFile::modeReadWrite | CFile::typeText );
		if(!result)
		{
			if(_access((const char *)(TargetDir+BaseName+".log"), 0)!=0)
			{
				int answer;
				answer=
					AfxMessageBox("Problem: The project already exists, but the corresponding "
					"log file could not be found.\n\n"
					"Press OK to start a new log file from scratch. All previously"
					" logged contents will remain lost.\n\n"
					"Press Cancel to abort opening the project.",
					MB_OKCANCEL | MB_ICONEXCLAMATION);
				if(answer==IDCANCEL)
					result=0;
				else
				{
					result=m_logfile.Open((const char *)(TargetDir+BaseName+".log"),
						CFile::modeCreate | CFile::modeReadWrite | CFile::typeText );
					if(result)
						m_logfile.WriteString("...\n"
						"**** ORIGINAL START OF LOGFILE IS MISSING ****\n"
						"...\n");
					else
					{
						sprintf(msg, "It was not possible to create a new log file.\n\n"
							"Possible causes:\n"
							"- You do not have write privileges to the project directory.\n"
							"- The disk is full.\n"
							"- The network connection to the disk drive is down.\n"
							"\nTip: Manually create an empty file named '%s.log', then retry opening"
							" the project.", (const char *)Title);
						AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
					}
				}
			}
			else
			{
				AfxMessageBox("Problem: Could not open the log file.\n\n"
					"Possible causes:\n"
					"- You do not have write privilege to the project directory.\n"
					"- Another user is currently accessing the same project.\n\n"
					"Opening will be aborted.", MB_OK | MB_ICONSTOP);
				result=0;
			}

			if(!result)
			{
				HaveValidDocument=FALSE;
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return FALSE;
			}
		} // end of problem handling for "could not open logfile". File is now open.

		m_logfile.SeekToEnd();
		CTime t = CTime::GetCurrentTime();
		CString s;
		AfxFormatString1(s, LOG_OPENPROJ, t.Format( "%H:%M on %A, %B %d, %Y" ));
		Log(s, LOG_DETAIL);

		m_pProgress->ChangeText("Loading fault data");
		if(!faultcube.Read())
			ProjState &= (~PRJ_HAVEFAULTS);

		m_pProgress->ChangeText("Loading grids");
		if(!zimscube.Read())
			ProjState &= (~PRJ_HAVETIME);

		m_pProgress->ChangeText("Loading seismic data");
		if(!seismicdata.Read())
			ProjState &= (~(PRJ_HAVERMSVEL | PRJ_PROCESSEDVEL));

		m_pProgress->ChangeText("Loading well data");
		if(ProjState & PRJ_HAVEWELLS || ProjState & PRJ_HAVEHORIZONS)
		{
			horizonlist.Init(this);
			horizonlist.Read();
		}

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
		return (HaveValidDocument=result);
	}

	BOOL CVelmanDoc::OnSaveDocument(const char *pathname)
	{
		CWaitCursor wait;

		if(!faultcube.Write())
			ProjState&=(~PRJ_HAVEFAULTS);
		if(!zimscube.Write())
			ProjState&=(~PRJ_HAVETIME);
		if(!seismicdata.Write())
			ProjState&=(~(PRJ_HAVERMSVEL | PRJ_PROCESSEDVEL));
		if(ProjState & PRJ_HAVEWELLS || ProjState & PRJ_HAVEHORIZONS)
			horizonlist.Write();
		return CDocument::OnSaveDocument(pathname);
	}

	/////////////////////////////////////////////////////////////////////////////
	// CVelmanDoc serialization

	void CVelmanDoc::Serialize(CArchive& ar)
	{
		int i, size;
		CString s;

		if (ar.IsStoring())
		{
			ar << ProjState << Title << BaseName << LogType << TargetDir << ZimsDir << WellTiDp << WellSurv;
			ar << ProjectName << Units << ProjectScope << GridFileType;
			size=RMSsource.GetSize();
			ar << size;
			for(i=0;i<size;i++)
			{
				ar << RMSsource[i];
			}
		}
		else
		{
			ar >> ProjState >> Title >> BaseName >> LogType >> TargetDir >> ZimsDir >> WellTiDp >> WellSurv;
			ar >> ProjectName >> Units >> ProjectScope >> GridFileType;
			ar >> size;
			for(i=0;i<size;i++)
			{
				ar >> s;
				RMSsource.Add(s);
			}
			// can override grid file type
			GridFileType=AfxGetApp()->GetProfileInt("Debug", "GridFileType", GridFileType);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// CVelmanDoc diagnostics

#ifdef _DEBUG
	void CVelmanDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}

	void CVelmanDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
#endif //_DEBUG

	// Write message to log
	void CVelmanDoc::Log(CString text, WORD level)
	{
		// Writes CString into the document's logfile. Level is the level of the output,
		// it must be less than or equal to the detail level of the logfile (brief...detailed)

		if(level<=LogType && m_logfile.m_hFile!=CFile::hFileNull)
		{
			m_logfile.WriteString((const char *)(text+"\n"));
			m_logfile.Flush();
		}
	}

	// Alert user to error
	BOOL CVelmanDoc::Error(CString text, int Type)
	{
		// output error/warning message, include it in log file if necessary
		// possible Types:
		// MSG_WARN Warning, include into detailed logs, then continue
		// MSG_ERR Error, always into logfile
		// MSG_CANCEL Set this bit to enable additional "Cancel" button

		// returns TRUE if CANCEL pressed, otherwise FALSE (also if no Cancel button)

		CString Msg;
		int result;

		if(Type & MSG_ERR)
		{
			Msg="Error: "+text;
			Log(Msg, LOG_STD);
		}
		else
		{
			Msg="Warning: "+text;
			Log(Msg, LOG_DETAIL);
		}
		if(Type & MSG_CANCEL)
		{
			result=AfxMessageBox(Msg, MB_OKCANCEL | MB_ICONEXCLAMATION);
			return(result==IDCANCEL);
		}
		else
		{
			AfxMessageBox(Msg, MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// CVelmanDoc commands

	void CVelmanDoc::OnHelpWhatnow()
	{
		CProjGuideDlg dlg;
		CString s;

		s="What you have done so far:\n";
		if(ProjState & PRJ_HAVEHORIZONS)
			s+="- Defined horizon names.\n";
		if(ProjState & PRJ_HAVETIME)
			s+="- Imported time data.\n";
		if(ProjState & PRJ_HAVERMSVEL)
			s+="- Imported RMS velocity data.\n";
		if(ProjState & PRJ_PROCESSEDVEL)
			s+="- Processed RMS velocity data.\n";
		if(ProjState & PRJ_HAVEWELLS)
			s+="- Imported well time/depth and survey data.\n";
		if(ProjState & PRJ_HAVEMODEL)
			s+="- Created time/depth models for all layers using the wells.\n"
			" (They were all automatically depth-converted)\n";
		else
		{
			if(ProjState & PRJ_HAVESOMEMODELS)
				s+="- Created time/depth models for some of the layers.\n";
		}
		if(ProjState & PRJ_HAVEDEPTHCONV)
			s+="- Performed the depth conversion\n";

		s+="\nWhat you should do next:\n";
		if(ProjectScope==SCOPE_TIMERMS && !(ProjState & PRJ_HAVEHORIZONS))
		{
			s+="- Define horizon names using the 'Seismic data' menu.\n";
			goto done;
		}

		if(!(ProjState & PRJ_HAVETIME))
		{
			s+="- Import time data using the 'Seismic data' menu.\n";
			goto done;
		}
		if(!(ProjState & PRJ_HAVERMSVEL) && ProjectScope>1 && !(ProjState & PRJ_PROCESSEDVEL))
		{
			s+="- Import RMS data using the 'Seismic data' menu.\n";
			goto done;
		}
		if(!(ProjState & PRJ_PROCESSEDVEL) && ProjectScope>1)
		{
			s+="- Process RMS data using the 'Seismic data' menu.\n";
			goto done;
		}
		if(!(ProjState & PRJ_HAVEWELLS) && ProjectScope!=SCOPE_TIMERMS)
		{
			s+="- Import well data using the 'Well data' menu.\n";
			goto done;
		}
		if(!(ProjState & PRJ_HAVEMODEL) && ProjectScope==1)
		{
			s+="- Create velocity models for all layers using the 'Well data' menu.\n";
			goto done;
		}
		if((ProjState & PRJ_HAVEMODEL) && ProjectScope==1 ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>1)
		{
			s+="- Depth conversion complete!\n";
			goto done;
		}

		s+="- Do the depth conversion!\n";

done:
		dlg.HelpText=s;
		dlg.DoModal();
	}

	// Saves well data to file
	void CVelmanDoc::OnExportWellData()
	{
		// create progress monitor
		AfxGetApp()->DoWaitCursor(1);
		m_pProgress = new CProgressMonit(this);
		if(m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Exporting project data...");
			m_pProgress->ChangeGeneralDescription(
				"Please wait while an ASCII transcript of the project is created.");
			m_pProgress->InitializePercentage();
			m_pProgress->ChangeText2("");
		}

		// dump well data
		if(ProjState & PRJ_HAVEWELLS)
		{
			m_pProgress->ChangeText("Exporting well data");
			horizonlist.WriteASCII2(GetExportDir());
			m_pProgress->ChangePercentage(10);
		}

		// kill progress monitor
		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
		AfxGetApp()->DoWaitCursor(-1);
	}

	// Import RMS data
	void CVelmanDoc::OnImportRmsData()
	{
		// the dialog box numbering starts at two here for historic reasons
		// dlg1 is the dialog that requests a ZIMS database file describing the position
		// of shotlines and shotpoints

		CRMSImportSuppfileDlg dlg1;
		CRMSImportDlg dlg2;
		CFileDialog dlg3(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT_ALL_FILES);
		CString filterfile, chosenFile;

		if(ProjState & PRJ_HAVERMSVEL)
		{
			if(AfxMessageBox(
				"You have already successfully imported RMS velocity information."
				" Do you want to add further data to it?",
				MB_YESNO | MB_ICONEXCLAMATION)==IDNO)
			{
				if(AfxMessageBox("Do you really want to delete the data imported so far?",
					MB_YESNO | MB_ICONEXCLAMATION)==IDNO)
					return;
				else
				{
					seismicdata.ClearContents();
					RMSsource.RemoveAll();
				}
			}
			ProjState &= (~PRJ_PROCESSEDVEL);
		}
		else
			// Whatever happens - clear the internal data base...
			seismicdata.ClearContents();

		// get filter name and units used
		dlg2.pDoc=this; // so that the dialog class object knows about the document
		dlg2.m_Unit=1-((Units & UNIT_FEET_RMSVELOCITY)==UNIT_FEET_RMSVELOCITY);
		if(dlg2.DoModal()==IDCANCEL)
		{
			// cancel can also mean that some error occured; check the dialog's flag!
			if(dlg2.Error)
			{
#ifdef VELMAN_UNIX
				AfxMessageBox("Could not find any filters.\n\n"
					"Make sure that there is a valid filter description file 'velman.flt' in the"
					" $HOME/velman directory.", MB_OK);
#else
				AfxMessageBox("Could not find any filters.\n\n"
					"Make sure that there is a valid filter description file 'VELMAN.FLT' in your"
					" Documents\\Velman directory.", MB_OK);
#endif
			}
			return;
		}
#ifdef VELMAN_UNIX
		filterfile=GetFilterFileDir()+"/velman.flt";
#else
		filterfile=GetFilterFileDir()+"\\VELMAN.FLT";
#endif
		Units &= (~UNIT_FEET_RMSVELOCITY);
		if(dlg2.m_Unit==0)
			Units |=UNIT_FEET_RMSVELOCITY;
		_chdir((const char *)TargetDir);

		// get RMS file name
#ifndef VELMAN_UNIX
		// if it's NOT Unix then want the satndard windows dialog
		dlg3.m_ofn.lpstrTitle="Import of RMS Data - Step 2/2: Velocity file specification";
		dlg3.m_ofn.Flags&=(~OFN_SHOWHELP);

		// Set initial dir for dialogue.
		dlg3.m_ofn.lpstrInitialDir = (LPCTSTR) TargetDir;
		if(dlg3.DoModal()==IDCANCEL)
			return;
#else
		COpenFileBrowser dlgNew3;

		dlgNew3.setFilePath(TargetDir);
		dlgNew3.title="Import of RMS Data - Step 2/2: Velocity file specification";
		if (dlgNew3.DoModal()==IDOK)
		{
			chosenFile=dlgNew3.m_shFilePath+dlgNew3.m_shFileName;
		}
		else
			return;
#endif

		// if it's a filter for 2d-files (2nd character of type definition ='3'), request
		// grid file that contains info on shotpoint location
		if(dlg2.ChosenType[1]=='3')
		{
			// before we display the dialog, we can check whether that makes sense. If there
			// is no grid file of the required type at all, we have an error.
			FillListOfDOSnames();
			// now create a list of ZIMS grid names from that list of DOS names
			CStringArray zimsnames;
			seismicdata.MakeZimsNameList(zimsnames, ListOfDOSnames, ZIMS_TYPE_LINE);
			if(zimsnames.GetSize()==0)
			{
				AfxMessageBox("ERROR:\nYou have specified a filter for files without coordinate information. "
					"VelocityManager needs a file from the grid database to find information on"
					" the positions of shotpoints and shotlines, but your database does not "
					"contain such a file. Create it, then start the import again.", MB_OK);
				return;
			}
			dlg1.pDoc=this;
			// call dialog. The list of DOS names is still valid, see OnInitDialog!
			if(dlg1.DoModal()!=IDOK)
				return;
		}

		// make modeless dialog to monitor progress
		m_pProgress = new CProgressMonit(this);
		if(m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Importing data...");
			m_pProgress->ChangeGeneralDescription("RMS data is being imported, validated and conditioned.");
			m_pProgress->InitializePercentage();
		}

		// if neccessary, import ZMAP supplementary file first
		if(dlg2.ChosenType[1]=='3')
		{
			m_pProgress->ChangeText("Reading Z-MAP supplementary data");
			m_pProgress->ChangeText2("");
			int index, headernum;
			index = CP3_TYPE_LINE; //sk126 dangerous!!!
			if(!(seismicdata.MakeDOSName(dlg1.ChosenGrid, ListOfDOSnames, index, headernum)))
			{
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return;
			}
			if(!(seismicdata.ReadZimsFile(ListOfDOSnames.GetAt(index), headernum, CSD_TYPE_RMSVEL) &&
				seismicdata.ReadZimsFile(ListOfDOSnames.GetAt(index), headernum, CSD_TYPE_TIME)))
			{
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return;
			}
		}

		m_pProgress->ChangeText("Reading RMS velocity data");
#ifndef VELMAN_UNIX
		if(seismicdata.Read(dlg3.GetPathName(), filterfile, dlg2.ChosenFilter))
		{
			RMSsource.Add(dlg3.GetPathName());
		}
		else
		{
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
			return;
		}
#else
		if(seismicdata.Read(chosenFile, filterfile, dlg2.ChosenFilter))
		{
			RMSsource.Add(chosenFile);
		}

		else
		{
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
			return;
		}
#endif

		// if it is a 2d file, this call will do everything necessary to attach coordinates
		// to every shotpoint (by combining info from the ZIMSgrid read before and
		// by doing a spline interpolation if necessary)

		if(dlg2.ChosenType[1]=='3')
		{
			// interpolate along the lines. Only output warnings once.
			if(!(seismicdata.Interpolate2DShotLines(CSD_TYPE_RMSVEL, TRUE) &&
				seismicdata.Interpolate2DShotLines(CSD_TYPE_TIME, FALSE)))
			{
				// remove progress monitoring window
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return;
			}
		}

		// we have the user input 3 pairs of coordinates in case of a 3d grid without
		// x/y information in the file.
		if(dlg2.ChosenType[1]=='2')
		{
			m_pProgress->ChangeText("Requesting coordinates");
			m_pProgress->ChangeText2("");
			CLinecode2Coord dlg2b1;
			dlg2b1.pDoc = this;

			do
			{
				if(dlg2b1.DoModal()==IDCANCEL)
				{
					m_pProgress->DestroyWindow();
					delete m_pProgress;
					m_pProgress=NULL;
					return;
				}
				// set up a rotation matrix and a shift vector to compute global coords from
				// the local coords used in the file. Those local coordinates are just NUMBERS,
				// counting lines (=y) and shotpoints (=x) from 1 onwards in steps of 1
				// do this dialog as often as FillMatrix() returns an error code.
			}
			while(!seismicdata.FillMatrix(
				dlg2b1.m_lin1, dlg2b1.m_shot1, dlg2b1.m_glbx1, dlg2b1.m_glby1,
				dlg2b1.m_lin2, dlg2b1.m_shot2, dlg2b1.m_glbx2, dlg2b1.m_glby2,
				dlg2b1.m_lin3, dlg2b1.m_shot3, dlg2b1.m_glbx3, dlg2b1.m_glby3));
			seismicdata.Convert2GlobalCoordinates();
		}

		// Flag - we have the RMS data...
		ProjState |= PRJ_HAVERMSVEL;

		// Clean Up data base and write data base to disk
		seismicdata.CleanInternalDataBase();
		m_pProgress->ChangeText("Writing RMS database to disk");
		seismicdata.Write();

		// remove progress monitoring window
		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;

		UpdateAllViews(NULL);
	}

	// Process RMS data
	void CVelmanDoc::OnProcessRmsVelocities()
	{
		if((ProjState & PRJ_HAVERMSVEL) && (ProjState & PRJ_HAVETIME))
		{
			// warn the user			
			//if(AfxMessageBox("Warning: The time taken to process this data will increase significantly with the number of RMS velocity data points.", MB_OKCANCEL) != IDOK)
			//	return;			

			// make modeless dialog to monitor progress
			m_pProgress = new CProgressMonit(this);

			if(m_pProgress && m_pProgress->GetSafeHwnd()==0)
			{
				// Initialize the Progress monitor box
				m_pProgress->Create();
				m_pProgress->SetWindowText("Processing RMS velocity data");
				m_pProgress->ChangeGeneralDescription("RMS data is being processed\r\nThis could take a long time");
				m_pProgress->InitializePercentage();

				// Now check the internal data base and throw out all unwanted shotpoints
				seismicdata.CleanInternalDataBase();

				if(zimscube.DoDixEquation())
				{
					ProjState |= PRJ_PROCESSEDVEL;
					// was successful, we can destroy the internal data base...
					seismicdata.Write();
					seismicdata.ClearMostContents();
				}
				else
				{
					AfxMessageBox("An error occurred while processing the RMS data. You should reimport the RMS data now.");
					seismicdata.Read();
				}

				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
			}
		}
		UpdateAllViews(NULL);
	}

	// Opens dialog for managing the filters present in the filter file
	void CVelmanDoc::OnImportFiltermanagement()
	{
		CFilterManagementDlg dlg;

		dlg.pDoc=this; // so that the dialog class object knows about the document
		dlg.DoModal();
	}

	// Two Helper functions that deal with filter file reading

	// file format: lines in ASCII
	// comment line
	// filter name as the user sees it
	// filter type
	// number of following rows in filter description
	// ... any further number of lines in a format that does not concern us here

#ifdef VELMAN_UNIX
	CString CVelmanDoc::GetNextFiltername(int &error, ifstream *stream, CString *type)
#else
	CString CVelmanDoc::GetNextFiltername(int &error, std::ifstream *stream, CString *type)
#endif
	{
		// return user-orientated name of the next filter in the file
		// stream must point to an OPEN file, the file pointer is moved to the
		// beginning of the next filter description (or the end of the file)
		// error contains error status on return: 0=ok, 1=end of file (nothing returned)
		// type contains the typeline of the next filter, if there is no error

		int i, num;
		char szBuf[256];
		CString filtername;

		stream->getline(szBuf, 254); // read comment line of next filter
		if(stream->fail() || stream->eof())
		{
			error=1;
			return ""; // we appear to be beyond the end of the file
		}

		// no error so far ==> we can relatively safely assume that the format is correct
		// unless we just read an empty line close to the end of the file, so we check EOF
		// once more after reading the "Filter name" line.
		stream->getline(szBuf, 254);
		if(stream->fail() || stream->eof())
		{
			error=1;
			return ""; // we appear to be beyond the end of the file
		}
		filtername=szBuf;
		// now read over the rest of this
		stream->getline(szBuf, 254); // type
		*type=szBuf;
		stream->getline(szBuf, 254); // number of following lines
		num=atoi(szBuf);
		if(num<1)
		{
			error=1;
			return ""; // incorrect file format
		}
		for(i=0;i<num;i++)
			stream->getline(szBuf, 254); // skip following lines
		error=0;
		return filtername;
	}

	// returns the name of the directory in which the file VELMAN.FLT ist stored
	CString CVelmanDoc::GetFilterFileDir()
	{
		CString dir;

#ifdef VELMAN_UNIX
		char szBuf[2048];
		char *variable=getenv("HOME");
		if(variable)
			strcpy(szBuf, variable);
		else
			strcpy(szBuf, "/");
		strcat(szBuf, "/velman");
		dir=szBuf;
#else
		//get directory VELMAN.FLT file should be in
		dir= CMainFrame::GetMyDocsFolder();

		//create path with file name as well
		CString fltpath = dir;
		fltpath.Append("\\VELMAN.FLT");

		//see if file exists already
		bool exists = CMainFrame::FileExists(fltpath);

		//if not then copy it from somewhere else
		if(!exists)
		{
			//see if it is in the windows directory, where it used to be kept
			bool windowsExists = CMainFrame::FileExists("c:\\Windows\\VELMAN.FLT");

			if(windowsExists)//copy from windows folder
			{
				AfxMessageBox("The filter file was in the Windows directory, it will be moved to the following location: " + fltpath, MB_OK | MB_ICONINFORMATION);

				//create the directory
				CreateDirectory(dir, NULL);
				//copy file
				CopyFile("c:\\Windows\\VELMAN.FLT", fltpath, false);
			}
			else//copy from program files folder
			{
				AfxMessageBox("The filter file was not present and will now be copied from the installation folder to the following location: " + fltpath, MB_OK | MB_ICONINFORMATION);

				//get file path (VELMAN.FLT should be next to exe file)

				CString OriginalFltPath = CMainFrame::GetExeFolder();
				OriginalFltPath.Append("\\VELMAN.FLT");

				//create the directory
				CreateDirectory(dir, NULL);
				//copy file
				CopyFile(OriginalFltPath, fltpath, false);
			}
		}

#endif

		return dir;
	}

	void CVelmanDoc::FillDataList(int no, int layer, int whatmodel, int whatweight)
	{
		// fills one of the four internal data lists that are used by the preview windows
		// for model selection. no is the list number to fill (0..3), layer the layer
		// from which to get the data, whatmodel the model according to which the xpos, ypos
		// members should be filled (top depth, mid time etc. etc.)
		// whatweight=0 (linear distance weighting) or 1 (quadratic weighting)
		// this routine does not check whether the lists have already been assigend
		// memory!!! This is done in MainFrame::OnModelCreatemodel(), and we rely on that
		// here.
		// The lists will usually be shorter than the maximum length allowed (max. number
		// of entries is number of wells, that size will have been allocated, but some
		// wells might not have measurepoints going with them for this layer)
		// That's why we set length[no] as well.
		// We also compute fits for the model. This is done because this function here
		// is called once and only once each time a different model is chosen, and thus
		// we only do the fitting once per layer if in the coarse model choice.

		CMeasurePt *measurept;
		CHorizon *horizon = (CHorizon *)horizonlist.GetAt(layer);
		double x, y;
		int wellnumber=horizonlist.wellnames.GetSize();
		// step through all wells
		int l=0;
		for(int i=0;i<wellnumber;i++)
		{
			// get measurement point
			measurept = (CMeasurePt *) horizon->GetAt(i);
			// see if it exists and then write status byte and measurement point
			if(measurept && !(measurept->ReadStatus() & MPT_STATUS_INVALID))
			{
				measurept->ReadValuePair(whatmodel, &x, &y);
				if(x!=MPT_ILLEGAL && y!=MPT_ILLEGAL && measurept->ReadZ1()!=MPT_ILLEGAL
					&& !( layer==0 && x==0 && (whatmodel==4 || whatmodel==6) ) )
				{
					xvals[no][l]=x;
					yvals[no][l]=y;
					z1vals[no][l]=measurept->ReadZ1();
					xpos[no][l]=measurept->ReadX();
					ypos[no][l]=measurept->ReadY();
					databaseindex[no][l]=i;
					l++;
				}
			}
		}
		length[no]=l;
		model[no]=whatmodel;
		weight[no]=whatweight;
		// do fitting
		CWellModelFunction *modfunc=well_model_lib[whatmodel];
		double a, b, chisq;
		BOOL result;
		if(modfunc->fit && length[no]>=2)
		{
			result=modfunc->fit(whatweight, length[no], z1vals[no], xvals[no], yvals[no], NULL, &a, &b, &chisq);
			HaveFit[no]=result;
			FitA[no]=a;
			FitB[no]=b;
			FitChi[no]=chisq;
		}
		else
		{
			HaveFit[no]=FALSE;
			FitA[no]=FIT_UNKNOWN;
			FitB[no]=FIT_UNKNOWN;
			FitChi[no]=FIT_UNKNOWN;
		}
	}

	// Loads the time grids, used after loading well data
	void CVelmanDoc::OnImportTimeData()
	{
		CWellhorzTimegridMapDlg dlg;
		CTieAfterImportDlg dlg2;

		double avg = 0, maxdiff = 0, t1 = 0, t2 = 0;
		int i = 0, j = 0, layer = 0, percent = 0, ii = 0, jj = 0;
		CZimsGrid *lastgrid, *lastbutonegrid, *thisgrid, *nextgrid, *firstgrid;
		CStringArray errortext;
		CObArray TheNewGrids;
		char buf[256], *p;
		BOOL allok = FALSE, AnyFault = FALSE, AnySwap = FALSE;
		CTimeImportErrorDlg errdlg;
		CString s;
		int index = 0, headernum = 0, faultindex = 0, faultheadernum = 0;
		CString gridname = "", faultname = "";
		BOOL gotgrid = FALSE;
		int fault = 0;
		int ***touchingzones;// 1 at every point where i-th grid touches is next
		// neighbour

		// this only works if we already have a number of CHorizon structures!
		if(ProjectScope==2 && !(ProjState & PRJ_HAVEHORIZONS))
		{
			AfxMessageBox("You have to define the names and order of the horizons first.");
			return;
		}

		if(ProjectScope!=2 && !(ProjState & PRJ_HAVEWELLS))
		{
			AfxMessageBox("You have to import well time/depth and survey data first.");
			return;
		}

		if(zimscube.GetSize()>0)
		{
			s="The time data grids have already been imported or created.\n"
				"Redoing this step means overwriting the previously imported data"
				" as well as ALL other grids (e.g. patched or filtered).";
			if(ProjState & PRJ_PROCESSEDVEL)
				s+="\n\nAnd you will also lose the time spent on processing the RMS velocity data.";
			s+="\n\nAre you sure you want to continue?";

			if(AfxMessageBox((const char *)s, MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2)!=IDYES)
				return;

			if(ProjState & PRJ_PROCESSEDVEL)
				seismicdata.Init(this); // kill processed velocities if any

			ProjState&=(~(PRJ_HAVETIME | PRJ_PROCESSEDVEL));
		}

		// jump here if "Retry import" button is pressed later as a reaction to import errors
DoTheImport:

		zimscube.Init(this);
		zimscube.ClearContents();
		TheNewGrids.RemoveAll();

		allok=FALSE;

		dlg.pDoc=this;
		dlg2.m_tieGrids=FALSE; // this is default, esp. if we never open the dialog

		if(dlg.DoModal()==IDOK)
		{
			// ask for tying options if we have a project with well data
			if(ProjectScope!=SCOPE_TIMERMS)
			{
				dlg2.m_tieGrids=TRUE; // if we open the dialog, change this default value
				dlg2.pDoc=this;
				if(dlg2.DoModal()!=IDOK)
					return;
			}

			m_pProgress = new CProgressMonit(this);
			if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
				return;
			m_pProgress->Create();
			m_pProgress->SetWindowText("Importing grids");
			m_pProgress->ChangeGeneralDescription("Please wait time grids are imported.");
			m_pProgress->ChangeText2("");
			m_pProgress->InitializePercentage();

			// the public member ZimsGridNameList contains (in order of horizons getting deeper and deeper)
			// a list of Zims grid names that have been assigend to those horizons. We convert them into DOS
			// names now and read them
			// we also use the public member DOSnames that was created during OnInitDialog.
			// also check whether all grids are in increasing order, reimport/correct them if not
			AnyFault=FALSE;
			AnySwap=FALSE;
			allok=TRUE;

			for(i=0;i<horizonlist.GetSize();i++)
			{
				gridname=dlg.ZimsGridNameList.GetAt(i);
				faultname=dlg.FaultGridNameList.GetAt(i);
				m_pProgress->ChangeText("Grid "+gridname);
				m_pProgress->ChangePercentage(i*100/horizonlist.GetSize());
				index = CP3_TYPE_GRID; //sk126 dangerous
				if ( seismicdata.MakeDOSName(gridname, ListOfDOSnames, index, headernum) )
				{
					faultindex = CP3_TYPE_FAULT; //sk126 dangerous

					// For EarthVision files, we can associate more than 1 fault file per
					// time grid. In this case the faultname will look like this:
					// faultfile1 <- faultfile2 <- faultfile3
					// We need to loop over all these filenames. Most of the time we
					// expect only one.
					// NOTE! Decided to not allow velman to have more than one fault file
					// per grid. I started doing this but then realised that all of the code
					// in velman only assumes there is one fault file per grid. Code like
					// grid assign operator for instance only copies one fault grid. However,
					// in case the work is completed, I've left this code here as it will
					// work fine if there is only one fault file. It works for multiple
					// fault files too.
					do
					{
						if ( faultname == "" )
							p = NULL;
						else
						{
							p=(char *)strstr(faultname, " <- ");
							if ( p != NULL )
								*p = NULL; // terminate first faultfile name
						}

						// Now check the fault file is available and link it to the grid.
						if ( faultname=="" || (seismicdata.MakeDOSName(faultname, ListOfDOSnames, faultindex, faultheadernum)))
						{
							if ( gotgrid = seismicdata.ReadZimsFile((const char *)(ListOfDOSnames.GetAt(index)), headernum, CSD_TYPE_TIME, i))
							{
								Log("Imported time grid "+gridname, LOG_BRIEF);
							}

							// If this happened then the last of the imported grids should be linked
							// to the faultgrid...
							if(gotgrid && faultname!="")
							{
								if ( seismicdata.ReadZimsFile((const char *)(ListOfDOSnames.GetAt(faultindex)),
									faultheadernum, CSD_TYPE_TIME, i))
								{
									Log("Imported fault information " + faultname, LOG_BRIEF);
									// Link last faultdata to last grid
									zimscube[zimscube.GetSize()-1]->Link(faultcube[faultcube.GetSize()-1]);
								}
							}
						}

						if ( p != NULL ) // set faultname to next file if there is one
							faultname = (p+4);
					} while ( p != NULL );
				}

				// get grid we just added
				CZimsGrid *grid = zimscube[zimscube.GetSize()-1];

				// check average value is not below 15,
				// if it is then it is likely the user has loaded data in seconds rather than milliseconds
				if(grid->GetAvg() < AfxGetApp()->GetProfileInt("ArithmeticParams", "SecondsCheckThreshold", 15) && grid->GetAvg() != 0)
				{
					if(AfxMessageBox("Grid appears to be in seconds convert to milliseconds?\r\n\r\n" + grid->GetName(), MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						grid->ConvertToMilliseconds();
						grid->CalcAverage();
					}
				}

				// sk126 -- save the first grid to check others...
				if(i==0)
					firstgrid=zimscube[zimscube.GetSize()-1];
				else
				{
					if(*firstgrid != *(zimscube[zimscube.GetSize()-1]))
					{
						Error("Time Grids have different extents. Please use matching grids\n"
							"Offending grid will be deleted - please reimport.", MSG_ERR);
						zimscube.RemoveAt(zimscube[zimscube.GetSize()-1]);
						m_pProgress->DestroyWindow();
						delete m_pProgress;
						m_pProgress=NULL;
						return;
					}
				}
			}

			int touchingCount =0; // total number of grid nodes we considered as touching eachother
			double maxTouchingDifference = AfxGetApp()->GetProfileInt("ArithmeticParams", "MaxTouchingDifference", 500); // max difference between touching values
			maxTouchingDifference /= 1000; // make it into milliseconds

			// maybe we must find out about the regions where two grids touch
			if(dlg2.m_tieGrids && ProjectScope!=SCOPE_TIMERMS && dlg2.m_how>0)
			{
				touchingzones=new int**[horizonlist.GetSize()-1];

				for(layer=0;layer<horizonlist.GetSize()-1;layer++)
				{
					touchingCount = 0;

					// find this grid and its follower. They always are the latest imported grids
					thisgrid = zimscube.FindHorizon(layer, CSD_TYPE_TIME, 0);
					nextgrid = zimscube.FindHorizon(layer+1, CSD_TYPE_TIME, 0);
					touchingzones[layer]=imatrix(0, thisgrid->GetRows()-1, 0, thisgrid->GetColumns()-1);
					for(i=0;i<thisgrid->GetRows();i++)
					{
						for(j=0;j<thisgrid->GetColumns();j++)
						{
							double a = thisgrid->GetGrid()[i][j];
							double b = nextgrid->GetGrid()[i][j];

							// set initially to not touching
							touchingzones[layer][i][j] = 0;

							// ignore null values
							if(thisgrid->GetZnon(a) || nextgrid->GetZnon(b))
							{								
								continue;
							}

							// get difference between values
							double diff = a - b;							

							// is the difference small enough that we will consider them as touching?							
							if(diff >= -maxTouchingDifference && diff <= maxTouchingDifference)
							{
								// record whether they are touching
								touchingzones[layer][i][j] = 1;

								touchingCount++;
							}
						}
					}

					sprintf(buf, "Number of grid nodes determined as being zero isochronochore in %s: %d", (LPCTSTR) thisgrid->GetName(), touchingCount);
					Log(buf, LOG_STD);
				}				
			}

			// Tie grids if required
			if(dlg2.m_tieGrids)
			{
				m_pProgress->ChangeGeneralDescription("Please wait, time grids are being tied to well data.");
				for(i=0;i<horizonlist.GetSize();i++)
				{
					gridname=dlg.ZimsGridNameList.GetAt(i);
					m_pProgress->ChangeText("Grid "+gridname);
					m_pProgress->ChangePercentage(i*100/horizonlist.GetSize());
					// always is the latest imported grid
					lastgrid = zimscube.FindHorizon(i, CSD_TYPE_TIME,-1);
					lastgrid->TieGrid(dlg2.NormGrad, 1.0*dlg2.NormGradFac);
				}
			}

			// use, then remove the touching zone information
			if(dlg2.m_tieGrids && ProjectScope!=SCOPE_TIMERMS && dlg2.m_how>0)
			{
				m_pProgress->ChangeGeneralDescription("Restoring regions on zero isochronochore.");
				for(layer=0;layer<horizonlist.GetSize()-1;layer++)
				{
					thisgrid = zimscube.FindHorizon(layer, CSD_TYPE_TIME, 0);
					nextgrid = zimscube.FindHorizon(layer+1, CSD_TYPE_TIME, 0);
					for(i=0;i<thisgrid->GetRows();i++)
					{
						for(j=0;j<thisgrid->GetColumns();j++)
						{
							if(touchingzones[layer][i][j])
							{
								switch(dlg2.m_how)
								{
								case 1: // set deeper grid to value of shallower grid
									nextgrid->GetGrid()[i][j] = thisgrid->GetGrid()[i][j];
									break;
								case 2: // set shallower grid to value of deeper grid
									thisgrid->GetGrid()[i][j] = nextgrid->GetGrid()[i][j];
									break;
								case 3: // set both to the average value
									avg= (thisgrid->GetGrid()[i][j] + nextgrid->GetGrid()[i][j]) / 2.0;
									thisgrid->GetGrid()[i][j]=avg;
									nextgrid->GetGrid()[i][j]=avg;
									break;
								}
							}
						}
					}
					free_imatrix(touchingzones[layer], 0, thisgrid->GetRows()-1, 0, thisgrid->GetColumns()-1);
				}
				delete touchingzones;
			}

			// check grids for inconsistencies
			errortext.RemoveAll();
			m_pProgress->ChangeGeneralDescription("Please wait time grids are checked.");
			lastbutonegrid=NULL;
			for(i=0;i<horizonlist.GetSize();i++)
			{
				lastgrid = zimscube.FindHorizon(i, CSD_TYPE_TIME, 0);
				lastgrid->Stamp();
				// Find last but one imported grid also, then compare the two
				if(lastbutonegrid)
				{
					// Check if there is a fault
					fault=(*lastbutonegrid)<(*lastgrid);
					// If there is correct
					if(fault>0)
					{
						CString log;
						AnyFault=TRUE;
						// percentage of bad points; round up so we never say "0%"
						j=lastgrid->GetRows()*lastgrid->GetColumns()-lastgrid->GetZnonVals();
						percent=max(1, (fault*100)/j);
						sprintf(buf, "%d%% of values in \"%s\" larger than in \"%s\".",
							percent, (const char *)lastbutonegrid->GetName(), (const char *)lastgrid->GetName());
						errortext.Add(CString(buf));
						log += CString(buf);
						if(fault*100>j*25)
							AnySwap=TRUE;// suspect two grids were swapped
						// find out what largest inconsistency is
						maxdiff=-1.0;
						for(ii=0;ii<lastgrid->GetRows();ii++)
						{
							for(jj=0;jj<lastgrid->GetColumns();jj++)
							{
								t1=((double **)(*lastbutonegrid))[ii][jj];
								t2=((double **)(*lastgrid))[ii][jj];
								if(!lastbutonegrid->GetZnon(t1) && !lastgrid->GetZnon(t2) && t2<t1)
									maxdiff=max(maxdiff, t1-t2);
							}
						}
						sprintf(buf, " (largest error is %6.1f msec)", maxdiff);
						errortext.Add(CString(buf));
						log += CString(buf);
						Log(log, LOG_BRIEF);
					}
				}
				lastbutonegrid=lastgrid;
				TheNewGrids.Add(lastgrid);
			}
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
			if(AnyFault)
			{
				errdlg.SetTexts(errortext, AnySwap);
				errdlg.DoModal();
				if(errdlg.answer==0)
				{
					zimscube.ClearContents();
					return; // cancel pressed
				}
				if(errdlg.answer==2)
					goto DoTheImport;// retry pressed
				// else (answer=10..13) autocorrect the grid list
				for(i=1;i<TheNewGrids.GetSize();i++)
					((CZimsGrid *)TheNewGrids[i])->AutoCorrect(errdlg.answer-10, (CZimsGrid *)TheNewGrids[i-1]);
			}
		}
		if(allok)
			ProjState |= PRJ_HAVETIME;

		UpdateAllViews(NULL);
	}

	//***********************************************************************
	// Subroutine : FillListOfDOSnames()
	// Arguments : none, The directory to search
	// Use : Clears the internal array ListOfDOSnames, then fills it with
	// the names of the files *.?H in the ZimsDir directory. The
	// ZimsDir variable must be set and must end with a backslash
	// Returns : none
	//***********************************************************************
	void CVelmanDoc::FillListOfDOSnames()
	{
		FillListOfDOSnames(ZimsDir);
	}

	void CVelmanDoc::FillListOfDOSnames(CString Directory)
	{
		struct _finddata_t c_file;
		long hfile;
		char buf[513];

		CString s;

		ListOfDOSnames.RemoveAll();
		if ( GridFileType != 5 )// for all formats apart from EV ones
		{
			switch(GridFileType)
			{
			case 0: // ascii .xls (Excel files)
				s=Directory+"*.xls";
			case 1:
				s=Directory+"*.?h";
				break;
			case 2:
				s=Directory+"*.mfd";
				break;
			case 3:
				s=Directory+"*.sv?";
				break;
			case 4:
				s=Directory+"*.?cps";
				break;
			case 6:
				s=Directory+"*.dat";
				break;
			default:
				Error("Invalid file type whilst searching for files.", MSG_ERR);
			}
			/* find first grid file in current directory */
			strcpy(buf, (const char *)s);
			if((hfile=_findfirst(buf, &c_file ))!=-1L)
			{
				s=c_file.name;
				ListOfDOSnames.Add(s);

				/* Find the rest of the files */
				while( _findnext( hfile, &c_file ) == 0 )
				{
					s=c_file.name;
					ListOfDOSnames.Add(s);
				}
			}

			return;
		}
		else
		{
			// earthvision files:
			// EV files have .2grd & .3grd suffixes for grid files
			// .flt and .nvflt for fault files. So we have to search
			// for files twice unlike the other formats
			s=Directory+"*.?grd";
			strcpy(buf, (const char *)s);
			if ((hfile=_findfirst(buf, &c_file)) != -1L )
			{
				s = c_file.name;
				ListOfDOSnames.Add(s);

				while( _findnext( hfile, &c_file ) == 0 )
				{
					s=c_file.name;
					ListOfDOSnames.Add(s);
				}
			}
			s=Directory+"*.vflt";
			strcpy(buf, (const char *)s);
			if ((hfile=_findfirst(buf, &c_file)) != -1L )
			{
				s = c_file.name;
				ListOfDOSnames.Add(s);

				while( _findnext( hfile, &c_file ) == 0 )
				{
					s=c_file.name;
					ListOfDOSnames.Add(s);
				}
			}
			s = Directory + "*.nvflt";
			strcpy(buf, (const char *)s);
			if ((hfile=_findfirst(buf, &c_file)) != -1L )
			{
				s = c_file.name;
				ListOfDOSnames.Add(s);
				while( _findnext( hfile, &c_file ) == 0 )
				{
					s=c_file.name;
					ListOfDOSnames.Add(s);
				}
			}
			return;
		}
	}

	// Select and read in well data from file
	void CVelmanDoc::OnImportWellData()
	{
		ImpWellHorData a;
		a.pDoc = this;
		if(a.DoModal() != IDOK)
			return;

		// make modeless dialog to monitor progress
		m_pProgress = new CProgressMonit(this);
		if(m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Importing well data...");
			m_pProgress->ChangeGeneralDescription("Please wait while well time/depth and survey data are imported.");
			m_pProgress->ChangeText2(""); // no 2nd line of text; 1st line is set from within well
		}

		// import well time/depth and survey data
		horizonlist.Init(this);

		if((HaveValidDocument=horizonlist.ReadNew(WellTiDp, UsePseudoVelocities)))
			ProjState |= PRJ_HAVEWELLS;

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
		UpdateAllViews(NULL);
	}

	// Removes a well from the projecct
	void CVelmanDoc::OnWelldataDeletewell()
	{
		CChooseWell dlg;
		dlg.pDoc = this;
		dlg.SetDlgTexts("Remove wells", "Select wells to remove from the project");

		if( dlg.DoModal() != IDOK)
			return;

		//remove selected well data
		for(int i = dlg.numgrids-1; i >= 0; i--)
		{
			horizonlist.RemoveWell(dlg.chosengrids[i]);
		}

	}

	// Removes selected grids from project
	void CVelmanDoc::OnGridsDelete()
	{
		CChooseManyGridsDlg dlg;
		CZimsGrid *grid[128];
		int i = 0;
		CString Names;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Remove grids", "Choose grids to be removed:");
		if(dlg.DoModal()==IDOK)
		{
			// we must get hold of the ptrs to the grids to be removed FIRST, because their indices
			// become invalid once we started removing grids.
			for(i=0;i<dlg.numgrids;i++)
			{
				grid[i]=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrids[i]));
				Names += grid[i]->GetName() + "\r\n";
			}

			if(Names != "")
			{
				// ask the user
				if(AfxMessageBox("Are you sure you want to delete these grids?\r\n\r\n" + Names, MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					for(i=0;i< dlg.numgrids ;i++)
					{
						Log("Deleted grid "+grid[i]->GetName(), LOG_STD);
						zimscube.RemoveAt(grid[i]);
					}
				}
			}
		}

	}

	// Export selected grids in choice of formats
	void CVelmanDoc::OnGridsExport()
	{
		CGridExportDlg dlg;
		CZimsGrid *grid;
		int i = 0;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		if(dlg.DoModal()!=IDOK)
			return;

		m_pProgress = new CProgressMonit(this);
		if(m_pProgress && m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Exporting grids");
			m_pProgress->ChangeGeneralDescription("");
			m_pProgress->ChangeText2("being exported");
		}

		//make mfd name
		CString s;
		char buf[256];
		CString num;
		int counter=1;

		s = BaseName + "00";

		while(CMainFrame::FileExists(GetExportDir() + s + ".mfd"))
		{
			sprintf(buf, "%s_(%d)_00", (const char *)BaseName, counter);

			s = (CString)buf;
			counter++;
		}

		MFDName=s;

		for(i=0;i<dlg.numgrids;i++)
		{
			grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrids[i]));
			m_pProgress->ChangeText((const char *)grid->GetName());
			grid->WriteZimsFile(dlg.OutputFileType, GetExportDir());
		}

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;

		//tell user where they went
		CString message = "Grids have been exported to the project directory: " + GetExportDir();
		AfxMessageBox(message, MB_OK | MB_ICONINFORMATION);
	}

	// Performs fourier transform on selected grid
	void CVelmanDoc::OnGridsFouriertransform()
	{
		CChooseManyGridsDlg dlg;
		CFFTDirectionDlg dlg2;
		CZimsGrid *grid, *newgrid;
		int i, dir;
		CString s, Names;
		BOOL HaveShownRebuildMessage=FALSE;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Transform grid into/out of Fourier space", "Choose a grid to be transformed:");
		if(dlg.DoModal()!=IDOK)
			return;
		dlg2.m_direction=0;
		if(dlg2.DoModal()!=IDOK)
			return;
		dir=dlg2.m_direction==0 ? 1 : -1; // direction of FFT

		m_pProgress = new CProgressMonit(this);
		if(m_pProgress && m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Fourier transforming grid data");
			m_pProgress->ChangeGeneralDescription("");
			m_pProgress->ChangeText2("being transformed");
		}
		for(i=0;i<dlg.numgrids;i++)
		{
			grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrids[i]));
			if(grid->GetUnitTimePower()!=0 || grid->GetUnitDistPower()!=0)
			{
				m_pProgress->ChangeText((const char *)grid->GetName());
				newgrid = new CZimsGrid(this);
				*newgrid = *grid;
				if(grid->GetZnonVals()!=0)
				{
					if(!HaveShownRebuildMessage)
					{
						AfxMessageBox("Cannot transform a grid with NULL values; have rebuilt missing values.\nCheck log file for a list of rebuilt grids.",
							MB_ICONINFORMATION | MB_OK);
						HaveShownRebuildMessage=TRUE;
					}
					newgrid->IndexGrid();
					Log("Rebuilt grid "+grid->GetName()+ " prior to transform.", LOG_BRIEF);
				}
				newgrid->FFT2D(dir);
				newgrid->SetUnitTimePower(-grid->GetUnitTimePower());
				newgrid->SetUnitDistPower(-grid->GetUnitDistPower());
				newgrid->SetType();
				s=grid->GetName() + (dir==1 ? "_FF" : "_BF"); // altered extension to filenames
				zimscube.Add(newgrid, s, grid->GetHorizon());
				Names += newgrid->GetName() + "\n";
				Log("Created grid '"+ newgrid->GetName() +"'", LOG_BRIEF);
			}
			else
			{
				s="Cannot Fourier-transform the unitless grid '" + grid->GetName() +"'.";
				Error((const char *)s, MSG_ERR);
			}
		}

		AfxMessageBox("The following grids have been created:\n\n" + Names, MB_OK | MB_ICONINFORMATION);

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
	}

	// Import new grid
	void CVelmanDoc::OnGridsImport()
	{
		CGridImportDlg dlg;
		int index = 0, headernum = 0;
		int faultindex = 0, faultheadernum = 0;
		BOOL gotgrid = FALSE;

		//browse for folder - doesn't work because ZimsDir is referenced throughout
		//CFileDialog filedlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);

		//filedlg.m_ofn.lpstrTitle="Select folder to import grids from";
		//filedlg.m_ofn.Flags&=(~OFN_SHOWHELP);
		//filedlg.m_ofn.lpstrInitialDir = (LPCTSTR) ZimsDir;

		//if(filedlg.DoModal()!=IDOK)
		// return;

		//CString Dir = filedlg.GetPathName();
		//
		////remove file name from path
		//LPSTR s = Dir.GetBuffer(Dir.GetLength());
		//PathRemoveFileSpec(s);
		//Dir = s;
		//Dir.Append("\\");

		FillListOfDOSnames(ZimsDir); // create a list of ZIMS grid names from that list of DOS names
		dlg.pDoc=this;
		if(dlg.DoModal()==IDOK)
		{
			index = CP3_TYPE_GRID; //sk126 dangerous
			if(!(seismicdata.MakeDOSName(dlg.ChosenGrid, ListOfDOSnames, index, headernum)))
				return;
			if(dlg.UseFaults)
			{
				faultindex = CP3_TYPE_FAULT; //sk126 dangerous
				if(!(seismicdata.MakeDOSName(dlg.FaultGrid, ListOfDOSnames, faultindex, faultheadernum)))
					return;
			}
			if(gotgrid = seismicdata.ReadZimsFile((const char *)(ListOfDOSnames.GetAt(index)), headernum, dlg.gridtype))
				Log("Imported grid "+dlg.ChosenGrid, LOG_BRIEF);

			// If this happened then the last of the imported grids should be linked
			// to the faultgrid...
			if(gotgrid && dlg.UseFaults)
			{
				if(seismicdata.ReadZimsFile((const char *)(ListOfDOSnames.GetAt(faultindex)),
					faultheadernum, dlg.gridtype))
				{
					Log("Imported fault information "+dlg.FaultGrid, LOG_BRIEF);
					// Sort the lines in the cube in order of lowest Y0
					faultcube[faultcube.GetSize()-1]->Sort();
					// Link last faultdata to last grid
					zimscube[zimscube.GetSize()-1]->Link(faultcube[faultcube.GetSize()-1]);
				}
			}
		}
	}

	// Fills null values in delected grid
	void CVelmanDoc::OnGridsRebuild()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CChooseManyGridsDlg dlg;
		CZimsGrid *grid, *newgrid;
		CRebuildSettingsDlg dlg2;
		CString Names;
		char buf[256];
		int i = 0;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Rebuild grid", "Choose a grid to be rebuilt:");
		if(dlg.DoModal()==IDOK)
		{
			if(dlg2.DoModal()!=IDOK)
				return;
			CWaitCursor wait;
			if(dlg2.m_doall)
				dlg2.m_number=-1; // rebuild entire grid regardless
			for(i=0;i<dlg.numgrids;i++)
			{
				grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrids[i]));
				newgrid = new CZimsGrid(this);
				*newgrid = *grid;
				newgrid->CleanGrid(dlg2.m_number);
				// Name of new grid: name_R5 for 5 nodes, or name_RA for "all nodes"
				if(dlg2.m_doall)
				{
					sprintf(buf, "%s_RA", (const char *)grid->GetName());
					Log("Rebuilt grid "+grid->GetName()+" entirely", LOG_BRIEF);
				}
				else
				{
					sprintf(buf, "Rebuilt grid %s, interpolating across %d nodes",
						(const char *)grid->GetName(), dlg2.m_number);
					Log(buf, LOG_BRIEF);
					sprintf(buf, "%s_R%d", (const char *)grid->GetName(), dlg2.m_number);
				}

				//add to grid database
				zimscube.Add(newgrid, CString(buf), grid->GetHorizon());

				//record the names
				Names += newgrid->GetName();
				Names += "\n";

				//set them to primary grids if user wanted to
				if(dlg2.SetAsPrimary)
					DesignateGrid(newgrid, newgrid->GetHorizon(), false);
			}
			AfxMessageBox("The following rebuilt grids have been created:\n\n" + Names, MB_OK);
		}
	}

	void CVelmanDoc::OnWelldataInterpolatefaultnullpolygons()
	{
		OnGridsRebuild();
	}

	// Rename a grid
	void CVelmanDoc::OnGridsRename()
	{
		CChooseAnyGrid dlg;
		CRenameGridDlg rendlg;
		char buf[255];

		CZimsGrid *grid;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Rename grid", "Choose the grid you wish to rename:");

		if(dlg.DoModal()!=IDOK)
			return;

		grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));
		sprintf(buf, "Rename grid '%s'", (LPCTSTR) grid->GetName());
		rendlg.oldname=buf;
		rendlg.m_gridname=grid->GetName();

		UBYTE oldtype=grid->GetType();

		rendlg.m_listType=(oldtype==CSD_TYPE_TIME ? 0 : (oldtype==CSD_TYPE_INTVEL ? 1 :
			(oldtype==CSD_TYPE_DEPTH ? 2 : 3 )) );
		rendlg.DoModal();

		if(rendlg.m_gridname!="")
		{
			// Check if name has changed
			if(rendlg.m_gridname != grid->GetName())
			{
				Log("Renamed '"+grid->GetName()+"' to '"+rendlg.m_gridname+"'", LOG_STD);
				grid->SetName(rendlg.m_gridname);
			}

			// Decode type
			UBYTE newtype;
			switch(rendlg.m_listType)
			{
			case 0:
				newtype = CSD_TYPE_TIME;
				break;
			case 1:
				newtype = CSD_TYPE_INTVEL;
				break;
			case 2:
				newtype = CSD_TYPE_DEPTH;
				break;
			case 3:
				newtype = CSD_TYPE_OTHER;
				break;
			}

			// Check if type has changed
			if(newtype != oldtype)
			{
				Log("Changed type of '"+grid->GetName(), LOG_STD);
				grid->SetType(newtype);
				grid->SetUnit();
			}
		}

	}

	// Create a new grid with a constant value
	void CVelmanDoc::OnGridsCreateconstantgrid()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CCreateConstantGridDlg dlg;
		CZimsGrid *grid, *newgrid;
		char buf[256];

		if(dlg.DoModal()!=IDOK)
			return;

		// Decode type
		UBYTE newtype = CSD_TYPE_DEPTH;
		switch(dlg.m_listType)
		{
		case 0:
			newtype = CSD_TYPE_TIME;
			break;
		case 1:
			newtype = CSD_TYPE_INTVEL;
			break;
		case 2:
			newtype = CSD_TYPE_DEPTH;
			break;
		case 3:
			newtype = CSD_TYPE_OTHER;
			break;
		}

		grid=zimscube.FindHorizon(0, CSD_TYPE_TIME, 0); // find time grid for first horizon as template
		if(grid==NULL)
			Error("You have to import a time grid for the first horizon that can be used as"
			" a template for this operation (defining height and width of the newly created"
			" grid).", MSG_ERR);
		else
		{
			newgrid = new CZimsGrid(this);
			(*newgrid) << (*grid); // set dimensions as in orig. grid
			(*newgrid)=dlg.m_value;
			newgrid->SetHorizon(-1); // assign no horizon
			newgrid->SetType(newtype);
			newgrid->SetUnit();
			sprintf(buf, "constant grid %-1.2f", dlg.m_value);
			zimscube.Add(newgrid, CString(buf),-1);
			Log("Created " + CString(buf), LOG_BRIEF);
		}

	}

	// Opens dialog where user manually defines horizons
	void CVelmanDoc::OnWellshorizonsDefinehorizons()
	{
		CDefineHorizonsDlg dlg;

		if(ProjState & PRJ_HAVEHORIZONS)
		{
			AfxMessageBox("You did already define the horizon names and their order. This step"
				" cannot be redone or undone. If the horizons that you defined are incorrect"
				" you have to start a new project.");
			return;
		}

		dlg.pDoc=this;
		horizonlist.Init(this);
		if(dlg.DoModal()==IDOK)
			ProjState|=PRJ_HAVEHORIZONS;
		UpdateAllViews(NULL);
	}

	// Create a copy of a grid
	void CVelmanDoc::OnGridsDuplicate()
	{
		CChooseAnyGrid dlg;
		CZimsGrid *grid, *newgrid;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Duplicate grid", "Choose a grid to be copied:");
		if(dlg.DoModal()==IDOK)
		{
			grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));
			newgrid = new CZimsGrid(this);
			(*newgrid) = (*grid);
			zimscube.Add(newgrid, grid->GetName() + "_copy", grid->GetHorizon());
			Log("Duplicated grid "+grid->GetName(), LOG_BRIEF);
		}
	}

	// Allows user to replace null values in a grid with a specified value
	void CVelmanDoc::OnGridsOverwritenull()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		COverwriteNULL dlg;
		CZimsGrid *grid;

		dlg.pDoc=this;

		if (dlg.DoModal()==IDOK )
		{
			grid = (CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));
			grid->OverwriteNULL(dlg.m_overwrite_value);
		}
	}

	// User chooses a grid and then can remove parts of the spectrum
	void CVelmanDoc::OnGridsSpectrumanalysis()
	{
		CChooseAnyGrid dlg;
		CSpecAnalysisDlg dlg2;
		CZimsGrid *grid, *newgrid;
		char buf[256];
		CString originalName;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Spectrum analysis", "Choose a grid to analyse:");

		if(dlg.DoModal()==IDOK)
		{
			grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));
			if(grid->IsAllNull())
			{
				AfxMessageBox("Cannot analyse a grid that only consists of NULL values."
					"\n\nYou should delete this grid.");
				return;
			}

			dlg2.grid=grid;
			dlg2.pDoc=this;
			if(dlg2.DoModal()==IDOK)
			{
				// create a quick copy of the grid and operate on that
				newgrid = new CZimsGrid(this);
				(*newgrid) = (*grid);
				originalName=grid->GetName();

#ifdef VELMAN_UNIX
				ofstream nullfile("\dev\null");

				for(int g=0;g<=originalName.GetLength();g++)
					nullfile << "[" << g << "]=" << (char) originalName[g] << ","
					<< (int) originalName[g] << endl << flush;

				nullfile.close();
#endif

				zimscube.Add(newgrid, originalName+"_clip", grid->GetHorizon());
				grid=newgrid;

				double first=dlg2.gmin+dlg2.slotsize*dlg2.first;
				double last=dlg2.gmin+dlg2.slotsize*dlg2.last;

				grid->ApplyCutoff(first, last, dlg2.UseMinMax);

				if(dlg2.DoRebuild)
					grid->CleanGrid(AfxGetApp()->GetProfileInt("ArithmeticParams", "MaxSpectCleanNodes", 10));

				grid->CalcAverage();

				if(dlg2.KnowUnit)
					sprintf(buf, "Narrowed spectrum of grid %s to [%5.2f%s..%5.2f%s]",
					(const char *)originalName, first, dlg2.UnitName, last, dlg2.UnitName);
				else
					sprintf(buf, "Narrowed spectrum of grid %s to [%5.2f..%5.2f]",
					(const char *)originalName, first, last);

				if(dlg2.SetAsPrimary)
					DesignateGrid(grid, grid->GetHorizon(), false);

				Log(buf, LOG_BRIEF);

				AfxMessageBox("The following clipped grid has been created:\n\n" + grid->GetName(), MB_OK | MB_ICONINFORMATION);

			}
		}
	}

	// Opens gris arithmatic dialog, where user can perform math operations to an entire grid
	void CVelmanDoc::OnGridsGridarithmetic()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CGridArithmeticDlg dlg;
		CZimsGrid *grid1, *grid2, *newgrid, *constgrid=NULL;
		CString op;
		char buf[128];

		if(zimscube.GetSize()<1)
		{
			AfxMessageBox("No grids in database");
			return;
		}
		dlg.pDoc=this;
		if(dlg.DoModal()==IDOK)
		{
			if(dlg.grid1!=-1)
				grid1=(CZimsGrid *)(zimscube.GetAt(dlg.grid1));
			else
				grid1=NULL;
			if(dlg.grid2!=-1)
				grid2=(CZimsGrid *)(zimscube.GetAt(dlg.grid2));
			else
				grid2=NULL;

			if(!grid1 && !grid2)
			{
				// only two values given but no grid. What dimension should the output grid have?
				AfxMessageBox("You only gave two constant values, but chose no grid for this operation.\n"
					"This means that you want to create a new grid from scratch. Choose 'Create constant"
					" grid for this type of operation.");
				return;
			}

			if(grid1==NULL || grid2==NULL)
			{
				// one of the two operands is a constant; create a grid for it first, using the
				// respective other grid as a template
				constgrid = new CZimsGrid(this);
				if(grid1!=NULL)
				{
					(*constgrid)<<(*grid1);
					(*constgrid)=dlg.val2;
					grid2=constgrid; // second operand no longer zero put poinst to temporary grid
					sprintf(buf, "%7.3f", dlg.val2);
				}
				else
				{
					(*constgrid)<<(*grid2);
					(*constgrid)=dlg.val1;
					grid1=constgrid; // first operand no longer zero put poinst to temporary grid
					sprintf(buf, "%7.3f", dlg.val1);
				}
				op=buf;
				constgrid->SetName(op); // assign number as name of temporary grid
			}
			// now we have two grids in any case
			switch(dlg.operation)
			{
			case 0:
				newgrid=(*grid1)+(*grid2);
				break;
			case 1:
				newgrid=(*grid1)-(*grid2);
				break;
			case 2:
				newgrid=(*grid1)*(*grid2);
				break;
			case 3:
				newgrid=(*grid1)/(*grid2);
				break;
			}
			if(newgrid)
			{
				// operation was successful, add grids to zimscube
				zimscube.Add(newgrid, dlg.resultname,-1);
				Log("Created grid "+dlg.resultname, LOG_BRIEF);
			}
			delete constgrid;
		}
	}

	//***********************************************************************
	// Subroutine : WarnCannotConvertLayer()
	// Use : Checks whether a certain layer can be depth converted,
	// displays error msg if not. User can press OK or CANCEL there.
	// Returns : 0 if layer can be converted
	// 2 if it error not work and CANCEL was pressed, 1: error, but OK pressed
	//***********************************************************************
	int CVelmanDoc::WarnCannotConvertLayer(int layer)
	{
		int WeCan=zimscube.CanDepthConvertLayer(layer);
		CString s;

		if(WeCan!=0)
		{
			s="Cannot depth convert layer '"+((CHorizon *)horizonlist.GetAt(layer))->GetName()+
				"':";
			if(WeCan & 1)
				s+="\nNo time grid found.";
			if(WeCan & 32)
				s+="\nNo interval velocity grid found.";
			if(WeCan & 2)
				s+="\nNo horizon data found.";
			if(WeCan & 4)
				s+="\nNo velocity model for this layer found.";
			if(WeCan & 8)
				s+="\nNo time grid for layer above found (you might have to reimport time grids).";
			if(WeCan & 16)
				s+="\nNo depth grid for layer above found (you might have deleted it).";

			s+="\n\nRefer to the \"What next?\" dialog on how to remedy the situation.";
			if(Error(s, MSG_ERR | MSG_CANCEL))
				return 2; // cancel pressed
			else
				return 1; // ok pressed
		}
		return 0;
	}

	//***********************************************************************
	// Subroutine : DepthConvertLayer()
	// Arguments : layer = layernum
	// Use : depth converts one layer if no well modelling necessary
	// (that depends on the project scope, see also DepthConvertModelLayer)
	// Returns : 0 if it worked (and it added a new grid to the cube),
	// 2 if it did not work and CANCEL was pressed, 1: error, but OK pressed
	//***********************************************************************
	int CVelmanDoc::DepthConvertLayer(int layer)
	{
		int retval;

		if((retval=WarnCannotConvertLayer(layer))!=0)
			return retval; // something went wrong

		// first rebuild the internal data base for this layer
		horizonlist.ProcessHorizon(layer);

		return (zimscube.DepthConvert(ProjectScope, layer) ? 0 : 1);
	}

	// Perform the depth conversion
	void CVelmanDoc::OnDepthconversionCreatedepthgrids()
	{
		int i, num;

		// Dialogue to ask whether we want to keep the int velocity slices
		if(ProjectScope==SCOPE_TIMEWELLRMS)
		{
			// First ask if we want to keep the interval velocity
			// time slices
			if(AfxMessageBox("Save the calibration surfaces, RMS interval velocities to the well interval velocities?", MB_YESNO) == IDYES)
				zimscube.keeptieslices = TRUE;
			else
				zimscube.keeptieslices = FALSE;
		}
		else
			zimscube.keeptieslices = FALSE;

		num=horizonlist.GetSize();
		// Project scope does not ask for depth conversion!
		if(ProjectScope==0)
		{
			AfxMessageBox("This should never have happened. VelMan cannot do velocity modelling only"
				"at the moment.\n\nDepth conversion is impossible anyway.", MB_OK);
			return;
		}

		// in the case of wells with velocity models, conversion has already been done on-the-fly
		// in the background
		if(ProjectScope==SCOPE_TIMEWELL)
		{
			AfxMessageBox("Depth conversion has already been performed!\n\n"
				"Each time you finished modelling the velocity within a layer, that layer was"
				" depth converted on the fly.\n"
				"Please use the 'Grid Tools' menu to export, contour or manipulate the newly created "
				"depth grids.\nOr use the 'Well data' menu if you have not yet modelled every"
				" layer.", MB_OK);
			return;
		}

		// do we already have depth grids?
		// each of these has gridid==0
		if(zimscube.FindHorizon(CSD_TYPE_DEPTH))
		{
			if(AfxMessageBox("There are already some depth grids in the internal database.\n\n"
				"Should they be disposed of (otherwise you might end up with several copies of depth grids"
				" for just one individual layer)?", MB_YESNO)==IDYES)
			{
				for(i=0;i<num;i++)
					zimscube.RemoveAt(zimscube.FindHorizon(i, CSD_TYPE_DEPTH, 0));
			}
		}

		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Depth converting");
		m_pProgress->ChangeGeneralDescription("Please wait while all layers are depth converted.");
		m_pProgress->ChangeText2("");
		m_pProgress->InitializePercentage();
		BOOL DidAllLayers=TRUE;

		for(i=0;i<num;i++)
		{
			if(DepthConvertLayer(i)==2)
			{
				DidAllLayers=FALSE;
				break; // cancel pressed during an error message: leave it
			}
		}

		// Destroy progress dialobue
		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;

		// Output depth conversion message...
		if(DidAllLayers)
		{
			if(ProjectScope==SCOPE_TIMEWELL || ProjectScope == SCOPE_TIMEWELLRMS)
				AfxMessageBox("Tied interval velocity grids to well data\n"
				"and completed depth conversion for all horizons.",
				MB_ICONINFORMATION);
			else
				AfxMessageBox("Completed depth conversion for all horizons.",
				MB_ICONINFORMATION);
		}
		else
		{
			if(ProjectScope==SCOPE_TIMEWELL || ProjectScope == SCOPE_TIMEWELLRMS)
				AfxMessageBox("Tied interval velocity grids to well data\n"
				"and completed depth conversion for some horizons.",
				MB_ICONINFORMATION);
			else
				AfxMessageBox("Completed depth conversion for some horizons.",
				MB_ICONINFORMATION);
		}

		// Update project state
		if(DidAllLayers)
			ProjState |= PRJ_HAVEDEPTHCONV;
		UpdateAllViews(NULL);
	}

	// Calibrate or tie a designated grid to the appropriate well
	// control data using the SurfaSe algorithm.
	void CVelmanDoc::OnGridsTietowelldata()
	{
		CChooseManyGridsDlg dlg;
		CHowToTieDlg howtodlg;

		CZimsGrid *grid;
		int i = 0;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Tie grid", "Choose a grid to be tied to well data:");
		if(dlg.DoModal()==IDOK)
		{
			if(howtodlg.DoModal()!=IDOK)
				return;

			m_pProgress = new CProgressMonit(this);
			if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
				return;

			m_pProgress->Create();
			m_pProgress->SetWindowText("Tying grids");
			m_pProgress->ChangeGeneralDescription("Please wait while correction grids are computed.");
			m_pProgress->ChangeText2("");
			m_pProgress->InitializePercentage();
			for(i=0;i<dlg.numgrids;i++)
			{
				grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrids[i]));
				m_pProgress->ChangeText("Tying grid "+grid->GetName());
				grid->TieGrid(howtodlg.NormGrad, 1.0*howtodlg.NormGradFac);
				m_pProgress->ChangePercentage(((i+1)*100)/dlg.numgrids);
			}
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
		}
	}

	// Shows statistics for a grid selected by user
	void CVelmanDoc::OnGridtoolsStatistics()
	{
		CChooseAnyGrid dlg;
		CGridStatisticsDlg sdlg;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Grid statistics", "Choose a grid to analyse:");
		if(dlg.DoModal()==IDOK)
		{
			sdlg.grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));
			sdlg.DoModal();
		}
	}

	int CVelmanDoc::DepthConvertModelLayerMulti(int layer, BOOL UseResiduals, BOOL TotalDeselection, CZimsGrid *depthgrid, CZimsGrid *corrgrid, BOOL &NegativeIntervalDepth, double &minisochore, double &maxisochore, double &minVint, double &maxVint)
	{
		char buf[4*256];
		double *rawz=NULL;
		double **rawxy=NULL;
		int i=0, j=0, rows=0, cols=0, n=0, size=0;
		CZimsGrid *timegrid, *toptimegrid=NULL, *z1grid=NULL;
		double **t1data, **t2data, **z1data, **z2data, **corrdata;
		double t1=0, t2=0, z1=0, z2=0, deltav=0, a=0, b=0, f=0, xval=0, yval=0;
		double res=0; // only computed for output to log file
		double power=2; // 2 for gridding square roots of factors etc.
		int ipower = AfxGetApp()->GetProfileInt("ArithmeticParams", "ResidualPower", 2); // the integer value read from the INI file
		int rstepsize = AfxGetApp()->GetProfileInt("ArithmeticParams", "WellResampleStep", 2);
		BOOL DeselectThisWell = FALSE; // whether well is deselected or not.

		//-------------------
		// find the horizon, flags, function and depth&time grids to use
		CHorizon *horizon=(CHorizon *)horizonlist.GetAt(layer);
		// function
		CWellModelFunction *modfunc=well_model_lib[horizon->ModelNumber];
		a=horizon->FitA;
		b=horizon->FitB;
		// grids
		timegrid=zimscube.FindHorizon(layer, CSD_TYPE_TIME, 0);
		if(layer>0)
		{
			toptimegrid=zimscube.FindHorizon(layer-1, CSD_TYPE_TIME, 0);
			z1grid=zimscube.FindHorizon(layer-1, CSD_TYPE_DEPTH, 0);
		}
		if(!timegrid)
			return 2;

		// the rest depends on how we wish to treat the residuals: Just grid the
		// residuals between model function and actual value found at well and use them
		// in depth conversion, or compute a factor that relates z2(function) with
		// z2(well) and grid the root of that factor and then square again, with the
		// advatange that this would then always be a positive number and we would
		// never have z2<z1.
		// if "function in z only" was ticked (i.e., all residuals are to be ignored),
		// we still distinguish between the two ways of handling residuals (for ease of
		// coding), but do not acutally compute residuals and rather use dummy values

		// ------------ MULTIPLICATIVE TREATMENT OF RESIDUALS -----------------------

		// create a correction grid in any case

		(*corrgrid) << (*timegrid);
		corrgrid->Link(NULL);// if timegrid has faults going with it, do not
		// use these for the residual grid

		if(ipower<1)
			ipower=2;
		if(ipower>1 && (ipower % 2) )
			ipower=2*(ipower/2); // round down odd numbers
		power=(double)ipower;

		if(UseResiduals)
		{
			Log("Forcing depth residuals to be positive (multiplicative residuals)", LOG_STD);
			//-------------------
			// set up a list of residual factors
			m_pProgress->ChangeText("Computing residuals");
			size=horizon->GetSize();
			CMeasurePt *measurept;
			rawxy=dmatrix(1, size, 1, 2);
			rawz=new double[size+1];
			if(rawz==NULL || rawxy==NULL)
			{
				delete rawz;
				free_dmatrix(rawxy, 1, size, 1, 2);
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return 1;
			}
			Log("R=Residual at well.\nList of residuals: \nWell\tResidual", LOG_BRIEF);

			for(i=0, n=0;i<size;i++)
			{
				measurept = (CMeasurePt *) horizon->GetAt(i);
				if(measurept!=NULL && !(measurept->ReadStatus() & MPT_STATUS_INVALID))
				{
					measurept->ReadValuePair(horizon->ModelNumber, &xval, &yval);
					z1 = measurept->ReadZ1();
					z2 = measurept->ReadDepth();
					// 7/04. Note that the following tests mean that there will not
					// be an entry in the log file for each well.
					if(xval!=MPT_ILLEGAL && yval!=MPT_ILLEGAL && z1 != MPT_ILLEGAL )
					{
						if(horizon->ModelNumber==0)
							res=(yval - z1*exp(xval/(horizon->FitA)) ) -
							(exp(xval/(horizon->FitA))-1.0)/(horizon->FitB);
						else
							res=yval-modfunc->func(xval, horizon->FitA, horizon->FitB);
						sprintf(buf, "%s\t%10.2f", (const char *)(horizonlist.wellnames[i]), res);

						DeselectThisWell = measurept->ReadStatus() & (MPT_STATUS_SCRL_EXCLUDE | MPT_STATUS_USER_EXCLUDE);
						if ( DeselectThisWell )
						{
							if ( TotalDeselection )
								strcat(buf, " | not used: well was excluded.");
							else
								strcat(buf, " | not used: well was deselected.");
						}
						Log(CString(buf), LOG_BRIEF);
						if ( DeselectThisWell && TotalDeselection )
							continue;

						t1=measurept->ReadtTop();
						t2=measurept->ReadTime();
						f=modfunc->depth(z1, t1, t2, a, b, 0.0)-z1;

						// If z2-z1 is -ve then we need to catch this otherwise calc. of
						// rawz may give a -ve number. If abs(f) goes very large this
						// indicates the model depth function (and that varies) may have
						// a divide by a very small number. If abs(f) becomes zero then
						// depth() must be same as z1 so the interval depth is zero.
						if(z2<z1 || fabs(f) > 1.0E20 || fabs(f) == 0.0 )
						{
							NegativeIntervalDepth=TRUE; // output error msg later
							rawz[n+1]=MPT_ILLEGAL;
						}
						else
							rawz[n+1]=pow((z2-z1)/f, 1.0/power);
						rawxy[n+1][1]=measurept->ReadX();
						rawxy[n+1][2]=measurept->ReadY();
						n++;
					}
				}
			}
			Log("", LOG_DETAIL);

			//-------------------
			// grid these to a "correction grid". If there were no residuals at all,
			// use constant 1.0 for correction grid
			m_pProgress->ChangeText("Gridding spatial variations");

			if(n>0)
			{
				corrgrid->Resample(n, rstepsize, rawz, rawxy, horizon->NormalizeGradients, horizon->NormGradFactor);
			}
			else
			{
				if(Error("No well data at all was available to create a residual (correction) grid for"
					" this model. The depth grid will be based entirely on the model, "
					" without tying it to actual well data.\n\n"
					"After depth conversion, those well data points missing in the "
					"well database will be back-interpolated.", MSG_WARN | MSG_CANCEL))
				{
					delete corrgrid;
					delete rawz;
					free_dmatrix(rawxy, 1, size, 1, 2);
					m_pProgress->DestroyWindow();
					delete m_pProgress;
					m_pProgress=NULL;
					return 2;
				}
				(*corrgrid)=1.0;
			}
			delete rawz;
			free_dmatrix(rawxy, 1, size, 1, 2);
		}
		else
		{
			(*corrgrid)=1.0; // do not use residuals => dummy value only
			Log("Depth function in z only - no spatial correction using residuals", LOG_BRIEF);
		}

		corrdata=corrgrid->GetGrid();
		corrgrid->Stamp();

		//-------------------
		// apply model function at every grid node
		m_pProgress->ChangeText("Computing depth");
		(*depthgrid) << (*timegrid);
		depthgrid->SetType(CSD_TYPE_DEPTH);
		rows=timegrid->GetRows();
		cols=timegrid->GetColumns();
		depthgrid->Link(timegrid->GetFaultGrid());

		if(layer>0)
		{
			t1data=toptimegrid->GetGrid();
			toptimegrid->Stamp();
			z1data=z1grid->GetGrid();
			z1grid->Stamp();
		}
		t2data=timegrid->GetGrid();
		timegrid->Stamp();
		z2data=depthgrid->GetGrid();
		depthgrid->Stamp();

		t1=0.0;
		z1=0.0; // these two will not change at all if layer==0
		minisochore=minVint=1.0E30;
		maxisochore=maxVint=-1.0E30;

		for(i=0;i<rows;i++)
		{
			for(j=0;j<cols;j++)
			{
				if(layer>0)
				{
					t1=t1data[i][j];
					z1=z1data[i][j];
					if(toptimegrid->GetZnon(t1) || z1grid->GetZnon(z1))
					{
						z2data[i][j]=depthgrid->GetZnon();
						// Need to blank out znon sections of
						// residual grid when we modify the corrdata below otherwise contour
						// plots go weird. (also see below)
						corrdata[i][j] = depthgrid->GetZnon();
						continue;
					}
				}

				t2=t2data[i][j];
				if(timegrid->GetZnon(t2))
				{
					z2data[i][j]=depthgrid->GetZnon();
					corrdata[i][j] = depthgrid->GetZnon(); // As above
				}
				else
				{
					deltav=corrdata[i][j];
					if(corrgrid->GetZnon(deltav))
						deltav=0.0;
					f=modfunc->depth(z1, t1, t2, a, b, 0.0)-z1;
					z2data[i][j]=z1+pow(deltav, power)*f;
					// Must also set the correction grid to the same
					// function used here.
					corrdata[i][j] = pow(corrdata[i][j], power);
					// did we get a negative interval depth (MUST be a mistake!)
					if(z2data[i][j]<z1)
					{
						z2data[i][j]=depthgrid->GetZnon();
						NegativeIntervalDepth=TRUE;
					}
					else
					{
						// keep track of min/max isochore and vInt
						minisochore=min(minisochore, z2data[i][j]-z1);
						maxisochore=max(maxisochore, z2data[i][j]-z1);
						if(t1!=t2 && z1!=z2data[i][j])
						{
							minVint=min(minVint, (z2data[i][j]-z1)/(t2-t1));
							maxVint=max(maxVint, (z2data[i][j]-z1)/(t2-t1));
						}
					}
				}
			}
		}

		return 0;
	}

	int CVelmanDoc::DepthConvertModelLayerAdd(int layer, BOOL UseResiduals, BOOL TotalDeselection, CZimsGrid *depthgrid, CZimsGrid *corrgrid, BOOL &NegativeIntervalDepth, double &minisochore, double &maxisochore, double &minVint, double &maxVint)
	{
		char buf[4*256];
		double *rawz=NULL;
		double **rawxy=NULL;
		int i=0, j=0, rows=0, cols=0, n=0, size=0;
		CZimsGrid *timegrid, *toptimegrid=NULL, *z1grid=NULL;
		double **t1data, **t2data, **z1data, **z2data, **corrdata;
		double t1=0, t2=0, z1=0, deltav=0, a=0, b=0, xval=0, yval=0;
		int rstepsize = AfxGetApp()->GetProfileInt("ArithmeticParams", "WellResampleStep", 2);
		BOOL DeselectThisWell = FALSE; // whether well is deselected or not.

		//-------------------
		// find the horizon, flags, function and depth&time grids to use
		CHorizon *horizon=(CHorizon *)horizonlist.GetAt(layer);
		// function
		CWellModelFunction *modfunc=well_model_lib[horizon->ModelNumber];
		a=horizon->FitA;
		b=horizon->FitB;
		// grids
		timegrid=zimscube.FindHorizon(layer, CSD_TYPE_TIME, 0);
		if(layer>0)
		{
			toptimegrid=zimscube.FindHorizon(layer-1, CSD_TYPE_TIME, 0);
			z1grid=zimscube.FindHorizon(layer-1, CSD_TYPE_DEPTH, 0);
		}
		if(!timegrid)
			return 2;

		// the rest depends on how we wish to treat the residuals: Just grid the
		// residuals between model function and actual value found at well and use them
		// in depth conversion, or compute a factor that relates z2(function) with
		// z2(well) and grid the root of that factor and then square again, with the
		// advatange that this would then always be a positive number and we would
		// never have z2<z1.
		// if "function in z only" was ticked (i.e., all residuals are to be ignored),
		// we still distinguish between the two ways of handling residuals (for ease of
		// coding), but do not acutally compute residuals and rather use dummy values

		// ---------------- ADDITIVE TREATMENT OF RESIDUALS -------------------------

		// create a correction grid in any case

		(*corrgrid) << (*timegrid);
		corrgrid->Link(NULL);// if timegrid has faults going with it, do not
		// use these for the residual grid
		if(UseResiduals)
		{
			//-------------------
			// set up a list of residuals
			m_pProgress->ChangeText("Computing residuals");
			size=horizon->GetSize();
			CMeasurePt *measurept;
			rawxy=dmatrix(1, size, 1, 2);
			rawz=new double[size+1];
			if(rawz==NULL || rawxy==NULL)
			{
				delete rawz;
				free_dmatrix(rawxy, 1, size, 1, 2);
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return 1;
			}
			Log("R=Residual at well.\nList of residuals:\nWell\tResidual", LOG_BRIEF);
			for(i=0, n=0;i<size;i++)
			{
				measurept = (CMeasurePt *) horizon->GetAt(i);
				if(measurept!=NULL && !(measurept->ReadStatus() & MPT_STATUS_INVALID))
				{
					measurept->ReadValuePair(horizon->ModelNumber, &xval, &yval);
					z1 = measurept->ReadZ1();
					if(xval!=MPT_ILLEGAL && yval!=MPT_ILLEGAL && z1 != MPT_ILLEGAL )
					{
						if(horizon->ModelNumber==0)
							rawz[n+1]=(yval - (measurept->ReadZ1())*exp(xval/(horizon->FitA)) ) -
							(exp(xval/(horizon->FitA))-1.0)/(horizon->FitB);
						else
							rawz[n+1]=yval-modfunc->func(xval, horizon->FitA, horizon->FitB);

						sprintf(buf, "%s\t%10.2f", (const char *)(horizonlist.wellnames[i]), rawz[n+1]);

						DeselectThisWell = measurept->ReadStatus() & (MPT_STATUS_SCRL_EXCLUDE | MPT_STATUS_USER_EXCLUDE);
						if ( DeselectThisWell )
						{
							if ( TotalDeselection )
								strcat(buf, " | not used: well was excluded.");
							else
								strcat(buf, " | not used: well was deselected.");
						}

						Log(CString(buf), LOG_BRIEF);
						if ( DeselectThisWell && TotalDeselection )
							continue;

						rawxy[n+1][1]=measurept->ReadX();
						rawxy[n+1][2]=measurept->ReadY();
						n++;
					}
				}
			}
			Log("", LOG_DETAIL);

			//-------------------
			// grid these to a "correction grid". If there were no residuals at all,
			// do not grid them but set correction grid to all 0 and issue warning
			m_pProgress->ChangeText("Gridding spatial variations");

			if(n>0)
			{				
				corrgrid->Resample(n, rstepsize, rawz, rawxy, horizon->NormalizeGradients,
					horizon->NormGradFactor);
			}
			else
			{
				if(Error("No well data at all was available to create a residual (correction) grid for"
					" this model. The depth grid will be based entirely on the model, "
					" without tying it to actual well data.\n\n"
					"After depth conversion, those well data points missing in the "
					"well database will be back-interpolated.", MSG_WARN | MSG_CANCEL))
				{
					delete corrgrid;
					delete rawz;
					free_dmatrix(rawxy, 1, size, 1, 2);
					m_pProgress->DestroyWindow();
					delete m_pProgress;
					m_pProgress=NULL;
					return 2;
				}
				(*corrgrid)=0.0;
			}

			delete rawz;
			free_dmatrix(rawxy, 1, size, 1, 2);
		}
		else
		{
			(*corrgrid)=0.0; // do not use residuals => dummy value only
			Log("Depth function in z only - no spatial correction using residuals", LOG_BRIEF);
		}

		corrdata=corrgrid->GetGrid();
		corrgrid->Stamp();

		//-------------------
		// apply model function at every grid node
		m_pProgress->ChangeText("Computing depth");

		(*depthgrid) << (*timegrid);
		depthgrid->SetType(CSD_TYPE_DEPTH);
		rows=timegrid->GetRows();
		cols=timegrid->GetColumns();
		depthgrid->Link(timegrid->GetFaultGrid());

		if(layer>0)
		{
			t1data=toptimegrid->GetGrid();
			toptimegrid->Stamp();
			z1data=z1grid->GetGrid();
			z1grid->Stamp();
		}
		t2data=timegrid->GetGrid();
		timegrid->Stamp();
		z2data=depthgrid->GetGrid();
		depthgrid->Stamp();

		t1=0.0;
		z1=0.0; // these two will not change at all if layer==0
		minisochore=minVint=1.0E30;
		maxisochore=maxVint=-1.0E30;

		for(i=0;i<rows;i++)
		{
			for(j=0;j<cols;j++)
			{
				if(layer>0)
				{
					t1=t1data[i][j];
					z1=z1data[i][j];
					if(toptimegrid->GetZnon(t1) || z1grid->GetZnon(z1))
					{
						z2data[i][j]=depthgrid->GetZnon();
						// Need to blank out znon sections of
						// residual grid when we modify the corrdata below otherwise contour
						// plots go weird. (also see below)
						corrdata[i][j] = depthgrid->GetZnon();
						continue;
					}
				}

				t2=t2data[i][j];
				if(timegrid->GetZnon(t2))
				{
					z2data[i][j]=depthgrid->GetZnon();
					corrdata[i][j] = depthgrid->GetZnon(); // As above
				}
				else
				{
					deltav=corrdata[i][j];
					if(corrgrid->GetZnon(deltav))
						deltav=0.0;
					z2data[i][j]=modfunc->depth(z1, t1, t2, a, b, deltav);
					// did we get a negative interval depth (MUST be a mistake!)
					if(z2data[i][j]<z1)
					{
						z2data[i][j]=depthgrid->GetZnon();
						NegativeIntervalDepth=TRUE;
					}
					else
					{
						// keep track of min/max isochore and vInt
						minisochore=min(minisochore, z2data[i][j]-z1);
						maxisochore=max(maxisochore, z2data[i][j]-z1);
						if(t1!=t2 && z1!=z2data[i][j])
						{
							minVint=min(minVint, (z2data[i][j]-z1)/(t2-t1));
							maxVint=max(maxVint, (z2data[i][j]-z1)/(t2-t1));
						}
					}
				}
			}
		}

		return 0;
	}

	//***********************************************************************
	// Subroutine : DepthConvertModelLayer()
	// Arguments : layer = layernum
	// Use : depth converts one layer if there iswell modelling necessary
	// (that depends on the project scope, see also DepthConvertLayer)
	// Returns : 0 if it worked (and it added a new grid to the cube),
	// 2 if it did not work and CANCEL was pressed, 1: error, but OK pressed
	//***********************************************************************
	int CVelmanDoc::DepthConvertModelLayer(int layer, double &min, double &max)
	{
		int result = 0;
		int retval = 0;
		CZimsGrid *corrgrid, *depthgrid, *timegrid;
		BOOL NegativeIntervalDepth=FALSE;
		double minisochore, maxisochore, minVint, maxVint; // don't actually use these
		BOOL UseResiduals; // use residuals at all?
		BOOL TotalDeselection; // ignore residuals of deselected wells? (ie. exclude deselected wells)

		if((retval=WarnCannotConvertLayer(layer))!=0)
			return retval; // something went wrong

		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return 2;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Depth converting");
		m_pProgress->ChangeGeneralDescription("Please wait while the layer is depth converted.");
		m_pProgress->ChangeText("Initializing");
		m_pProgress->ChangeText2("");

		//-------------------
		// find the horizon, flags, function and depth&time grids to use
		CHorizon *horizon=(CHorizon *)horizonlist.GetAt(layer);
		// flags
		UseResiduals=!(horizon->HowToHandleWellResiduals & WELL_MODEL_NORESIDUALSATALL);
		TotalDeselection=(horizon->HowToHandleWellResiduals & WELL_MODEL_NORESIDUALSIFDESELECTED);

		// grids
		timegrid=zimscube.FindHorizon(layer, CSD_TYPE_TIME, 0);

		if(!timegrid)
			return 2;

		corrgrid=new CZimsGrid(this);
		depthgrid=new CZimsGrid(this);

		// the rest depends on how we wish to treat the residuals: Just grid the
		// residuals between model function and actual value found at well and use them
		// in depth conversion, or compute a factor that relates z2(function) with
		// z2(well) and grid the root of that factor and then square again, with the
		// advatange that this would then always be a positive number and we would
		// never have z2<z1.
		// if "function in z only" was ticked (i.e., all residuals are to be ignored),
		// we still distinguish between the two ways of handling residuals (for ease of
		// coding), but do not acutally compute residuals and rather use dummy values

		// ------------ MULTIPLICATIVE TREATMENT OF RESIDUALS -----------------------
		if((horizon->HowToHandleWellResiduals & WELL_MODEL_MULTIPLICATIVERESIDUALS) && horizon->ModelNumber!=10)
		{
			if(UseResiduals)
			{
				// we need to calculate the residual correction grid seperately

				//calculate with no residuals
				CZimsGrid *depthgridNoResiduals = new CZimsGrid(this);
				result = DepthConvertModelLayerMulti(layer, false, TotalDeselection, depthgridNoResiduals, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);

				//calculate the main grid
				result = DepthConvertModelLayerMulti(layer, true, TotalDeselection, depthgrid, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);

				// residuals correction grid is residuals minus grid without
				corrgrid = (*depthgrid) - (*depthgridNoResiduals);
			}
			else
			{
				result = DepthConvertModelLayerMulti(layer, false, TotalDeselection, depthgrid, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);
			}
		}
		else if((horizon->HowToHandleWellResiduals & WELL_MODEL_ADDITIVERESIDUALS) || !UseResiduals || horizon->ModelNumber==10)
		{// ---------------- ADDITIVE TREATMENT OF RESIDUALS -------------------------
			result = DepthConvertModelLayerAdd(layer, UseResiduals, TotalDeselection, depthgrid, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);
		}
		else // Hybrid
		{
			double **dgrid, **mgrid, **agrid, **corrdata;
			CZimsGrid *multigrid = new CZimsGrid(this);
			CZimsGrid *addgrid = new CZimsGrid(this);

			//calc values with no residuals
			DepthConvertModelLayerAdd(layer, FALSE, TotalDeselection, depthgrid, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);

			//calc multiplicative and additive residual grids
			DepthConvertModelLayerMulti(layer, TRUE, TotalDeselection, multigrid, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);
			DepthConvertModelLayerAdd(layer, TRUE, TotalDeselection, addgrid, corrgrid, NegativeIntervalDepth, minisochore, maxisochore, minVint, maxVint);

			//get their grids
			dgrid = depthgrid->GetGrid();
			mgrid = multigrid->GetGrid();
			agrid = addgrid->GetGrid();
			corrdata = corrgrid->GetGrid();
			corrgrid->Stamp();

			double MminusNR, AminusNR;

			for(int x = 0; x < depthgrid->GetRows(); x++)
			{
				for(int y =0; y < depthgrid->GetColumns(); y++)
				{
					// get difference between residual and no residual grids
					MminusNR = mgrid[x][y] - dgrid[x][y];
					AminusNR = agrid[x][y] - dgrid[x][y];

					//make difference positive (we only care about magnitude
					MminusNR = fabs(MminusNR);
					AminusNR = fabs(AminusNR);

					//keep smaller value in depth grid
					if(MminusNR < AminusNR)
					{
						//record residual, checking if null first
						if(depthgrid->GetZnon(mgrid[x][y]))
							corrdata[x][y] = depthgrid->GetZnon();
						else
							corrdata[x][y] = mgrid[x][y] - dgrid[x][y];
					}
					else
					{
						if(depthgrid->GetZnon(mgrid[x][y]))
							corrdata[x][y] = depthgrid->GetZnon();
						else
							corrdata[x][y] = agrid[x][y] - dgrid[x][y];
					}
				}
			}

			//apply a matrix filter to correction grid
			CZimsGrid *newgrid;
			newgrid = new CZimsGrid(this);
			(*newgrid) = (*corrgrid);

			int smoothLength = AfxGetApp()->GetProfileInt("ArithmeticParams", "OptimizeSmoothLength", 350);
			int minSmoothNodes = AfxGetApp()->GetProfileInt("ArithmeticParams", "MinSmoothNodes", 7);

			newgrid->SmoothMovingWindow2(corrgrid, smoothLength, minSmoothNodes);

			corrgrid = newgrid;
			corrgrid->SetType(CSD_TYPE_DEPTH);

			//apply correction to depth grid
			depthgrid = *depthgrid + *corrgrid;
		}

		// set all information on the depth grid
		depthgrid->SetType(CSD_TYPE_DEPTH);
		depthgrid->SetUnit();
		depthgrid->SetName("D_"+timegrid->GetName());

		// set min/max, so can show user the values
		min = depthgrid->CalcMinVal();
		max = depthgrid->CalcMaxVal();

		// add the grid to the database
		zimscube.Add(depthgrid, "D_"+timegrid->GetName(), layer);
		zimscube.DesignateGrid(depthgrid);

		// tie the depth grid if necessary
		if(AfxGetApp()->GetProfileInt("ArithmeticParams", "DepthTieAfterModelDC", 0))
			depthgrid->TieGrid(horizon->NormalizeGradients, horizon->NormGradFactor, FALSE);

		// keep the correction grid if we actually used residuals and user has indicated they want to keep it
		if(horizon->KeepResidualGridAfterDC && UseResiduals)
		{
			// keep them as interval velocity grids
			corrgrid->SetType(CSD_TYPE_RESIDUAL);
			corrgrid->SetUnit();
			zimscube.Add(corrgrid, "R_"+timegrid->GetName(), layer);
		}
		else
			delete corrgrid;

		// if this was not the bottom layer, we want to set the Z1 and T1 values for the one below us
		if(layer<horizonlist.GetSize()-1)
			horizonlist.ProcessHorizon(layer+1);

		if(NegativeIntervalDepth)
		{
			Error("A depth grid has been created, but some depth grid nodes have been set"
				" to NULL, because the velocity function utilised to"
				" model this interval has proven not to be viable."
				"\n\nIt is recommended an alternative velocity function"
				" be used;please refer to the manual for further information.", MSG_WARN);
		}

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;

		return 0;
	}

	// User can choose a grid to set a primary grid for a given horizon
	void CVelmanDoc::OnGridtoolsDesignate()
	{
		CDesignateGridDlg dlg;
		CZimsGrid *grid;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		if(dlg.DoModal()==IDOK)
		{
			grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));
			DesignateGrid(grid, dlg.newhorizon, true);
		}
	}

	// Sets grid as primary grid of given horizon
	void CVelmanDoc::DesignateGrid(CZimsGrid *grid, int Horizon, bool Message)
	{
		CZimsGrid *oldgrid;
		CString oldname, s;

		oldgrid=zimscube.FindHorizon(Horizon, grid->GetType(), 0);
		if(oldgrid!=NULL)
		{
			oldname=oldgrid->GetName();
			s="Designated grid '"+grid->GetName()+"' for depth conversion, replacing grid '"
				+ oldgrid->GetName()+"'.";
		}
		else
			s="Designated grid '"+grid->GetName()+"' for depth conversion.";
		grid->SetHorizon(Horizon);
		zimscube.DesignateGrid(grid);
		if(Message)
			AfxMessageBox(s, MB_OK);
		Log(s, LOG_BRIEF);
	}

	// Copy all null values from the selected reference grid, left listing,
	// to the selected grid in the right listing, grid to be limited.
	void CVelmanDoc::OnGridtoolsLimit()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CLimitGridDlg dlg;
		CZimsGrid *grid1=NULL, *grid2=NULL, *newgrid;

		if(zimscube.GetSize()<2)
		{
			AfxMessageBox("No two grids in database");
			return;
		}
		dlg.pDoc=this;
		if(dlg.DoModal()==IDOK)
		{
			// retrieve grids and check they are o.k.
			if(dlg.grid1!=-1)
				grid1=(CZimsGrid *)(zimscube.GetAt(dlg.grid1));
			if(dlg.grid2!=-1)
				grid2=(CZimsGrid *)(zimscube.GetAt(dlg.grid2));
			if(grid1==NULL || grid2==NULL)
			{
				AfxMessageBox("Error: You did not select two valid grids.");
				return;
			}
			if(grid1==grid2)
			{
				AfxMessageBox("Error: You selected the same grid twice.");
				return;
			}
			if(*grid1 != *grid2)
			{
				AfxMessageBox("Error: You selected two incompatible grids.");
				return;
			}

			// create a copy of grid2 and limit it with respect to grid1
			newgrid = new CZimsGrid(this);
			(*newgrid) = (*grid2);
			zimscube.Add(newgrid, grid2->GetName() + "_Limited", grid2->GetHorizon());
			newgrid->LimitGrid(grid1);
			Log("Limited grid "+grid2->GetName()+" with respect to grid "+
				grid1->GetName()+", yielding "+newgrid->GetName(), LOG_BRIEF);
		}
	}

	// Applies a fft filter to the selected grid
	void CVelmanDoc::OnGridsFFTSmooth()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CChooseAnyGrid dlg;
		CSmoothGridOptionsDlg smoothdlg;
		char buf[256];
		CString s;
		CZimsGrid *grid, *newgrid, *orig;
		BOOL HadToIndex=FALSE;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Spatial filter", "Choose a grid to be spatially filtered:");
		if(dlg.DoModal()==IDOK)
		{
			orig=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrid));

			// copy the grid so we don't screw up the original
			grid = new CZimsGrid(this);
			(*grid) = (*orig);

			if(grid->IsAllNull())
			{
				AfxMessageBox("Cannot filter a grid that only consists of NULL values."
					"\n\nYou should delete this grid.");
				return;
			}

			smoothdlg.GridToSmooth=grid;
			smoothdlg.pDoc=this;

			if(grid->GetZnonVals()!=0)
			{
				grid->IndexGrid(); // cannot FFT a grid with ZNONs
				HadToIndex=TRUE;
			}

			if(smoothdlg.DoModal()==IDOK)
			{
				newgrid = new CZimsGrid(this);
				*newgrid = *grid;

				if(HadToIndex)
					grid->UnIndexGrid();

				newgrid->FFT2D(1);
				newgrid->LowPassFilter(smoothdlg.filtervalue);
				newgrid->FFT2D(-1);
				newgrid->PatchInSomeOldValues(grid);

				if(HadToIndex)
					newgrid->LimitGrid(grid);

				sprintf(buf, "%s_S%2.1f%", (const char *)orig->GetName(), smoothdlg.filtervalue);

				s=buf;
				zimscube.Add(newgrid, s, grid->GetHorizon());
				Log("Created new grid: "+newgrid->GetName(), LOG_BRIEF);

				AfxMessageBox("The following grid has been created:\n\n" + newgrid->GetName(), MB_OK | MB_ICONINFORMATION);
			}

			delete grid;
		}
	}

	// Apply the matrix filter to the grid user selects
	void CVelmanDoc::OnGridsMatrixSmooth()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CChooseManyGridsDlg dlg;
		CSmoothWndOptDlg smoothdlg;
		char buf[256];
		CString s, s1;
		CZimsGrid *grid, *newgrid, *RemovedGrid;
		int i = 0;
		CString Names;

		dlg.pDoc = this; // so that dialog can get hold of list of grid names
		dlg.SetDlgTexts("Filter grid", "Choose a grid to be filtered:");

		if(dlg.DoModal()!=IDOK)
			return;

		smoothdlg.pDoc = this;

		if(smoothdlg.DoModal()!=IDOK)
			return;

		CWaitCursor wait;
		m_pProgress = new CProgressMonit(this);
		if(m_pProgress && m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Filtering grid");
			m_pProgress->ChangeGeneralDescription("Matrix filtering in progress.");
			m_pProgress->ChangeText("");
			m_pProgress->ChangeText2("");
			m_pProgress->InitializePercentage();
		}
		for(i=0;i<dlg.numgrids;i++)
		{
			grid=(CZimsGrid *)(zimscube.GetAt(dlg.chosengrids[i]));

			if(m_pProgress)
				m_pProgress->ChangeText("Filtering grid "+grid->GetName());

			newgrid = new CZimsGrid(this);
			(*newgrid) = (*grid);

			//smoothen it
			newgrid->SmoothMovingWindow(grid, smoothdlg.windowsize);

			//create names
			sprintf(buf, "%s_Low%d", (const char *)grid->GetName(), smoothdlg.windowsize);
			s=buf;

			//add them to grids
			zimscube.Add(newgrid, s, grid->GetHorizon());

			Names += newgrid->GetName() + "\n";
			Log("Created new grid: " + newgrid->GetName(), LOG_BRIEF);

			//do we want to keep the removed high frequencies?
			if(smoothdlg.KeepHighFrequencies)
			{
				RemovedGrid = (*grid) - (*newgrid);
				sprintf(buf, "%s_High%d", (const char *)grid->GetName(), smoothdlg.windowsize);
				s1=buf;
				zimscube.Add(RemovedGrid, s1, grid->GetHorizon());
				Names += RemovedGrid->GetName() + "\n";
				Log("Created new grid: " + RemovedGrid->GetName(), LOG_BRIEF);
			}

			//do we want to make new grid primary?
			if(smoothdlg.SetAsPrimary)
			{
				//only if source grid was primary
				if(grid->GetID()==0 && grid->GetHorizon()>=0)
				{
					DesignateGrid(newgrid, newgrid->GetHorizon(), false);
				}
			}

			if(m_pProgress)
				m_pProgress->ChangePercentage((100*(i+1))/dlg.numgrids);
		}
		if(m_pProgress)
		{
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
		}

		AfxMessageBox("The following grids have been created:\n\n" + Names, MB_OK | MB_ICONINFORMATION);

	}

	// Shows a directory of grids in project
	void CVelmanDoc::OnGridtoolsDirectory()
	{
		CGridDatabaseListDlg dlg;

		dlg.pDoc=this;
		dlg.DoModal();
	}

	void CVelmanDoc::OnDepthconversionConvertsecondarytimegrid()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		// Perform the actual depth conversion
		CChooseManyGridsDlg dlg;
		CSecondaryDepthConvOptDlg dlg2;
		CZimsGrid *secondarygrid;
		int i = 0;

		dlg.pDoc=this; // so that dialog can get hold of list of grid names
		dlg.OnlyTimeGrids=TRUE;
		dlg.SetDlgTexts("Depth convert time grids", "Choose one or more time grids:");

		if(!(dlg.DoModal()==IDOK) || dlg.numgrids==0)
			return;

		// check all grids; exit if any one is knackered
		for(i=0;i<dlg.numgrids;i++)
		{
			secondarygrid=(CZimsGrid *)zimscube.GetAt(dlg.chosengrids[i]);
			if(WarnCannotConvertSecondaryGrid(secondarygrid))
				return;
		}

		// ask for options on how to do the depth conversion. Only allow cubic splines
		// for 4 or more primary time grids=4 or more horizons
		dlg2.may_apply_cubic=(horizonlist.GetSize()>=4);
		if(!(dlg2.DoModal()==IDOK))
			return;

		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return;

		CWaitCursor wait;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Depth converting");
		m_pProgress->ChangeGeneralDescription("Please wait while grid is depth converted.");
		m_pProgress->ChangeText2("");
		m_pProgress->InitializePercentage();

		zimscube.DepthConvertSecondaryTimegrids(dlg.numgrids, dlg.chosengrids,
			dlg2.interp_method, dlg2.percentage, dlg2.m_rebuild, horizonlist.GetSize());

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
	}

	//***********************************************************************
	// Subroutine : WarnCannotConvertSecondaryGrid()
	// Use : Checks whether a secondary time grid can be depth converted,
	// displays error msg if not. User can press OK or CANCEL there.
	// Returns : 0 if all is well, 1 otherwise
	//***********************************************************************
	int CVelmanDoc::WarnCannotConvertSecondaryGrid(CZimsGrid *grid)
	{
		int WeCan=zimscube.CanDepthConvertSecondaryTimegrid(grid, horizonlist.GetSize());
		CString s;

		if(WeCan!=0)
		{
			if(grid)
				s="Cannot depth convert grid '"+ grid->GetName() +"':";
			else
				s="Cannot perform operation:";
			if(WeCan & 1)
				s+="\nNot all primary time grids found.";
			if(WeCan & 2)
				s+="\nNot all primary depth grids found.";
			if(WeCan & 4)
				s+="\nThis is not a time grid.";
			if(WeCan & 8)
				s+="\nThis is a primary time grid itself.";
			if(WeCan & 16)
				s+="\nThis grid is not compatible in extent to the others.";
			Error(s, MSG_ERR);
			return 1;
		}
		return 0;
	}

	// Opens seperate program to Convert SEGY data to a ascii format we can use
	void CVelmanDoc::OnSeismicdataOpensegyconverter()
	{
#ifdef VELMAN_UNIX
		return;
#endif
		if(!IsNetfx40ClientInstalled())
		{
			AfxMessageBox( "Microsoft .NET Framework 4.0 needs to be installed "
				" before the SEGY converter can be used.", MB_OK | MB_ICONINFORMATION);
			return;
		}

		//launch program to convert segy files
		CMainFrame::ShellExecuteAndWait("SEGY Converter\\SEGYConverter.exe", "\""+ ProjectName + "\"", false);
	}

	void CVelmanDoc::OnSeismicdataTestrmsimport()
	{
		CRMSImportSuppfileDlg dlg1;
		CRMSImportDlg dlg2;
		CString chosenFile;

		CFileDialog dlg3(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT_ALL_FILES);

		CString filterfile;
		CRMSTestImportResultsDlg resdlg;

		// get filter name and units used
		dlg2.pDoc=this; // so that the dialog class object knows about the document
		dlg2.m_Unit=1-((Units & UNIT_FEET_RMSVELOCITY)==UNIT_FEET_RMSVELOCITY);
		if(dlg2.DoModal()==IDCANCEL)
		{
			// cancel can also mean that some error occured; check the dialog's flag!
			if(dlg2.Error)
			{
#ifdef VELMAN_UNIX
				AfxMessageBox("Could not find any filters.\n\n"
					"Make sure that there is a valid filter description file 'velman.flt' in the"
					" $HOME/velman directory.", MB_OK);
#else
				AfxMessageBox("Could not find any filters.\n\n"
					"Make sure that there is a valid filter description file 'VELMAN.FLT' in the"
					" Windows directory.", MB_OK);
#endif
			}
			return;
		}
#ifdef VELMAN_UNIX
		filterfile=GetFilterFileDir()+"/velman.flt";
#else
		filterfile=GetFilterFileDir()+"\\VELMAN.FLT";
#endif
		Units &= (~UNIT_FEET_RMSVELOCITY);
		if(dlg2.m_Unit==0)
			Units |=UNIT_FEET_RMSVELOCITY;

		_chdir((const char *)TargetDir);
		// get RMS file name
#ifndef VELMAN_UNIX
		dlg3.m_ofn.lpstrTitle="Import of RMS Data - Step 2/2: Velocity file specification";
		dlg3.m_ofn.Flags&=(~OFN_SHOWHELP);

		// Set initial dir for dialogue.
		dlg3.m_ofn.lpstrInitialDir = (LPCTSTR)TargetDir;

		if(dlg3.DoModal()==IDCANCEL)
			return;
#else
		// we're using unix
		COpenFileBrowser dlg3_new;
		dlg3_new.setFilePath(TargetDir);
		dlg3_new.title="Import of RMS Data - Step 2/2: Velocity file specification";
		if (dlg3_new.DoModal()==IDOK)
		{
			chosenFile=dlg3_new.m_shFilePath+dlg3_new.m_shFileName;
		}
		else
			return;
#endif

		// make modeless dialog to monitor progress
		m_pProgress = new CProgressMonit(this);
		if(m_pProgress->GetSafeHwnd()==0)
		{
			m_pProgress->Create();
			m_pProgress->SetWindowText("Importing data...");
			m_pProgress->ChangeGeneralDescription(
				"Please wait while a sample of the RMS velocity and time data is imported.");
			m_pProgress->InitializePercentage();
		}

		m_pProgress->ChangeText("Reading RMS velocity data");

#ifndef VELMAN_UNIX
		seismicdata.Read(dlg3.GetPathName(), filterfile, dlg2.ChosenFilter, CString(""), TRUE);
#else
		seismicdata.Read(chosenFile, filterfile, dlg2.ChosenFilter, CString(""), TRUE);
#endif

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;

		resdlg.pDoc=this;
		resdlg.DoModal();
	}

	void CVelmanDoc::OnDepthconversionConverttimeslice()
	{
		// this is pretty much a copy of "DepthConversionConvertsecondarytimegrid"
		// we just create a fake (temporary) constant time grid
		// we make this part of a list of grids, so we can copy even more code from
		// the other function, even if it does not make much sense technically

		// Perform the actual depth conversion
		CDepthConvertTimesliceDlg dlg;
		CSecondaryDepthConvOptDlg dlg2;
		CZimsGrid *grid;

		dlg2.m_rebuild = FALSE;

		grid=zimscube.FindHorizon(0, CSD_TYPE_TIME, 0); // find time grid for first horizon as template
		if(grid==NULL)
		{
			Error("You have to import a time grid for the first horizon that can be used as"
				" a template for this operation (defining height and width of the newly created"
				" depth grid).", MSG_ERR);
			return;
		}

		if(WarnCannotConvertSecondaryGrid(NULL))
			return;

		if(!(dlg.DoModal()==IDOK))
			return;

		// ask for options on how to do the depth conversion. Only allow cubic splines
		// for 4 or more primary time grids=4 or more horizons
		dlg2.may_apply_cubic=(horizonlist.GetSize()>=4);
		if(!(dlg2.DoModal()==IDOK))
			return;

		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return;

		CWaitCursor wait;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Depth converting");
		m_pProgress->ChangeGeneralDescription("Please wait while grid is depth converted.");
		m_pProgress->ChangeText2("");
		m_pProgress->InitializePercentage();

		// temporarily add the constant grid to the zimscube, so it has an index
		// that we can pass to DepthConvertSecondaryTimegrids(). We will remove it
		// later.

		zimscube.DepthConvertSecondaryTimeslice(dlg.value,
			dlg2.interp_method, dlg2.percentage, dlg2.m_rebuild);

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
	}

	void CVelmanDoc::OnDepthconversionVelocitycubeVelocitytimeslice()
	{
		// this is pretty much a copy of "DepthConversionConvertsecondarytimegrid",
		// we just create a fake (temporary) constant time grid
		// we make this part of a list of grids, so we can copy even more code from
		// the other function, even if it does not make much sense technically

		// Perform the actual depth conversion
		CDepthConvertTimesliceDlg dlg;
		CSecondaryDepthConvOptDlg dlg2;
		CZimsGrid *grid;

		grid=zimscube.FindHorizon(0, CSD_TYPE_TIME, 0); // find time grid for first horizon as template
		if(grid==NULL)
		{
			Error("You have to import a time grid for the first horizon that can be used as"
				" a template for this operation (defining height and width of the newly created"
				" depth grid).", MSG_ERR);
			return;
		}

		if(WarnCannotConvertSecondaryGrid(NULL))
			return;

		if(!(dlg.DoModal()==IDOK))
			return;

		// ask for options on how to do the depth conversion. Only allow cubic splines
		// for 4 or more primary time grids=4 or more horizons
		dlg2.may_apply_cubic=(horizonlist.GetSize()>=4);
		dlg2.RebuildDoesNotApply=TRUE;
		if(!(dlg2.DoModal()==IDOK))
			return;

		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return;

		CWaitCursor wait;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Depth converting");
		m_pProgress->ChangeGeneralDescription("Please wait while grid is depth converted.");
		m_pProgress->ChangeText2("");
		m_pProgress->InitializePercentage();

		// temporarily add the constant grid to the zimscube, so it has an index
		// that we can pass to DepthConvertSecondaryTimegrids(). We will remove it
		// later.

		zimscube.VelocityConvertSecondaryTimeslice(dlg.value, dlg2.interp_method, dlg2.percentage);

		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;
	}

	// Displays the log file to user
	void CVelmanDoc::OnProjectViewlogfile()
	{
		CProjectLogDlg dlg;

		ULONGLONG maxsize = 1024*1024;

		// read last maxsize KB of logfile or entire logfile, whatever is smaller
		ULONGLONG pos=m_logfile.GetPosition();

		if(pos<maxsize)
		{
			dlg.SkipFirstLine=FALSE;
			m_logfile.SeekToBegin();
		}
		else
		{
			dlg.SkipFirstLine=TRUE;
			m_logfile.Seek(pos-maxsize, CFile::begin);
		}
		dlg.logfile=&m_logfile;
		dlg.DoModal();
		m_logfile.SeekToEnd();
	}

	// Saves log file as report
	void CVelmanDoc::OnProjectSaveprojectreport()
	{
		CString filename="";
		CStdioFile f;
		CString s;

		filename = GetExportDir() + BaseName + "_report.rtf";

		//open file
		if(!f.Open( (LPCTSTR)filename, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText ))
		{
			Error("Could not open file " + filename, MSG_ERR);
			return;
		}

		m_logfile.SeekToBegin();

		//write report from log
		while(m_logfile.GetPosition() < m_logfile.GetLength())
		{
			m_logfile.ReadString(s);
			f.WriteString(s+"\n");
		};

		f.Close();
		m_logfile.SeekToEnd();

		//AfxMessageBox("Saved report:\n\n" + filename, MB_OK | MB_ICONINFORMATION);
	}

	void CVelmanDoc::OnDepthconversionVelocitycubeConvertsinglewelllocations()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CSecondaryDepthConvOptDlg dlg;
		CTimeConvertDepthDlg dlg2;

		if(WarnCannotConvertSecondaryGrid(NULL))
			return;

		// ask for options on how to do the depth conversion. Only allow cubic splines
		// for 4 or more primary time grids=4 or more horizons
		dlg.may_apply_cubic=(horizonlist.GetSize()>=4);
		dlg.RebuildDoesNotApply=TRUE;
		dlg.TimeConversion=TRUE; // change dialog box heading
		if(!(dlg.DoModal()==IDOK))
			return;

		wellpointlist.interp_method=dlg.interp_method;
		wellpointlist.extrap_gradient_percent=dlg.percentage;
		dlg2.pDoc=this;
		dlg2.DoModal();
	}

	// Opens the well forecast dialog
	void CVelmanDoc::OnDepthconversionVelocitycubeWellforecast()
	{
		CWellForecastDlg dlg;

		dlg.pDoc=this;
		dlg.DoModal();
	}

	// computes interval velocity for given layer
	int CVelmanDoc::ComputeIntervalvelocity(int i, double &min, double &max)
	{
		char buf[256];
		CZimsGrid *timediff; // temporary grid storing (tBtm-tTop)/2000
		CZimsGrid *vInt; // newly created interval velocity grid
		CZimsGrid *topgrid, *bottomgrid; // top/bottom time or depth
		CString layername, gridname;

		// first of all create a temporary grid to contain the denominator
		timediff=new CZimsGrid(this);
		topgrid=zimscube.FindHorizon(0, CSD_TYPE_TIME, 0); // use first time grid as a template

		if(!topgrid)
		{
			Error("No time grid for first layer -- check grid database", MSG_ERR);
			return 1;
		}

		(*timediff) << (*topgrid); // copy size etc.

		layername=((CHorizon *)horizonlist.GetAt(i))->GetName();
		sprintf(buf, "Layer %s", (const char *)layername);

		sprintf(buf, "Computing interval velocity for layer %s", (const char *)layername);
		Log(buf, LOG_STD);

		// find both time grids
		bottomgrid=zimscube.FindHorizon(i, CSD_TYPE_TIME, 0);
		if(!bottomgrid)
		{
			Log(" No bottom time grid found", LOG_STD);
			return 1;
		}
		*timediff=*bottomgrid;
		if(i>0)
		{
			// if not in top layer, find time grid one layer up to compute tInt
			topgrid=zimscube.FindHorizon(i-1, CSD_TYPE_TIME, 0);
			if(!topgrid)
			{
				Log(" No top time grid found", LOG_STD);
				return 1;
			}
			*timediff -= *topgrid;
		}
		*timediff *= (1.0/2000.0); // convert into one-way seconds

		// find both depth grids
		bottomgrid=zimscube.FindHorizon(i, CSD_TYPE_DEPTH, 0);
		if(!bottomgrid)
		{
			Log(" No bottom depth grid found", LOG_STD);
			return 1;
		}
		if(i>0)
		{
			// if not in top layer, find depth grid one layer up to compute tInt
			topgrid=zimscube.FindHorizon(i-1, CSD_TYPE_DEPTH, 0);
			if(!topgrid)
			{
				Log(" No top depth grid found", LOG_STD);
				return 1;
			}
		}
		// create and compute a new vInt grid
		vInt=new CZimsGrid(this);

		*vInt = *bottomgrid;
		if(i>0)
			*vInt -= *topgrid;

		*vInt /= *timediff;
		if(i==0)
			gridname="Surface";
		else
			gridname=((CHorizon *)horizonlist.GetAt(i-1))->GetName();

		// remove impossible values by removing top and bottom 1%
		int percentToCutoff = AfxGetApp()->GetProfileInt("Well models", "PercentToCutoffInt", 1);
		vInt->ApplyCutoffAverage(percentToCutoff, true);

		min = vInt->CalcMinVal();
		max = vInt->CalcMaxVal();

		gridname+="_";
		gridname+=layername;

		// set the grid to be an interval
		vInt->SetType(CSD_TYPE_INTVEL);
		zimscube.Add(vInt, gridname, i);

		// make it primary
		zimscube.DesignateGrid(vInt);

		// remove temporary grid
		delete timediff;

		return 0;
	}

	// Computes interval velocities for all horizons
	void CVelmanDoc::OnVelocityvolumeComputeallintervalvelocities()
	{
		/*
		using dBtm-dTop
		vInt = 2000

		with a factor of 2000 because the times are two-way times in msec

		the denominator is stored in a temporary grid.
		*/

		int i, num, bad;
		bad = 0;

		// init progress monitor dialog
		m_pProgress = new CProgressMonit(this);
		if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
			return;

		m_pProgress->Create();
		m_pProgress->SetWindowText("Computing interval velocities");
		m_pProgress->ChangeGeneralDescription("Please wait while interval velocities for all layers are computed.");
		m_pProgress->ChangeText2("being computed");
		m_pProgress->InitializePercentage();

		// walk along all horizons
		num=horizonlist.GetSize();
		for(i=0;i<num;i++)
		{
			double min, max;

			if(ComputeIntervalvelocity(i, min, max) != 0)
				bad++;

			m_pProgress->ChangePercentage(((i+1)*100)/num);
		}

		// kill progress monitor
		m_pProgress->DestroyWindow();
		delete m_pProgress;
		m_pProgress=NULL;

		// if not bad grids, issue notification and quit
		if(bad == 0)
		{
			AfxMessageBox("Interval velocities have been successfully computed for all layers.", MB_OK | MB_ICONINFORMATION);
			Log("Interval velocities have been successfully computed for all layers.", LOG_STD);

			return;
		}
		else
		{
			// otherwise construct and issue warning message
			CString warning = "Not all interval velocity grids have been created.\nWould you like to inspect the log file?";

			if(AfxMessageBox(warning, MB_YESNO | MB_ICONINFORMATION)==IDYES)
				OnProjectViewlogfile();
		}
	}

	// Performs the IMRII depth artefact removal
	void CVelmanDoc::OnVelocityvolumeImrconverttimegrid()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CIMRDialog imrdlg;
		CZimsGrid *timegrid = NULL, *smoothgrid = NULL;
		int windowsize, gridindex, offset, Layers;
		int interp_method = 0; // linear interpolation
		char buffer[256];

		imrdlg.pDoc = this;
		if(imrdlg.DoModal() == IDOK && imrdlg.found)
		{
			// copy data out of the dialog
			timegrid = zimscube[imrdlg.timegrid];
			windowsize = imrdlg.smoothwindow;

			if(timegrid->GetHorizon() == 0)
			{
				AfxMessageBox("IMR can only be performed on layers below the first one.", MB_OK | MB_ICONINFORMATION);
				return;
			}

			//Layers = imrdlg.timegrid + 1;
			Layers = timegrid->GetHorizon() + 1;

			TRACE("Grid Name %s\n", timegrid->GetName());
			TRACE("Smoothsize %d\n", windowsize);

			// cursor to wait
			CWaitCursor wait;

			// create progress monitor and initialize it
			m_pProgress = new CProgressMonit(this);
			if(!m_pProgress || m_pProgress->GetSafeHwnd()!=0)
				return;

			m_pProgress->Create();
			m_pProgress->SetWindowText("IMR Processing");
			m_pProgress->ChangeGeneralDescription(
				"Please wait while grid is processed.");
			m_pProgress->ChangeText("IMR Processing");
			m_pProgress->ChangeText2("");
			m_pProgress->InitializePercentage();
			m_pProgress->ChangePercentage(0);

			// allocate memory for the new smoothed grid
			// and copy data into the new structure
			smoothgrid = new CZimsGrid(this);
			*smoothgrid << *timegrid;
			offset = (AfxGetApp()->GetProfileInt("ArithmeticParams", "IMROffset", 10));
			smoothgrid->SmoothMovingWindow(timegrid, windowsize+offset);

			// now try to convert this timeslice
			if(WarnCannotConvertSecondaryGrid(smoothgrid))
			{
				delete smoothgrid;
				m_pProgress->DestroyWindow();
				delete m_pProgress;
				m_pProgress=NULL;
				return;
			}

			// add grid to the zimscube
			zimscube.Add(smoothgrid, CString("S_"+timegrid->GetName()), (int) *smoothgrid);

			// get the last grid in the zimscube - this is the smooth grid
			gridindex = zimscube.GetUpperBound();
			// depthconvert with standard values
			zimscube.DepthConvertSecondaryTimegrids(1, &gridindex, interp_method, 100, FALSE, Layers);
			//depthgrid is the last grid to be added
			CZimsGrid *depthgrid = zimscube[zimscube.GetUpperBound()];
			// remove the smoothed grid from the zimscube
			zimscube.RemoveAt(smoothgrid);
			// now smoothe the depthgrid
			smoothgrid = new CZimsGrid(this);
			*smoothgrid << *depthgrid;
			smoothgrid->SmoothMovingWindow(depthgrid, windowsize);

			// now subtract the lf parts from the depthgrid
			// the result is the high pass filtered depthgrid
			*depthgrid -= *smoothgrid;
			// now depth--convert the time grid without filtering
			gridindex = zimscube.FindHorizon(timegrid);
			// depthconvert with standard values
			zimscube.DepthConvertSecondaryTimegrids(1, &gridindex, interp_method, 100, FALSE, Layers);
			CZimsGrid *depthgrid2 = zimscube[zimscube.GetUpperBound()];
			// subtract high frequency oscillations
			*depthgrid2 -= *depthgrid;

			CString Message = "The following grids have been created:\n\n";

			// rename both difference and resulting grid
			sprintf(buffer, "%s_DPS%d", (LPCTSTR) timegrid->GetName(), windowsize);
			depthgrid->SetName(CString(buffer));
			Message += depthgrid->GetName() + " (operator grid)";

			sprintf(buffer, "%s_IMRD%d", (LPCTSTR) timegrid->GetName(), windowsize);
			depthgrid2->SetName(CString(buffer));
			Message += "\n" + depthgrid2->GetName();

			//tell user what we've done
			AfxMessageBox(Message, MB_OK | MB_ICONINFORMATION);

			// now give back all memory
			delete(smoothgrid);
			m_pProgress->DestroyWindow();
			delete m_pProgress;
			m_pProgress=NULL;
		}
	}

	// Performs raytrace operation
	void CVelmanDoc::OnRaytrace()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CZimsGrid **olddepthgrids; // ptrs to 'normal' depthgrids
		CZimsGrid **depthdiffgrids; // difference grids between migrated and non-migrated depths
		CZimsGrid **raydepthgrids; // ptrs to newly created raytraced depthgrids
		CZimsGrid **raypregrids; // pts to ray traced grids before regridding takes place
		CZimsGrid **xoffsetgrids; // ptrs to x & y offset grids; displacement on horiz.
		CZimsGrid **yoffsetgrids;
		CZimsGrid **az_distgrids; // azimuthal distance grids (distance along vector)
		CZimsGrid **az_anglegrids; // azimuth angle grids (goes with above grids)
		CZimsGrid **anglegrids; // stores angles from rays at each horizon
		CZimsGrid **timegrids; // ptrs to current primary time grids
		CZimsGrid **intvelgrids; // ptrs to current interval velocity grids
		CZimsGrid **__timegrids; // internal arrays used to hold rebuilt grids
		CZimsGrid **__intvelgrids; // holds rebuilt arrays
		int i, j, horiz, nhorizons = horizonlist.GetSize();
		char name[256];
		BOOL ok; // if .FALSE. then raytracing was abandoned.
		BOOL depthgridsdone = FALSE;// if TRUE then there are ordinary depth grids
		// and the calc. of diff. grids is done.
		BOOL feet = Units & UNIT_OUTPUTINFEET;// TRUE if all output is in feet. This implies
		// that the internal velocity and depth grids will be in feet. The
		// raytracing requires metres so we have to convert to m/s before
		// calling the ray-tracing.

		BOOL keeppregrids;// if set TRUE then the depth grids computed by the
		// raytracing BEFORE the regridding is done are saved into
		// the cube and are available for checking by the user. This
		// is primarily a testing feature and would not normally be
		// enabled except during development.

		// First check raytracing is licensed. If it isn't bring up a
		// alert and quit. Check this even if the menu is disabled since
		// you never know....

		if ( !(((CVelmanApp *)AfxGetApp())->HaveRaytraceLic) )
		{
			Error("You are not licensed to use Ray Trace 3D.", MSG_ERR );
			return;
		}

		keeppregrids = AfxGetApp()->GetProfileInt("RayTracing", "KeepPreGrids", 0 ); // set FALSE (off) by default

		// Check we have all the grids present that we need.
		// Depthgrids, Time grids and Interval velocity grids.

		olddepthgrids = new CZimsGrid* [nhorizons]; // ptrs to existing grids
		timegrids = new CZimsGrid* [nhorizons]; // .. remember to delete these below
		intvelgrids = new CZimsGrid* [nhorizons];
		__timegrids = new CZimsGrid* [nhorizons]; // these next two are internal only
		__intvelgrids = new CZimsGrid* [nhorizons];

		for ( horiz = 0; horiz < nhorizons; horiz++ )
		{
			olddepthgrids[horiz] = zimscube.FindHorizon( horiz, CSD_TYPE_DEPTH, 0 );
			timegrids[horiz] = zimscube.FindHorizon( horiz, CSD_TYPE_TIME, 0 );
			intvelgrids[horiz] = zimscube.FindHorizon( horiz, CSD_TYPE_INTVEL, 0 );

			// check grids are available
			if ( olddepthgrids[horiz] )
			{
				depthgridsdone = TRUE;
			}
			if ( ! timegrids[horiz] )
			{
				Error( "You cannot use raytracing until primary "
					"time grids are available for all horizons.", MSG_ERR );
				delete[] olddepthgrids;
				delete[] timegrids;
				delete[] intvelgrids;
				return;
			}
			if ( ! intvelgrids[horiz] )
			{
				Error( "You cannot use raytracing until interval velocity "
					"grids are available for all horizons.\nIf modelling has been"
					" completed, they can be computed from the 'VelocityVolume' menu.", MSG_ERR );
				delete[] olddepthgrids;
				delete[] timegrids;
				delete[] intvelgrids;
				return;
			}

			// Now rebuild (remove null regions) the time and int. velocity grids
			// so that the raytracing always gets a value (note we still pass the original
			// timegrids so we can check whether the ray starts or hits a FALSE null region.

			int reb_pts = 50;
			__timegrids[horiz] = new CZimsGrid(this);
			__intvelgrids[horiz] = new CZimsGrid(this);
			*__timegrids[horiz] = *timegrids[horiz];
			*__intvelgrids[horiz] = *intvelgrids[horiz];
			__timegrids[horiz]->CleanGrid( reb_pts );
			__intvelgrids[horiz]->CleanGrid( reb_pts );

		}

		// Bring up an alert box to warn user this may take
		// a long time and are they sure they want to proceed.
		// The value '12000' below is chosen purely arbitrarily to issue the warning
		// if the raytracing is expected to take more than a few minutes.
		if ( olddepthgrids[0]->GetRows()*olddepthgrids[0]->GetColumns()*nhorizons > 12000 )
		{
			if (AfxMessageBox("Ray tracing for all horizons may take a long time.\n"
				"Are you sure you want to proceed?", MB_YESNO ) == IDNO )
				return;
		}

		// Allocate the grid arrays; they get initialised further down
		// (initialisation means the internal grid array is allocated for instance)
		raydepthgrids = new CZimsGrid* [nhorizons]; // create array of pointers to CZimsGrids..
		raypregrids = new CZimsGrid* [nhorizons];
		xoffsetgrids = new CZimsGrid* [nhorizons]; // .. remember to delete these arrays below..
		yoffsetgrids = new CZimsGrid* [nhorizons]; // .. but not the grids themselves!
		az_distgrids = new CZimsGrid* [nhorizons];
		az_anglegrids = new CZimsGrid* [nhorizons];
		depthdiffgrids = new CZimsGrid* [nhorizons]; // migrated - non-migrated depth grids
		anglegrids = new CZimsGrid* [nhorizons];
		for ( horiz = 0; horiz < nhorizons; horiz++ )
		{
			raydepthgrids[horiz] = new CZimsGrid(this);
			raypregrids[horiz] = new CZimsGrid(this);
			xoffsetgrids[horiz] = new CZimsGrid(this);
			yoffsetgrids[horiz] = new CZimsGrid(this);
			az_distgrids[horiz] = new CZimsGrid(this);
			az_anglegrids[horiz] = new CZimsGrid(this);
			depthdiffgrids[horiz] = new CZimsGrid(this);
			anglegrids[horiz] = new CZimsGrid(this);
		}

		// Initialise the grids
		// Set the grid values themselves to Znon so that any pts not able to be
		// raytraced are automatically taken care of.
		for ( horiz = 0; horiz < nhorizons; horiz++ )
		{
			*raydepthgrids[horiz] << *olddepthgrids[horiz]; // copies grid but NOT grid values
			*raydepthgrids[horiz] = ZIMS_VAL_ZNON; // overloaded operator sets entire grid
			raydepthgrids[horiz]->Stamp(); // timestamps last use
			*raypregrids[horiz] << *olddepthgrids[horiz];
			*raypregrids[horiz] = ZIMS_VAL_ZNON;
			raypregrids[horiz]->Stamp();
			xoffsetgrids[horiz]->Init( olddepthgrids[horiz]->GetRows(),
				olddepthgrids[horiz]->GetColumns(), this, CSD_TYPE_OTHER );
			yoffsetgrids[horiz]->Init( olddepthgrids[horiz]->GetRows(),
				olddepthgrids[horiz]->GetColumns(), this, CSD_TYPE_OTHER );
			xoffsetgrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			yoffsetgrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			xoffsetgrids[horiz]->SetHorizon( olddepthgrids[horiz]->GetHorizon() );
			yoffsetgrids[horiz]->SetHorizon( olddepthgrids[horiz]->GetHorizon() );
			*xoffsetgrids[horiz] = ZIMS_VAL_ZNON;
			*yoffsetgrids[horiz] = ZIMS_VAL_ZNON;
			az_distgrids[horiz]->Init( olddepthgrids[horiz]->GetRows(),
				olddepthgrids[horiz]->GetColumns(), this, CSD_TYPE_OTHER );
			az_anglegrids[horiz]->Init( olddepthgrids[horiz]->GetRows(),
				olddepthgrids[horiz]->GetColumns(), this, CSD_TYPE_OTHER );
			az_distgrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			az_anglegrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			az_distgrids[horiz]->SetHorizon( olddepthgrids[horiz]->GetHorizon() );
			az_anglegrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			*az_distgrids[horiz] = ZIMS_VAL_ZNON;
			*az_anglegrids[horiz] = ZIMS_VAL_ZNON;
			depthdiffgrids[horiz]->Init( olddepthgrids[horiz]->GetRows(),
				olddepthgrids[horiz]->GetColumns(), this, CSD_TYPE_DEPTH );
			depthdiffgrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			depthdiffgrids[horiz]->SetHorizon( olddepthgrids[horiz]->GetHorizon() );
			*depthdiffgrids[horiz] = ZIMS_VAL_ZNON;
			anglegrids[horiz]->Init( olddepthgrids[horiz]->GetRows(),
				olddepthgrids[horiz]->GetColumns(), this, CSD_TYPE_OTHER );
			anglegrids[horiz]->WriteLimits( olddepthgrids[horiz]->GetXlimits(),
				olddepthgrids[horiz]->GetYlimits() );
			anglegrids[horiz]->SetHorizon( olddepthgrids[horiz]->GetHorizon() );
			*anglegrids[horiz] = ZIMS_VAL_ZNON;
		}

		// Do the first horizon; this is a special case as the rays are vertical.
		// For first horizon all we do is copy the old grid since the ray always hits
		// the first horizon perpendicular and nothing shifts. Offset grids get set to zero.
		// Therefore there is no actual raytracing to be done.

		horiz = 0;
		*raydepthgrids[horiz] = *olddepthgrids[horiz]; // copies grid & grid values.
		*raypregrids[horiz] = *olddepthgrids[horiz];
		*xoffsetgrids[horiz] = 0.0;
		*yoffsetgrids[horiz] = 0.0;
		*anglegrids[horiz] = 0.0; // angular values is always zero at first horizon.

		// If modelling in feet then first convert to m/sec for the raytracing.

		if ( feet )
		{
			for ( horiz = 0; horiz < nhorizons; horiz++ )
				*__intvelgrids[horiz] *= METERS_PER_FOOT;
		}

		// Do the actual ray tracing

		ok = zimscube.RayDepth(raydepthgrids, raypregrids, timegrids, __timegrids,
			__intvelgrids, xoffsetgrids, yoffsetgrids, anglegrids, rt_limitanglevalue,
			GetRTregridbinsize(rt_regridbinsize));

		if ( ok )
		{
			int rows = timegrids[0]->GetRows();
			int columns = timegrids[0]->GetColumns();

			// Convert the output grids to feet if that's what we are modelling in.
			// Ray tracing always uses metres for depths (grid units are ALWAYS metres).

			if ( feet )
			{
				for ( horiz = 0; horiz < nhorizons; horiz++ )
				{
					*raydepthgrids[horiz] /= METERS_PER_FOOT;
					if ( keeppregrids )
						*raypregrids[horiz] /= METERS_PER_FOOT;
				}
			}

			// Compute the depth difference grids
			if ( depthgridsdone && rt_depthdiffgrids )
			{
				double newval, oldval;

				for ( horiz = 0; horiz < nhorizons; horiz++ )
				{
					for ( i = 0; i < rows; i++ )
					{
						for ( j = 0; j < columns; j++ )
						{
							newval = ((double **)(*raydepthgrids[horiz]))[i][j];
							oldval = ((double **)(*olddepthgrids[horiz]))[i][j];
							if ( ! raydepthgrids[horiz]->GetZnon(newval) &&
								! olddepthgrids[horiz]->GetZnon(oldval) )
							{
								((double **)(*depthdiffgrids[horiz]))[i][j] = newval - oldval;
							}
						}
					}
				}
			}

			// Compute the azimuth and vector distance grids. The azimuthal angle is
			// simply the N/E/S/W angle (i.e. clockwise from N) and the distance is
			// the length of the vector (in metres always).

			if ( rt_azimuthgrids )
			{
				double xx, yy, hyp, cosa;
				double pi = acos(-1.0);
				// n.b. if 0 (zero), azimuth angle is in radians, if 1 it's stored in degrees
				BOOL az_units = AfxGetApp()->GetProfileInt("RayTracing", "AzimuthAngleUnits", 1 );
				double rad2deg = 180.0 / pi;
				double eps = 0.001;// if distances are less than this set them to zero.
				// do this to avoid getting silly answers from small nos.

				for ( horiz=0; horiz < nhorizons; horiz++ )
				{
					for ( i=0; i<rows; i++ )
					{
						for ( j=0; j<columns; j++ )
						{
							xx = ((double **)(*xoffsetgrids[horiz]))[i][j];
							yy = ((double **)(*yoffsetgrids[horiz]))[i][j];
							if ( ! olddepthgrids[horiz]->GetZnon(xx) &&
								! olddepthgrids[horiz]->GetZnon(yy) )
							{
								if ( fabs(xx) < eps )
									xx = 0.0;
								if ( fabs(yy) < eps )
									yy = 0.0;
								hyp = sqrt(xx*xx + yy*yy);
								((double **)(*az_distgrids[horiz]))[i][j] = hyp;
								if ( hyp == 0.0 )
								{
									((double **)(*az_anglegrids[horiz]))[i][j] = 0.0;
								}
								else
								{
									cosa = yy/hyp;
									((double **)(*az_anglegrids[horiz]))[i][j] = acos(cosa); // angle in radians
									if ( xx < 0.0 )
										((double **)(*az_anglegrids[horiz]))[i][j] = 2.0*pi -
										((double **)(*az_anglegrids[horiz]))[i][j];
									if ( az_units )
										((double **)(*az_anglegrids[horiz]))[i][j] *= rad2deg;
								}
							}
						}
					}
				}
			}

			// Finish setting up the grids
			// don't bother for the ones we are not keeping
			for ( horiz = 0; horiz < nhorizons; horiz++ )
			{
				sprintf( name, "%s_ray", (LPCTSTR) olddepthgrids[horiz]->GetName());
				raydepthgrids[horiz]->SetName(CString(name));
				raydepthgrids[horiz]->CalcAverage();
				raydepthgrids[horiz]->SetType(CSD_TYPE_DEPTH);
				raydepthgrids[horiz]->CalcMaxVal();
				raydepthgrids[horiz]->CalcMinVal();

				if ( keeppregrids )
				{
					sprintf( name, "%s_ray_pregrid", (LPCTSTR) olddepthgrids[horiz]->GetName());
					raypregrids[horiz]->SetName(CString(name));
					raypregrids[horiz]->CalcAverage();
					raypregrids[horiz]->SetType(CSD_TYPE_DEPTH);
					raypregrids[horiz]->CalcMaxVal();
					raypregrids[horiz]->CalcMinVal();
				}
				if ( rt_anglegrids )
				{
					sprintf( name, "%s_ray_angle", (LPCTSTR) olddepthgrids[horiz]->GetName());
					anglegrids[horiz]->SetName(CString(name));
					anglegrids[horiz]->CalcAverage();
					anglegrids[horiz]->SetType(CSD_TYPE_OTHER);
					anglegrids[horiz]->CalcMaxVal();
					anglegrids[horiz]->CalcMinVal();
				}

				if ( rt_xyoffsetgrids )
				{
					sprintf( name, "%s_ray_xdisp", (LPCTSTR) olddepthgrids[horiz]->GetName());
					xoffsetgrids[horiz]->SetName(CString(name));
					sprintf( name, "%s_ray_ydisp", (LPCTSTR) olddepthgrids[horiz]->GetName());
					yoffsetgrids[horiz]->SetName(CString(name));
					xoffsetgrids[horiz]->CalcAverage();
					yoffsetgrids[horiz]->CalcAverage();
					xoffsetgrids[horiz]->SetType(CSD_TYPE_OTHER);
					yoffsetgrids[horiz]->SetType(CSD_TYPE_OTHER);
					xoffsetgrids[horiz]->CalcMaxVal();
					yoffsetgrids[horiz]->CalcMaxVal();
					xoffsetgrids[horiz]->CalcMinVal();
					yoffsetgrids[horiz]->CalcMinVal();
				}

				if ( rt_azimuthgrids )
				{
					sprintf( name, "%s_az_dist", (LPCTSTR) olddepthgrids[horiz]->GetName() );
					az_distgrids[horiz]->SetName(CString(name));
					sprintf( name, "%s_az_angle", (LPCTSTR) olddepthgrids[horiz]->GetName() );
					az_anglegrids[horiz]->SetName(CString(name));
					az_distgrids[horiz]->CalcAverage();
					az_anglegrids[horiz]->CalcAverage();
					az_distgrids[horiz]->SetType(CSD_TYPE_OTHER);
					az_anglegrids[horiz]->SetType(CSD_TYPE_OTHER);
					az_distgrids[horiz]->CalcMaxVal();
					az_anglegrids[horiz]->CalcMaxVal();
					az_distgrids[horiz]->CalcMinVal();
					az_anglegrids[horiz]->CalcMinVal();
				}

				if ( depthgridsdone && rt_depthdiffgrids )
				{
					sprintf( name, "%s_ray_depthdiff", (LPCTSTR) olddepthgrids[horiz]->GetName() );
					depthdiffgrids[horiz]->SetName(CString(name));
					depthdiffgrids[horiz]->CalcAverage();
					depthdiffgrids[horiz]->SetType(CSD_TYPE_DEPTH);
					depthdiffgrids[horiz]->CalcMaxVal();
					depthdiffgrids[horiz]->CalcMinVal();
				}

				// Now add the new grids into the grid array and designate them as
				// the primary grids
				// set 3rd arg to -1 as we want grid to use horiz value in its object
				zimscube.Add( raydepthgrids[horiz], raydepthgrids[horiz]->GetName(), -1 );
				//zimscube.DesignateGrid( raydepthgrids[horiz] ); Decided not to make new grid primary by default

				if ( keeppregrids )
					zimscube.Add( raypregrids[horiz], raypregrids[horiz]->GetName(), -1 );
				if ( rt_xyoffsetgrids )
				{
					zimscube.Add( xoffsetgrids[horiz], xoffsetgrids[horiz]->GetName(), -1 );
					zimscube.Add( yoffsetgrids[horiz], yoffsetgrids[horiz]->GetName(), -1 );
				}
				if ( rt_azimuthgrids )
				{
					zimscube.Add( az_distgrids[horiz], az_distgrids[horiz]->GetName(), -1 );
					zimscube.Add( az_anglegrids[horiz], az_anglegrids[horiz]->GetName(), -1 );
				}
				if ( rt_anglegrids )
					zimscube.Add( anglegrids[horiz], anglegrids[horiz]->GetName(), -1 );
				if ( depthgridsdone && rt_depthdiffgrids )
					zimscube.Add( depthdiffgrids[horiz], depthdiffgrids[horiz]->GetName(), -1 );
			}

			AfxMessageBox("Ray trace 3D grids have been created. "
				"We recommend you now tie the depth grids to the wells." );

			ProjState|=PRJ_RAYTRACED;
			UpdateAllViews(NULL); // forces status display on main window to be updated
		}
		else
		{
			AfxMessageBox("An error occurred during raytracing and it "
				"could not be completed." );
		}

		// Tidy up; delete everything that needs deleting
		delete[] olddepthgrids;
		delete[] timegrids;
		delete[] intvelgrids;
		delete[] raydepthgrids;
		delete[] raypregrids;
		for ( horiz = 0; horiz < nhorizons; horiz++ )// delete the rebuilt grids
		{
			delete __timegrids[horiz];
			delete __intvelgrids[horiz];
		}
		delete[] __timegrids;
		delete[] __intvelgrids;
		if ( rt_xyoffsetgrids )
		{
			delete[] xoffsetgrids;
			delete[] yoffsetgrids;
		}
		if ( this->rt_azimuthgrids )
		{
			delete[] az_distgrids;
			delete[] az_anglegrids;
		}
		if ( rt_depthdiffgrids )
			delete[] depthdiffgrids;
		if ( rt_anglegrids )
			delete[] anglegrids;

		return;
	}

	// Displays the Raytrace properties dialogue box
	void CVelmanDoc::OnRaytrace3dProperties()
	{
#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		CRayTracePropsDlg dlg;

		// set dialogue items
		dlg.m_azimuthgrids = rt_azimuthgrids;
		dlg.m_anglegrids = rt_anglegrids;
		dlg.m_depthdiffgrids = rt_depthdiffgrids;
		dlg.m_xyoffsetgrids = rt_xyoffsetgrids;
		dlg.m_limitanglevalue = rt_limitanglevalue;
		dlg.m_regridbinsize = rt_regridbinsize;

		if ( dlg.DoModal() == IDOK )
		{
			// copy member variables back
			rt_azimuthgrids = dlg.m_azimuthgrids;
			rt_anglegrids = dlg.m_anglegrids;
			rt_depthdiffgrids = dlg.m_depthdiffgrids;
			rt_xyoffsetgrids = dlg.m_xyoffsetgrids;
			rt_limitanglevalue = dlg.m_limitanglevalue;
			rt_regridbinsize = dlg.m_regridbinsize;
		}
	}

	// Opens the project in VM3D, a seperate program for displaying the grids
	void CVelmanDoc::OnProject3dvis()
	{

#ifdef VELMAN_UNIX
		return;
#endif

#ifdef VelmanLight
		AfxMessageBox(VMLightMessage, MB_OK | MB_ICONINFORMATION);
		return;
#endif

		if(HaveValidDocument)
		{
			//check required .net framework installed
			if(!IsNetfx40ClientInstalled())
			{
				AfxMessageBox( "Microsoft .NET Framework 4.0 needs to be installed "
					" before the 3D visualisation can be used.", MB_OK | MB_ICONINFORMATION);
				return;
			}

			// make sure we save project first
			OnSaveDocument(ProjectName);

			// Launch external visualisation program; usually in same directory
			CMainFrame::ShellExecuteAndWait("VM3D\\VM3D.exe", "\""+ ProjectName + "\"", false);

		}

	}

	// Gets the directory that we use to export all sorts of data to
	CString CVelmanDoc::GetExportDir()
	{
		//ensure directory exists
		CreateDirectory(ExportDir, NULL);
		return ExportDir;
	}

	// Opens the selected help video
	void CVelmanDoc::OpenVideo(CString FileName)
	{
#ifdef VELMAN_UNIX
		return;
#endif

		//get location of exe
		TCHAR acAppPath[MAX_PATH];
		GetModuleFileName(NULL, acAppPath, MAX_PATH);
		LPSTR exefolder = (LPSTR)acAppPath;
		PathRemoveFileSpec(exefolder);

		//add folder and file name
		CString path = exefolder;
		path.Append("\\Video tutorials\\");
		path.Append(FileName);

		//open the video
		int value = (int)ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOW);

		//http://msdn.microsoft.com/en-us/library/bb762153(VS.85).aspx
		if(value <= 32)
			AfxMessageBox("There was a problem opening the video. Ensure there is an application to play the video installed and the video is present (if not then see the installation CD.)", MB_OK | MB_ICONINFORMATION);
	}

	void CVelmanDoc::OnOpenDemoVideo()
	{
		OpenVideo("1.01 Getting started with functions.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosFunctionmodelling()
	{
		OpenVideo("1.02 Function modelling.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosFunctionrefinement()
	{
		OpenVideo("1.03 Function refinement.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosFunctionqualitycontrol()
	{
		OpenVideo("1.04 Function quality control.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosSavingwelldata()
	{
		OpenVideo("1.05 Saving well data.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosFunctionresidualcontrols()
	{
		OpenVideo("1.06 Function residual controls.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosGettingstartedwithrmsvelocities()
	{
		OpenVideo("2.01 Getting started with RMS velocities.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosProcessvelocitydata()
	{
		OpenVideo("2.02 Process velocity data.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosRmsqualitycontrol()
	{
		OpenVideo("2.03 RMS quality control.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosImportingsegydata()
	{
		OpenVideo("2.04 Importing SEGY data.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosSavinggrids()
	{
		OpenVideo("3.01 Saving grids.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosWelldataformat()
	{
		OpenVideo("3.02 Well data format.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosImrii()
	{
		OpenVideo("3.03 IMRII.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideos3dvisualisation()
	{
		OpenVideo("3.04 3D Visualisation.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosDepthmigration()
	{
		OpenVideo("3.05 Depth migration.wmv");
	}

	void CVelmanDoc::OnDemonstrationvideosDepthconvertsecondarygrid()
	{
		OpenVideo("3.06 Depth convert secondary grid.wmv");
	}

	//
	// Methods for updating menu buttons
	//

	// well data
	void CVelmanDoc::OnUpdateWellmodellingImportwelldata(CCmdUI* pCmdUI)
	{
		// only enable "Import well data" item if the project scope needs the well data!
		pCmdUI->Enable(ProjectScope!=SCOPE_TIMERMS);
	}

	void CVelmanDoc::OnUpdateImportImporttimedata(CCmdUI* pCmdUI)
	{
		// not available for "velocity model only" or "time and well data only" projects
		pCmdUI->Enable(!(ModelViewOpen | ListOfModelsOpen | TimegridContourOpen |
			TyingTableOpen | SeisdataMapOpen));
	}

	void CVelmanDoc::OnUpdateWelldataDeletewell(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateWelldataInterpolatefaultnullpolygons(CCmdUI *pCmdUI)
	{

	}

	// seismic data
	void CVelmanDoc::OnUpdateWellshorizonsDefinehorizons(CCmdUI* pCmdUI)
	{
		// only enable "Define horizons" item if the project scope needs *NO* well data!
		pCmdUI->Enable(ProjectScope==SCOPE_TIMERMS);
	}

	void CVelmanDoc::OnUpdateSeismicdataOpensegyconverter(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(!(ProjectScope==SCOPE_TIMEWELL || ProjectScope==0));
	}

	void CVelmanDoc::OnUpdateSeismicdataTestrmsimport(CCmdUI* pCmdUI)
	{
		// not available for "velocity model only" or "time and well data only" projects
		pCmdUI->Enable(!(ProjectScope==SCOPE_TIMEWELL || ProjectScope==0));
	}

	void CVelmanDoc::OnUpdateImportImportrmsdata(CCmdUI* pCmdUI)
	{
		// not available for "velocity model only" or "time and well data only" projects
		pCmdUI->Enable((ProjectScope!=SCOPE_TIMEWELL) && (ProjectScope!=0) && (ProjState & PRJ_HAVETIME));
	}

	void CVelmanDoc::OnUpdateSeismicdataMapofshotpoints(CCmdUI* pCmdUI)
	{
		// just like process RMS velocities
		pCmdUI->Enable( !SeisdataMapOpen && (ProjState & PRJ_HAVERMSVEL) && (ProjState & PRJ_HAVETIME));
	}	

	void CVelmanDoc::OnUpdateProcessProcessrmsvelocities(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable( (ProjState & PRJ_HAVERMSVEL) && (ProjState & PRJ_HAVETIME));
	}

	void CVelmanDoc::OnUpdateDepthconversionCreatedepthgrids(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable(ProjectScope!=SCOPE_VELOCITIES); // not available for "velocity model only" projects
	}

	void CVelmanDoc::OnUpdateImportFiltermanagement(CCmdUI *pCmdUI)
	{

	}

	// velocity volume
	void CVelmanDoc::OnUpdateDepthconversionConvertsecondarytimegrid(CCmdUI* pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif

		// only enable command if depth conversion has been done; either if all
		// layers have been modelled or if the RMS data has been depth converted
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube &&
			((ProjState & PRJ_HAVEMODEL) && ProjectScope==1 ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>1));
	}

	void CVelmanDoc::OnUpdateDepthconversionConverttimeslice(CCmdUI* pCmdUI)
	{
		// only enable command if depth conversion has been done; either if all
		// layers have been modelled or if the RMS data has been depth converted
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube &&
			((ProjState & PRJ_HAVEMODEL) && ProjectScope==SCOPE_TIMEWELL ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>SCOPE_TIMEWELL));
	}

	void CVelmanDoc::OnUpdateDepthconversionVelocitycubeVelocitytimeslice(CCmdUI* pCmdUI)
	{
		// only enable command if depth conversion has been done; either if all
		// layers have been modelled or if the RMS data has been depth converted
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube &&
			((ProjState & PRJ_HAVEMODEL) && ProjectScope==SCOPE_TIMEWELL ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>SCOPE_TIMEWELL));
	}

	void CVelmanDoc::OnUpdateDepthconversionVelocitycubeConvertsinglewelllocations(CCmdUI* pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif

		// only enable command if depth conversion has been done; either if all
		// layers have been modelled or if the RMS data has been depth converted
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube &&
			((ProjState & PRJ_HAVEMODEL) && ProjectScope==SCOPE_TIMEWELL ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>SCOPE_TIMEWELL) );
	}

	void CVelmanDoc::OnUpdateDepthconversionVelocitycubeWellforecast(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube);
	}

	void CVelmanDoc::OnUpdateVelocityvolumeComputeallintervalvelocities(CCmdUI* pCmdUI)
	{
		// only enable command if depth conversion has been done; either if all
		// layers have been modelled or if the RMS data has been depth converted
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube &&
			((ProjState & PRJ_HAVEMODEL) && ProjectScope==SCOPE_TIMEWELL ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>SCOPE_TIMEWELL) );
	}

	void CVelmanDoc::OnUpdateVelocityvolumeImrconverttimegrid(CCmdUI* pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif

		// this is where we control whether to gray out IMR in menu,
		// this uses the same case as OnUpdateDepthconversionConverttimeslice.

		// only enable command if depth conversion has been done; either if all
		// layers have been modelled or if the RMS data has been depth converted
		pCmdUI->Enable(((CVelmanApp *)AfxGetApp())->HaveVelocityCube &&
			((ProjState & PRJ_HAVEMODEL) && ProjectScope==SCOPE_TIMEWELL ||
			(ProjState & PRJ_HAVEDEPTHCONV) && ProjectScope>SCOPE_TIMEWELL));
	}

	// depth migration 3d
	void CVelmanDoc::OnUpdateRaytrace(CCmdUI* pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif

		// Condition for controlling whether the menu is enabled or not.
		pCmdUI->Enable( ((CVelmanApp *)AfxGetApp())->HaveRaytraceLic );
	}

	void CVelmanDoc::OnUpdateRaytrace3dProperties(CCmdUI* pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif

		// Condition for controlling whether the menu item is enabled or not.
		pCmdUI->Enable( ((CVelmanApp *)AfxGetApp())->HaveRaytraceLic );
	}

	// grid tools
	void CVelmanDoc::OnUpdateGridtoolsDirectory(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsExport(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsImport(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsDelete(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsDuplicate(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsRename(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridtoolsDesignate(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridtoolsStatistics(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsFouriertransform(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsTietowelldata(CCmdUI* pCmdUI)
	{
		// not available for "velocity model only" or "time and rms data only" projects
		pCmdUI->Enable(ProjectScope==SCOPE_TIMEWELL || ProjectScope==SCOPE_TIMEWELLRMS);
	}	

	void CVelmanDoc::OnUpdateGridsFFTSmooth(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif
	}

	void CVelmanDoc::OnUpdateGridsMatrixSmooth(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
#endif
	}

	void CVelmanDoc::OnUpdateGridsRebuild(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif
	}

	void CVelmanDoc::OnUpdateGridtoolsLimit(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif
	}

	void CVelmanDoc::OnUpdateGridsSpectrumanalysis(CCmdUI *pCmdUI)
	{

	}

	void CVelmanDoc::OnUpdateGridsCreateconstantgrid(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif
	}

	void CVelmanDoc::OnUpdateGridsGridarithmetic(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif
	}


	void CVelmanDoc::OnUpdateGridsOverwritenull(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
		return;
#endif
	}

	// 3d vis
	void CVelmanDoc::OnUpdateProject3dvis(CCmdUI *pCmdUI)
	{
#ifdef VelmanLight
		pCmdUI->Enable(0);
#endif
	}


