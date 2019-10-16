// ImpWellHorData.cpp : implementation file
//

#include "stdafx.h"
#include "Velman.h"
#include "VelmanDoc.h"
#include "ImpWellHorDataDlg.h"

// ImpWellHorData dialog

IMPLEMENT_DYNAMIC(ImpWellHorData, CDialog)

	ImpWellHorData::ImpWellHorData(CWnd* pParent /*=NULL*/)
	: CDialog(ImpWellHorData::IDD, pParent)
{

}

ImpWellHorData::~ImpWellHorData()
{
}

BOOL ImpWellHorData::OnInitDialog()
{
	if(pDoc->ProjState & PRJ_HAVEWELLS)
	{
		AfxMessageBox("You have already imported well data. This step cannot be redone."
			" If you have to re-import data, you have to start a new project.");

		OnCancel();
		return false;
	}

	CWnd::CheckDlgButton(IDC_RADIO3, 1);
	CWnd::CheckDlgButton(IDC_CHECK1, 1);
	return true;
}

void ImpWellHorData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ImpWellHorData, CDialog)
	ON_BN_CLICKED(IDOK, &ImpWellHorData::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTONSINGLE, &ImpWellHorData::OnBnClickedButtonsingle)
END_MESSAGE_MAP()

// ImpWellHorData message handlers

void ImpWellHorData::OnBnClickedButtonsingle()
{
	// new method to read data
	CFileDialog td_dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Spreadsheet file (*.csv;)|*.csv;||");
	td_dlg.m_ofn.lpstrTitle="Import well data";
	td_dlg.m_ofn.lpstrInitialDir = (LPCTSTR)pDoc->TargetDir;

	if(td_dlg.DoModal()!=IDOK)
		return;

	tdPath = td_dlg.GetPathName();

	((CStatic *)GetDlgItem(IDC_STATIC4))->SetWindowTextA((LPCTSTR)tdPath);
}

void ImpWellHorData::OnBnClickedOk()
{
	if(tdPath == "")
	{
		AfxMessageBox( "Please select the well data file or cancel", MB_OK | MB_ICONINFORMATION);
		return;
	}

	// set file to load
	pDoc->WellTiDp = tdPath;
	pDoc->WellSurv = "Unused";

	if(IsDlgButtonChecked(IDC_CHECK2))
	{
		pDoc->UsePseudoVelocities = Pseudo_All;
	}
	else
	{
		int pseudoDeviated = AfxGetApp()->GetProfileInt("Well Import", "PseudoDeviated", 1);

		if(pseudoDeviated == 1)
			pDoc->UsePseudoVelocities = Pseudo_Deviated;
		else
			pDoc->UsePseudoVelocities = Pseudo_None;
	}


	if(IsDlgButtonChecked(IDC_CHECK1))
		pDoc->VerticaliseWells = FALSE;
	else
		pDoc->VerticaliseWells = TRUE;

	OnOK();
}