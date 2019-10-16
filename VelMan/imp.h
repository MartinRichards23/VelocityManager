#pragma once


// imp

class imp : public CWnd
{
	DECLARE_DYNAMIC(imp)

public:
	imp();
	virtual ~imp();

	enum { IDD = IDD_GRIDIMPORT };

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};


