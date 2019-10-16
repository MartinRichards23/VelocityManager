// ChooseWell.cpp : implementation file
//

#include "stdafx.h"
#include "Velman.h"
#include "VelmanDoc.h"
#include "ChooseWellDlg.h"
#include "mainfrm.h"

// CChooseWell dialog

IMPLEMENT_DYNAMIC(CChooseWell, CDialog)

CChooseWell::CChooseWell(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseWell::IDD, pParent)
{
	
}

CChooseWell::~CChooseWell()
{
}

void CChooseWell::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChooseWell, CDialog)
END_MESSAGE_MAP()

BOOL CChooseWell::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(dlg_title);
	SetDlgItemText(IDC_LABEL, dlg_message);

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST1);

	if(pDoc->horizonlist.wellnames.GetSize() == 0)
	{
		AfxMessageBox("No wells available", MB_OK | MB_ICONINFORMATION);
		CDialog::OnCancel();
		return TRUE;
	}

	for(int i=0;i < pDoc->horizonlist.wellnames.GetSize();i++)
	{
		pLB->AddString(pDoc->horizonlist.wellnames[i]);
	}

	return TRUE;
}

void CChooseWell::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST1);
	int click[128];

	//get index to remove at
	numgrids=pLB->GetSelCount();
	chosengrids=new int[numgrids];

	if(numgrids < 1)
	{
		AfxMessageBox("No wells were selected", MB_OK | MB_ICONINFORMATION);
		CDialog::OnCancel();
		return;
	}

		pLB->GetSelItems(128, click);

		for(int i = 0; i < numgrids; i++)
		{
			chosengrids[i] = click[i];
		}

		CDialog::OnOK();

}

void CChooseWell::SetDlgTexts(const char *title, const char *message)
{
	dlg_title = title;
	dlg_message = message;
}
