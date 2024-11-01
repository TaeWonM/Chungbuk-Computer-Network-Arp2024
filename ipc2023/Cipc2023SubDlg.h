#pragma once
#include "afxdialogex.h"


// Cipc2023SubDlg 대화 상자

class Cipc2023SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(Cipc2023SubDlg)

public:
	Cipc2023SubDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Cipc2023SubDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
