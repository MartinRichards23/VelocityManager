// ConfigForm.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "ConfigForm.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConfigForm dialog

ConfigForm::ConfigForm(CWnd* pParent /*=NULL*/)
: CDialog(ConfigForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(ConfigForm)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void ConfigForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConfigForm)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ConfigForm, CDialog)
	//{{AFX_MSG_MAP(ConfigForm)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConfigForm message handlers

#ifdef VELMAN_UNIX
INT		ConfigForm::DoModal()
#else
INT_PTR ConfigForm::DoModal()
#endif
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::DoModal();
}

int ConfigForm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void ConfigForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// TODO: Add your specialized creation code here
	// Do not call CDialog::OnPaint() for painting messages
}

int ConfigForm::GetBorder(void)
{
	RECT form;
	RECT ok;
	this->GetWindowRect(&form);
	GetDlgItem (IDOK)->GetWindowRect(&ok);

	int i=form.top;
	int j=ok.top;

	int dif = j-i;

	return dif;
}

int ConfigForm::GetSideBorder(void)
{
	RECT form;
	RECT ok;
	this->GetWindowRect(&form);
	GetDlgItem (IDOK)->GetWindowRect(&ok);

	int i=form.left;
	int j=ok.left;

	int dif = j-i;

	return dif;
}

void ConfigForm::OnCancelMode()
{
	CDialog::OnCancelMode();

	// TODO: Add your message handler code here

}
