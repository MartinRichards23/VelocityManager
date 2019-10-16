#pragma once

// ImpWellHorData dialog

class ImpWellHorData : public CDialog
{
	DECLARE_DYNAMIC(ImpWellHorData)

public:
	ImpWellHorData(CWnd* pParent = NULL);   // standard constructor
	virtual ~ImpWellHorData();
	CVelmanDoc *pDoc;
	CString InitialDir;
	CString tdPath;

	// Dialog Data
	enum { IDD = IDD_IMP_WELL_HOR_DATA };

private:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonsingle();
};
