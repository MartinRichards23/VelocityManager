// numericr.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "numericr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumericResiduesDlg dialog

CNumericResiduesDlg::CNumericResiduesDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNumericResiduesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNumericResiduesDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNumericResiduesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNumericResiduesDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CNumericResiduesDlg::Create()
{
	return CDialog::Create(CNumericResiduesDlg::IDD);
}

BEGIN_MESSAGE_MAP(CNumericResiduesDlg, CDialog)
	//{{AFX_MSG_MAP(CNumericResiduesDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumericResiduesDlg message handlers

BOOL CNumericResiduesDlg::OnInitDialog()
{
	// fill listboxes and label various items
	smallFont.CreateFont(14, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	CListBox *pLB;
	double *x, *y, *z1, resi;
	double pa, pb, pv, pk;
	int i, j, win, num;
	char buf[256], buf3[128];
	CWellModelFunction *modfunc;

	CDialog::OnInitDialog();

	for(win=0;win<4;win++)
	{
		modfunc=pDoc->well_model_lib[pDoc->model[win]];
		if(pDoc->model[win]==10)
		{
			// no parameters output if "contoured interval vels"
			strcpy(buf, "The list shows the interval velocities at the wells.");
		}
		else
		{
			if(pDoc->HaveFit[win])
			{
				pa = pDoc->FitA[win];
				pb = pDoc->FitB[win];
				if ( pDoc->model[win]==0 )  // custom fit, needs converting to Vo and K .5/02
				{
					pv = ab2V(pa, pb);
					pk = ab2K(pa, pb);
					sprintf(buf, "%s=%-12.2f\n%s=%-8.4f\nabs. residual=%-5.2f%%",
						(const char *)modfunc->parnameA, pv,
						(const char *)modfunc->parnameB, pk, pDoc->FitChi[win]);
				}
				else
				{
					sprintf(buf, "%s=%-16.12g\n%s=%-16.12g\navg. residual=%-5.2f%%",	// increased precision
						(const char *)modfunc->parnameA, pa,
						(const char *)modfunc->parnameB, pb, pDoc->FitChi[win]);
				}
			}
			else
				sprintf(buf, "(no fit available)");
		}
		strcpy(buf3, (const char *)modfunc->name);
		switch(win)
		{
		case 0:
			pLB=(CListBox *)GetDlgItem(IDC_NUMRES_LIST1);
			SetDlgItemText(IDC_NUMRES_PAR1, buf);
			SetDlgItemText(IDC_NUMRES_FRAME1, buf3);
			GetDlgItem(IDC_NUMRES_FRAME1)->SetFont(&smallFont);
			GetDlgItem(IDC_NUMRES_PAR1)->SetFont(&smallFont);
			break;
		case 1:
			pLB=(CListBox *)GetDlgItem(IDC_NUMRES_LIST2);
			SetDlgItemText(IDC_NUMRES_PAR2, buf);
			SetDlgItemText(IDC_NUMRES_FRAME2, buf3);
			GetDlgItem(IDC_NUMRES_FRAME2)->SetFont(&smallFont);
			GetDlgItem(IDC_NUMRES_PAR2)->SetFont(&smallFont);
			break;
		case 2:
			pLB=(CListBox *)GetDlgItem(IDC_NUMRES_LIST3);
			SetDlgItemText(IDC_NUMRES_PAR3, buf);
			SetDlgItemText(IDC_NUMRES_FRAME3, buf3);
			GetDlgItem(IDC_NUMRES_FRAME3)->SetFont(&smallFont);
			GetDlgItem(IDC_NUMRES_PAR3)->SetFont(&smallFont);
			break;
		case 3:
			pLB=(CListBox *)GetDlgItem(IDC_NUMRES_LIST4);
			SetDlgItemText(IDC_NUMRES_PAR4, buf);
			SetDlgItemText(IDC_NUMRES_FRAME4, buf3);
			GetDlgItem(IDC_NUMRES_FRAME4)->SetFont(&smallFont);
			GetDlgItem(IDC_NUMRES_PAR4)->SetFont(&smallFont);
			break;
		}
		if(pDoc->HaveFit[win] && modfunc->func)
		{
			int TabStopArray[1]={48};
			pLB->SetTabStops(1, TabStopArray);
			x=pDoc->xvals[win];
			y=pDoc->yvals[win];
			z1=pDoc->z1vals[win];
			double a=pDoc->FitA[win];
			double b=pDoc->FitB[win];
			num=pDoc->length[win];
			for(i=0;i<num;i++)
			{
				if(pDoc->model[win]==0)
					resi=(y[i] - z1[i]*exp(x[i]/a) ) - (exp(x[i]/a)-1.0)/b;
				else
					resi=y[i]-modfunc->func(x[i], a, b);
				j=pDoc->databaseindex[win][i];	// to find out the well name we need to
				// the number of the i-th well. If wells before this one were excluded
				// from the dynamic list, that number might be larger than i!
				sprintf(buf, "%s\t%5.3f", (const char *)(pDoc->horizonlist.wellnames[j]), resi);
				pLB->AddString(buf);
			}
		}
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNumericResiduesDlg::OnDestroy()
{
	CDialog::OnDestroy();
	smallFont.DeleteObject();
}
