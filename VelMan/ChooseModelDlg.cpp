// choosemo.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "numericr.h"
#include "ChooseModelDlg.h"
#include "CoarseViewDlg.h"
#include "global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelChoiceDlg dialog

CModelChoiceDlg::CModelChoiceDlg(CWnd* pParent /*=NULL*/)
: CDialog(CModelChoiceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelChoiceDlg)
	m_distweight4 = FALSE;
	m_distweight3 = FALSE;
	m_distweight2 = FALSE;
	m_distweight1 = FALSE;
	//}}AFX_DATA_INIT
}

BOOL CModelChoiceDlg::Create()
{
	ResidualTableListOpen=FALSE;
	return CDialog::Create(CModelChoiceDlg::IDD);
}

void CModelChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelChoiceDlg)
	DDX_Check(pDX, IDC_MODELCHOICE_LIN4, m_distweight4);
	DDX_Check(pDX, IDC_MODELCHOICE_LIN3, m_distweight3);
	DDX_Check(pDX, IDC_MODELCHOICE_LIN2, m_distweight2);
	DDX_Check(pDX, IDC_MODELCHOICE_LIN1, m_distweight1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModelChoiceDlg, CDialog)
	//{{AFX_MSG_MAP(CModelChoiceDlg)
	ON_CBN_SELCHANGE(IDC_MODELCHOICE_FUNC1, OnSelchangeModelchoiceFunc1)
	ON_CBN_SELCHANGE(IDC_MODELCHOICE_FUNC2, OnSelchangeModelchoiceFunc2)
	ON_CBN_SELCHANGE(IDC_MODELCHOICE_FUNC3, OnSelchangeModelchoiceFunc3)
	ON_CBN_SELCHANGE(IDC_MODELCHOICE_FUNC4, OnSelchangeModelchoiceFunc4)
	ON_BN_CLICKED(IDC_MODELCHOICE_RESIDUALS, OnModelchoiceResiduals)
	ON_BN_CLICKED(IDC_MODELCHOICE_MODEBTN, OnModelchoiceModebtn)
	ON_BN_CLICKED(IDC_MODELCHOICE_HIDEDLG, OnModelchoiceHidedlg)
	ON_BN_CLICKED(IDC_MODELCHOICE_LIN1, OnSelchangeModelchoiceFunc1)
	ON_BN_CLICKED(IDC_MODELCHOICE_LIN2, OnSelchangeModelchoiceFunc2)
	ON_BN_CLICKED(IDC_MODELCHOICE_LIN3, OnSelchangeModelchoiceFunc3)
	ON_BN_CLICKED(IDC_MODELCHOICE_LIN4, OnSelchangeModelchoiceFunc4)
	ON_BN_CLICKED(IDC_MODELCHOICE_QUAD1, OnSelchangeModelchoiceFunc1)
	ON_BN_CLICKED(IDC_MODELCHOICE_QUAD2, OnSelchangeModelchoiceFunc2)
	ON_BN_CLICKED(IDC_MODELCHOICE_QUAD3, OnSelchangeModelchoiceFunc3)
	ON_BN_CLICKED(IDC_MODELCHOICE_QUAD4, OnSelchangeModelchoiceFunc4)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelChoiceDlg message handlers

void CModelChoiceDlg::OnCancel()
{
	// re-enable the "Model/Create model" menu item
	pDoc->ModelViewOpen=FALSE;
	pDoc->PressCancel=FALSE;

	for(int i=0;i<4;i++)
	{
		delete pDoc->xvals[i];
		delete pDoc->yvals[i];
		delete pDoc->xpos[i];
		delete pDoc->ypos[i];
		delete pDoc->z1vals[i];
		delete pDoc->residuals[i];
		delete pDoc->databaseindex[i];
	}
	// kill the frame window with the four model choices
	(pDoc->pCoarseModelFrame)->DestroyWindow();
	// kill the dialog itself
	DestroyWindow();
	// the dialog object itself was created dynamically, this is the last function that
	// is called after the windows has been removed. We can now destroy the object
	// itself
	delete this;
}

void CModelChoiceDlg::OnOK()
{
	// dummy function; called if user presses RETURN in that dialog
}

void CModelChoiceDlg::CleanUp()
{

	/* this was once done by the dialog. Now CoarseModelView does it itself
	// close any Frames left open
	if(pDoc->OpenPreviewViews & 1)
	pCoarseModelFrame->DestroyWindow();
	pDoc->OpenPreviewViews=0;		// should not really be necessary
	*/
}

BOOL CModelChoiceDlg::OnInitDialog()
{
	// create a small font for the controls and lists
	smallFont.CreateFont(14, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");

	// set all 4 buttons to default weighting option
	// default value is 1
	m_distweight1=AfxGetApp()->GetProfileInt("Well models", "WeightDefault1", 1);
	m_distweight2=AfxGetApp()->GetProfileInt("Well models", "WeightDefault2", 1);
	m_distweight3=AfxGetApp()->GetProfileInt("Well models", "WeightDefault3", 1);
	m_distweight4=AfxGetApp()->GetProfileInt("Well models", "WeightDefault4", 1);
	CDialog::OnInitDialog();

	GetDlgItem(IDC_MODELCHOICE_LIN1)->SetFont(&smallFont);
	GetDlgItem(IDC_MODELCHOICE_LIN2)->SetFont(&smallFont);
	GetDlgItem(IDC_MODELCHOICE_LIN3)->SetFont(&smallFont);
	GetDlgItem(IDC_MODELCHOICE_LIN4)->SetFont(&smallFont);

	CComboBox *pCB1=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC1);
	CComboBox *pCB2=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC2);
	CComboBox *pCB3=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC3);
	CComboBox *pCB4=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC4);
	pCB1->SetFont(&smallFont);
	pCB2->SetFont(&smallFont);
	pCB3->SetFont(&smallFont);
	pCB4->SetFont(&smallFont);
	CWellModelFunction *pMod;

	CStatic *pS=(CStatic *)GetDlgItem(IDC_MODELCHOICE_STATIC);
	pS->SetFont(&smallFont);

	for(int i=0;i<pDoc->GetNumModelFuncs();i++)
	{
		pMod=pDoc->well_model_lib[i];
		pCB1->AddString(pMod->name);
		pCB2->AddString(pMod->name);
		pCB3->AddString(pMod->name);
		pCB4->AddString(pMod->name);
	}
	pCB1->SetCurSel(AfxGetApp()->GetProfileInt("Well models", "FuncDefault1", 0));
	pCB2->SetCurSel(AfxGetApp()->GetProfileInt("Well models", "FuncDefault2", 1));
	pCB3->SetCurSel(AfxGetApp()->GetProfileInt("Well models", "FuncDefault3", 2));
	pCB4->SetCurSel(AfxGetApp()->GetProfileInt("Well models", "FuncDefault4", 3));

	SetDlgItemText(IDC_MODELCHOICE_MODEBTN, "Contour &view");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CModelChoiceDlg::OnSelchangeModelchoiceFunc1()
{
	// change the contents of the corresponding list of data in the document
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC1);
	CButton *pB=(CButton *)GetDlgItem(IDC_MODELCHOICE_LIN1);
	int w=pB->GetCheck() ? 0 : 1;
	AfxGetApp()->DoWaitCursor(1);
	pDoc->FillDataList(0, layer, pCB->GetCurSel(), w);
	AfxGetApp()->DoWaitCursor(-1);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_MODELCHOICE);
}

void CModelChoiceDlg::OnSelchangeModelchoiceFunc2()
{
	// change the contents of the corresponding list of data in the document
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC2);
	CButton *pB=(CButton *)GetDlgItem(IDC_MODELCHOICE_LIN2);
	int w=pB->GetCheck() ? 0 : 1;
	AfxGetApp()->DoWaitCursor(1);
	pDoc->FillDataList(1, layer, pCB->GetCurSel(), w);
	AfxGetApp()->DoWaitCursor(-1);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_MODELCHOICE);
}

void CModelChoiceDlg::OnSelchangeModelchoiceFunc3()
{
	// change the contents of the corresponding list of data in the document
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC3);
	CButton *pB=(CButton *)GetDlgItem(IDC_MODELCHOICE_LIN3);
	int w=pB->GetCheck() ? 0 : 1;
	AfxGetApp()->DoWaitCursor(1);
	pDoc->FillDataList(2, layer, pCB->GetCurSel(), w);
	AfxGetApp()->DoWaitCursor(-1);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_MODELCHOICE);
}

void CModelChoiceDlg::OnSelchangeModelchoiceFunc4()
{
	// change the contents of the corresponding list of data in the document
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_MODELCHOICE_FUNC4);
	CButton *pB=(CButton *)GetDlgItem(IDC_MODELCHOICE_LIN4);
	int w=pB->GetCheck() ? 0 : 1;
	AfxGetApp()->DoWaitCursor(1);
	pDoc->FillDataList(3, layer, pCB->GetCurSel(), w);
	AfxGetApp()->DoWaitCursor(-1);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_MODELCHOICE);
}

void CModelChoiceDlg::OnModelchoiceResiduals()
{
	residual_dlg.horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));
	residual_dlg.pDoc=pDoc;
	ResidualTableListOpen=TRUE;
	residual_dlg.DoModal();
	ResidualTableListOpen=FALSE;
}

void CModelChoiceDlg::OnModelchoiceModebtn()
{
	// flip viewmode
	int m=3-(pDoc->pCMview)->ViewMode;
	(pDoc->pCMview)->ViewMode=m;

	CButton *pB=(CButton *)GetDlgItem(IDC_MODELCHOICE_MODEBTN);
	pB->SetWindowText(m==VIEWMODE_SCATTER ? "Contour &view" : "Scatter &view");
	(pDoc->pCMview)->Invalidate();	// enforce redraw
}

void CModelChoiceDlg::OnModelchoiceHidedlg()
{
	// Hide the dialog and unhide the "show dialog" button in the preview window

	ShowWindow(SW_HIDE);
	(pDoc->pCMview->DlgBackButton).ShowWindow(SW_RESTORE);
}

void CModelChoiceDlg::OnDestroy()
{
	CDialog::OnDestroy();
	smallFont.DeleteObject();
	if(ResidualTableListOpen)
		residual_dlg.DestroyWindow();
}
