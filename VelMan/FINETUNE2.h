#pragma once


// FINETUNE2 dialog

class FINETUNE2 : public CDialogEx
{
	DECLARE_DYNAMIC(FINETUNE2)

public:
	FINETUNE2(CWnd* pParent = NULL);   // standard constructor
	virtual ~FINETUNE2();

// Dialog Data
	enum { IDD = IDD_FINETUNE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
