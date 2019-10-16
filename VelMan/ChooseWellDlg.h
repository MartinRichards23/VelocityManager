#pragma once

// CChooseWell dialog

class CChooseWell : public CDialog
{
	DECLARE_DYNAMIC(CChooseWell)

public:
	CChooseWell(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	int *chosengrids;
	int numgrids;

	virtual ~CChooseWell();
	void SetDlgTexts(const char *title, const char *message);

// Dialog Data
	enum { IDD = IDD_CHOOSE_ANY_WELL };

private:
	CString dlg_title, dlg_message;

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
