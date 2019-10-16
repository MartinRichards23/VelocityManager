// GAritDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "GridArithDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CGridArithmeticDlg dialog

CGridArithmeticDlg::CGridArithmeticDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGridArithmeticDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridArithmeticDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CGridArithmeticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridArithmeticDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGridArithmeticDlg, CDialog)
	//{{AFX_MSG_MAP(CGridArithmeticDlg)
	ON_EN_CHANGE(IDC_ARITH_EDIT2, OnChangeArithEdit2)
	ON_CBN_SELCHANGE(IDC_ARITH_INPUT1, OnSelchangeArithInput1)
	ON_CBN_SELCHANGE(IDC_ARITH_INPUT2, OnSelchangeArithInput2)
	ON_EN_CHANGE(IDC_ARITH_EDIT1, OnChangeArithEdit1)
	ON_CBN_SELCHANGE(IDC_ARITH_OPERATOR, OnSelchangeArithOperator)
	ON_EN_CHANGE(IDC_ARITH_RESULTNAME, OnChangeArithResultname)
	ON_BN_CLICKED(IDC_ARITH_RADIO1, OnArithRadio)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ARITH_RADIO2, OnArithRadio)
	ON_BN_CLICKED(IDC_ARITH_RADIO3, OnArithRadio)
	ON_BN_CLICKED(IDC_ARITH_RADIO4, OnArithRadio)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridArithmeticDlg message handlers

BOOL CGridArithmeticDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// fill the two listboxes
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_ARITH_INPUT1);
	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
		OnCancel();		// give it up if there are no grids around
	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	pLB=(CListBox *)GetDlgItem(IDC_ARITH_INPUT2);
	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
		OnCancel();		// give it up if there are no grids around
	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	// mark the two "listbox" radio buttons
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO1)))->SetCheck(1);
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO3)))->SetCheck(1);

	// select "-" as operation
	((CComboBox *)GetDlgItem(IDC_ARITH_OPERATOR))->SetCurSel(1);

	AutoResultName=TRUE;	// try to find a result name on your own
	InternalNameChange=FALSE;
	MakeUpName();

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;
}

void CGridArithmeticDlg::OnChangeArithEdit1()
{
	// select appropriate radio button
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO2)))->SetCheck(1);
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO1)))->SetCheck(0);
	MakeUpName();
}

void CGridArithmeticDlg::OnChangeArithEdit2()
{
	// select appropriate radio button
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO4)))->SetCheck(1);
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO3)))->SetCheck(0);
	MakeUpName();
}

void CGridArithmeticDlg::OnSelchangeArithInput1()
{
	// select appropriate radio button
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO1)))->SetCheck(1);
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO2)))->SetCheck(0);
	MakeUpName();
}

void CGridArithmeticDlg::OnSelchangeArithInput2()
{
	// select appropriate radio button
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO3)))->SetCheck(1);
	((CButton *)(GetDlgItem(IDC_ARITH_RADIO4)))->SetCheck(0);
	MakeUpName();
}

void CGridArithmeticDlg::OnOK()
{
	CListBox *pLB;
	int click, i;
	grid1=-1;
	grid2=-1;
	val1=0.0;
	val2=0.0;
	BOOL found1=FALSE;
	BOOL found2=FALSE;
	CString s;

	if(( (CButton *)GetDlgItem(IDC_ARITH_RADIO1))->GetCheck())
	{
		pLB=(CListBox *)GetDlgItem(IDC_ARITH_INPUT1);
		click=pLB->GetCurSel();
		for(i=0;i<entries.GetSize();i++)
		{
			if( ((entries[i])&0xFF) == click)
			{
				grid1=entries[i]>>8;
				found1=TRUE;
				break;
			}
		}
	}
	else
	{
		GetDlgItem(IDC_ARITH_EDIT1)->GetWindowText(s);
		val1=atof((const char *)s);
		found1=TRUE;
	}

	if(( (CButton *)GetDlgItem(IDC_ARITH_RADIO3))->GetCheck())
	{
		pLB=(CListBox *)GetDlgItem(IDC_ARITH_INPUT2);
		click=pLB->GetCurSel();
		for(i=0;i<entries.GetSize();i++)
		{
			if( ((entries[i])&0xFF) == click)
			{
				grid2=entries[i]>>8;
				found2=TRUE;
				break;
			}
		}
	}
	else
	{
		GetDlgItem(IDC_ARITH_EDIT2)->GetWindowText(s);
		val2=atof((const char *)s);
		found2=TRUE;
	}

	operation=((CComboBox *)GetDlgItem(IDC_ARITH_OPERATOR))->GetCurSel();

	// if no grid name was declared, but the stupid user still deactivated "auto-naming"
	// we just reactivate the "auto"-mode and make up a name
	CEdit *pE=(CEdit *)GetDlgItem(IDC_ARITH_RESULTNAME);
	if(!AutoResultName && pE->GetWindowTextLength()==0)
	{
		AutoResultName=TRUE;
		MakeUpName();
	}
	pE->GetWindowText(resultname);

	if(found1 && found2)
		CDialog::OnOK();
}

void CGridArithmeticDlg::MakeUpName()
{
	// invent a name for the resulting grid, unless told by the flag not to do so
	if(!AutoResultName)
		return;

	CEdit *pE=(CEdit *)GetDlgItem(IDC_ARITH_RESULTNAME);
	CString buf, buf2;
	CListBox *pLB;
	int i = 0;

	// name of first operand
	if(( (CButton *)GetDlgItem(IDC_ARITH_RADIO1))->GetCheck())
	{
		pLB=(CListBox *)GetDlgItem(IDC_ARITH_INPUT1);
		pLB->GetText(pLB->GetCurSel(), buf);
		i=buf.Find("\t");
		if(i>-1)
			buf=buf.Mid(i+1);
	}
	else
		GetDlgItem(IDC_ARITH_EDIT1)->GetWindowText(buf);

	// name of operator
	char *ops[]={"+", "-", "*", "/" };
	buf+=ops[((CComboBox *)GetDlgItem(IDC_ARITH_OPERATOR))->GetCurSel()];

	// name of second operand
	if(( (CButton *)GetDlgItem(IDC_ARITH_RADIO3))->GetCheck())
	{
		pLB=(CListBox *)GetDlgItem(IDC_ARITH_INPUT2);
		pLB->GetText(pLB->GetCurSel(), buf2);
		i=buf2.Find("\t");
		if(i>-1)
			buf2=buf2.Mid(i+1);
	}
	else
		GetDlgItem(IDC_ARITH_EDIT2)->GetWindowText(buf2);
	buf+=buf2;

	InternalNameChange=TRUE;
	pE->SetWindowText((const char *)buf);
}

void CGridArithmeticDlg::OnSelchangeArithOperator()
{
	MakeUpName();
}

void CGridArithmeticDlg::OnChangeArithResultname()
{
	if(!InternalNameChange)
		AutoResultName=FALSE;
	InternalNameChange=FALSE;
}

void CGridArithmeticDlg::OnArithRadio()
{
	MakeUpName();
}

void CGridArithmeticDlg::OnDestroy()
{
	smallFont.DeleteObject();
	CDialog::OnDestroy();
}
