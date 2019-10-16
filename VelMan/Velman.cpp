// velman.cpp : Defines the class behaviors for the application.

/* Licensing documentation

There are three ways of licensing this product on source-code level:

a) not at all
Rationale: all PC versions, testing versions on the sun
Note: This still means one can protect the product using a dongle and an
"envelope" program around the executable
b) quick-and-dirty
Rationale: for trial versions at trusted sites, such as Shell
This works by setting an expiry date. The software will work completely
(both the program itself and the extra "VelocityVolume" module), and will not
work anymore after the expiry date. Can have infinite number of concurrent
users, can run on any platform, can make infinitely many copies. Does NOT check
for specialties, such as tampering the system date.
Look for LICENSE_QUICKANDDIRTY in the code (using #ifdefs)
c) good-but-dear
Rationale: For actual licenses where one needs to limit the number of users,
or for trial versions on non-trusted sites (which might make copies/tamper
their date)
Works with the "Sentinel" license manager using key codes
Look for LICENSE_GOODBUTDEAR in the code (using #ifdefs)

Note that only one of either b) or c) must be selected, otherwise an error
occurs at compile time. The same is TRUE if one selects c) type licensing on
the PC.

Note also that option c) requires the flag LICENSE_GOODBUTDEAR to be #defined
in the file "mainfrm.cpp" also. It is only for reasons of documentation that
the flags are to be #defined in the source code, rather than on the compiler
command line using the -D option.

None of this has anything to do with the "ONLY_DEMO_VERSION" flag, which selects
a special version of Velman which by itself will run indefinitely (but may be
combined with any type of licensing), but does not allow the creation of new
projects, so one can only open existing projects.

*/

//****************************************************************************************
// uncomment the following line to create a demo version of Velocity Manager
// this needs doing in this file only
//#define ONLY_DEMO_VERSION
//****************************************************************************************

#ifdef VELMAN_UNIX
//---------------------------- configurable options for licensing --------------
//  LICENSING: Before delivering to customers, you should have one of
//             either LICENSE_QUICKANDDIRTY or LICENSE_GOODBUTDEAR, but
//             not both. If you uncomment both of these there will be no
//             licensing effective at all and the EXPIRY options set below
//             will not have any effect. Note that if LICENSE_GOODBUTDEAR
//             is uncommented then the EXPIRY options also have no effect.
//
// The following line selects the type of licensing desired
// select licensing type b)
// #define LICENSE_QUICKANDDIRTY
// select licensing type c)
#define LICENSE_GOODBUTDEAR

// options for b) type licensing
// these three items define the last day on which Velocity Manager runs
// (on Unix systems, no checking is done on the PC version)
#define LICENSE_EXPIRY_DAY 30
#define LICENSE_EXPIRY_MONTH 9 // 0=January..11=December
#define LICENSE_EXPIRY_YEAR 105 // year minus 1900
#else // this is PC - make sure no licensing is turned on
#undef LICENSE_GOODBUTDEAR
#undef LICENSE_QUICKANDDIRTY
#endif
//---------------------------- end of configurable options for licensing -------

// make sure no c) or b) type licensing on PC
#ifndef VELMAN_UNIX
#ifdef LICENSE_GOODBUTDEAR
#error Sentinel-type licensing will not work on PC platform
#endif
#ifdef LICENSE_QUICKANDDIRTY
#error quick and dirty licensing will not work on PC platform
#endif
#endif

// check no conflicting licensing under Unix
#ifdef LICENSE_QUICKANDDIRTY
#ifdef LICENSE_GOODBUTDEAR
#error Must not select both licensing types on Unix platform
#endif
#endif

#include "stdafx.h"
#include "velman.h"

#include "mainfrm.h"
#include "VelmanDoc.h"
#include "VelmanView.h"

#include "CoarseViewDlg.h"
#include "finetune.h"
#include "ListModelsView.h"
#include "ContourView.h"
#include "WellDataView.h"
#include "map1view.h"

#include "openfilebrowser.h"

#include <direct.h>
#include <time.h> // this is only needed for the quick-and-dirty licensing

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// include Sentinel stuff if required
#ifdef LICENSE_GOODBUTDEAR

#include "slm.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

SLM_INSTANCE	SLinstance;
SLM_CONNECTION	SLconnection;
char SLserver[SLM_HOST_LEN + 1];

/////////	Callback for heartbeat.
void Hrtbeat_exception(long code, char* feature)
{
	pid_t	pidgrp;

	//  If this procedure ever gets called it means that something nasty has
	//  happened to the license server. This may happen because of network
	//  timeouts, the server has crashed (or not responding). Or the sys. admin
	//  may have used the elmadmin -b option to forcibly remove the license from
	//  this running velman. This latter reason is a serious concern because unchecked
	//  it would allow unscrupulous customers to run infinite number of velmans
	//
	//  My strategy here is to allow velman to continue under certain circumstances
	//  such as a server down. A message is sent to stdout in all cases. If the
	//  server has gone down, velman will automatically reconnect when the server
	//  comes back up.  For other error conditions, including the use of elmadmin -b
	//  we kill the program. See sentinelLM reference manual (page 4.24) for more
	//  details.

	printf("%s.\n", slm_message(SLinstance, SLserver, feature, code));

	if (code == SLM_NOT_LICENSED || code == SLM_LOST || code == SLM_EXPIRED ||
		code == SLM_INSUFFICIENT_SERVERS || code == SLM_INVALID_CLIENT)
	{
		//  something like this doesn't work. it hangs the program. i suspect it's
		//  because it's a blocking alert running in a forked process. It would be nice
		//  though if we could bring this to the users attention in a more obvious way
		//  before the program terminates - perhaps give the user 60 secs to save any work.
		//
		//	AfxMessageBox("VelocityManager has lost its license.\n"
		//	"This is either because it was removed by the system administrator or"
		//	"the license server went down and VelocityManager failed to reconnect.\n"
		//	"VelocityManager must now stop. Please click on OK.", MB_OK | MB_ICONSTOP );

		pidgrp = getpgrp();		// find out process group

		slm_disconnect(SLinstance, SLconnection);
		slm_endapi(&SLinstance);

		printf("Velman no longer has a license and has terminated.\n");
		sigsend(P_PGID, pidgrp, 9);	// send SIGKILL to velman
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CVelmanApp

#define theDerivedClass CVelmanApp ::
BEGIN_MESSAGE_MAP(CVelmanApp, CWinApp)
	//{{AFX_MSG_MAP(CVelmanApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
#undef theDerivedClass
#define theDerivedClass
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()
#undef theDerivedClass

/////////////////////////////////////////////////////////////////////////////
// CVelmanApp construction

CVelmanApp::CVelmanApp()
{
	CWinApp::EnableHtmlHelp();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVelmanApp object

CVelmanApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CVelmanApp initialization

BOOL CVelmanApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	//set ini file location
	if (AfxGetApp()->m_pszProfileName)
	{
		free((void*)AfxGetApp()->m_pszProfileName);
	}
	CString dir = CMainFrame::GetMyDocsFolder() + "\\Velman.ini";

	LPCSTR d = (LPCSTR)dir;

	AfxGetApp()->m_pszProfileName = _tcsdup(d);

	CWaitCursor wait;

	OpendContSettings = FALSE; // variable to ensure no more than 1 settings window
	// open at any one time for the contour windows

	//SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings(10); // Load standard INI file options (including MRU)
	FitInXandY = GetProfileInt("ArithmeticParams", "FitInXandY", 0);

#ifdef LICENSE_QUICKANDDIRTY
	// here we do the quick-and-dirty license checking. Features:
	// a) expires on the day given in the #define's at the beginning of this file
	// b) works on all machines, with any number of users
	// c) only active under Unix, PC version always works forever

	// find current time
	time_t current_dt = time(NULL);
	struct tm* tm_dt = gmtime(&current_dt);
	// compare to preset expiry time
	if (tm_dt->tm_year > LICENSE_EXPIRY_YEAR ||
		(tm_dt->tm_year == LICENSE_EXPIRY_YEAR && tm_dt->tm_mon > LICENSE_EXPIRY_MONTH) ||
		(tm_dt->tm_year == LICENSE_EXPIRY_YEAR && tm_dt->tm_mon == LICENSE_EXPIRY_MONTH &&
			tm_dt->tm_mday > LICENSE_EXPIRY_DAY))
	{
		AfxMessageBox("The evaluation period of VelocityManager has expired.\n\n"
			"Please contact Cambridge Petroleum Software for details of the purchase"
			" of an unrestricted copy.\n\n"
			"Press OK to terminate the application.", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
#endif

	HaveVelocityCube = TRUE;  // always enabled on PC, but might be reset further down
	// if running under unix
	HaveRaytraceLic = TRUE;  // turn on raytracing menu.

#ifdef LICENSE_GOODBUTDEAR

	char	feature01[SLM_FEATURE_LEN + 1] = "01";	// this is Velocity Manager
	char	feature02[SLM_FEATURE_LEN + 1] = "02";	// this is Velocity Volume + feature 1.
	char    feature03[SLM_FEATURE_LEN + 1] = "03";    // this is ray tracing + features 1 & 2
	long	status;

	//    initialise the license server
	SLserver[0] = '\0';
	if ((status = slm_startapi(&SLinstance)) != SLM_OK)
	{
		AfxMessageBox(slm_message(SLinstance, SLserver, (char*)0, status), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	//  try and connect to a license server.
	//  since server is empty, the type of connection (broadcast, explicit etc) is under the
	//  control of the installation
	//  slm_connect( instance, host, feature, keydir, callback )
	//  note that it's important to set feature to NULL because of the way
	//  we do the feature priority below.
	if ((SLconnection = slm_connect(SLinstance, SLserver, (char*)0,
		(char*)0, (long (*)(void*, long, long))NULL)) < 0)
	{
		AfxMessageBox(slm_message(SLinstance, SLserver, (char*)0, SLconnection), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	//  try and get a license. Note that higher numbered features include all lower numbered ones.
	//  try for raytracing license first (features 3, 2 & 1 enabled)
	if ((status = slm_getlicense(SLinstance, SLconnection, feature03, SLM_GETLIC, 0L, NULL, NULL)) < 0)
	{
		//		no raytracing; try velocity volume (features 2 & 1 only)
		HaveRaytraceLic = FALSE;
		if ((status = slm_getlicense(SLinstance, SLconnection, feature02, SLM_GETLIC, 0L, NULL, NULL)) < 0)
		{
			//	didn't get a license, try velocity manager instead
			HaveVelocityCube = FALSE;
			if ((status = slm_getlicense(SLinstance, SLconnection, feature01, SLM_GETLIC, 0L, NULL, NULL)) < 0)
			{
				//	no licenses for anything available
				AfxMessageBox(slm_message(SLinstance, SLserver, feature01, status), MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	//	establish the heartbeat using Unix style signals via sentinel API and not mainwin
	slm_heartbeat(SLinstance, SLconnection, 60, Hrtbeat_exception);

#endif

	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd), // standard MDI child frame
		RUNTIME_CLASS(CVelmanView));
	AddDocTemplate(pDocTemplate);

	m_pTemplate1 = new CMultiDocTemplate(IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CCoarseModelView));

	/* Multi form behaviour removed
	#ifdef VELMAN_UNIX // allows multiple contours only on pc
	m_pTemplate2=new CMultiDocTemplate(IDR_VELMANTYPE,
	RUNTIME_CLASS(CVelmanDoc),
	RUNTIME_CLASS(CMDIChildWnd),
	RUNTIME_CLASS(CContourView));
	#else
	m_pTemplate2=new CMultiDocTemplate(IDR_VELMANTYPE,
	RUNTIME_CLASS(CVelmanDoc),
	//RUNTIME_CLASS(CMDIChildWnd),
	RUNTIME_CLASS(CFrameWnd),
	RUNTIME_CLASS(CContourView));
	#endif
	*/
	m_pTemplate2 = new CMultiDocTemplate(IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CContourView));// might be poss to use CMDIFrameWnd

	m_pTemplate3 = new CMultiDocTemplate(IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CWellDataView));

	m_pTemplate4 = new CMultiDocTemplate(IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CFineTuneView));

	m_pTemplate6 = new CMultiDocTemplate(IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CListOfModelsView));

	m_pTemplate7 = new CMultiDocTemplate(IDR_VELMANTYPE,
		RUNTIME_CLASS(CVelmanDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CMapOfShotpointsView));

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// enable file manager drag/drop and DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();

	// simple command line parsing
	if (m_lpCmdLine[0] != '\0')
	{
		// open an existing document
		OpenDocumentFile(m_lpCmdLine);
	}

	m_pMainWnd->DragAcceptFiles();
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);

	int width = GetProfileInt("Window Placement", "Width", 0);
	//int height=GetProfileInt("Window Placement", "Height",-1);
	//if(width!=-1 && height!=-1)
	//	pMainFrame->SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

	pMainFrame->UpdateWindow();

	return TRUE;
}

int CVelmanApp::ExitInstance()
{
	// Clear up secondary templates
	delete m_pTemplate1;
	delete m_pTemplate2;
	delete m_pTemplate3;
	delete m_pTemplate4;
	delete m_pTemplate5;
	delete m_pTemplate6;
	delete m_pTemplate7;

#ifdef LICENSE_GOODBUTDEAR
	slm_disconnect(SLinstance, SLconnection);
	slm_endapi(&SLinstance);
#endif
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWelldataInterpolatefaultnullpolygons();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

#define theDerivedClass CAboutDlg ::
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#undef theDerivedClass

// App command to run the dialog
void CVelmanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVelmanApp commands

void CVelmanApp::OnFileOpen()
{
	CString directory;
	char current_dir[1024];

#ifndef VELMAN_UNIX

	// we're on a windows platform
	CFileDialog dlg(TRUE, ".prj", "*.prj", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Project Files (*.prj) | *.prj | All Files (*.*) | *.* ||");
	char* defdir = getenv("VELMAN_DEFAULTDIR");
	if (defdir)
	{
		directory = defdir;
	}
	else
	{
		if (!_getcwd(current_dir, 1023))
			strcpy(current_dir, "/");
		directory = GetProfileString("New Projects", "TargetDir", current_dir);
		if (directory == "")
			directory = "/";
	}
	_chdir((const char*)directory);

	char* title = "Open Project";

	// Set initial dir for dialogue.
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)directory;
	dlg.m_ofn.lpstrTitle = title;
	dlg.m_ofn.Flags &= (~OFN_SHOWHELP);
	if (dlg.DoModal() == IDOK)
		CWinApp::OpenDocumentFile(dlg.GetPathName());

#else
	COpenFileBrowser dlg_new;
	dlg_new.extensionPhrase = "Project Files (*.prj), *.prj";
	dlg_new.title = "Open Project";
	if (dlg_new.DoModal() == IDOK)
	{
		directory = dlg_new.m_shFilePath + dlg_new.m_shFileName;
		CWinApp::OpenDocumentFile(directory);
	}
#endif

}

BOOL CVelmanApp::PreTranslateMessage(MSG* pMsg)
{
	return CWinApp::PreTranslateMessage(pMsg);
}

DWORD CVelmanApp::GetProfileColor(CString section, CString entry, DWORD defaultval)
{
	// helper routine; works like GetProfileInt and reads a color description
	// in the format  red;green;blue

	int r, g, b;
	CString fileentry;

	fileentry = GetProfileString(section, entry);
	if (fileentry == "")
		return defaultval;

	sscanf((const char*)fileentry, "%d;%d;%d", &r, &g, &b);
	return RGB(r, g, b);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buf[256];

	CString version = VELMAN_LONGVERSION;

#ifdef VM64
	version += " 64bit";
#else
	version += " 32bit";
#endif

#ifdef VelmanLight
	version += " Light";
#else
	version += " Full";
#endif

#ifdef DEMO_VERSION
	version += " Demo";
#endif

#ifdef DEBUG
	version += " debug";
#endif

	GetDlgItem(IDC_ABOUTBOX_VERSION)->SetWindowText(version);

	// demo version?
#ifdef ONLY_DEMO_VERSION
	char* lictype = "demo version";
#else
	char* lictype = "fully licensed version";
#endif

	sprintf(buf, "This is a %s version of VelocityManager.", lictype); // changed this now elm is up.
	GetDlgItem(IDC_LICENSE_LICTYPE)->SetWindowText(buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVelmanApp::OnFileNew()
{
	// overridden so that OnFileNew is no longer a protected member function
	CWinApp::OnFileNew();
}