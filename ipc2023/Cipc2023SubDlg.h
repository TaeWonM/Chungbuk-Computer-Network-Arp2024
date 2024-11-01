#pragma once
#include "afxdialogex.h"
#include "ipc2023Dlg.h"

class Cipc2023Dlg;

// Cipc2023SubDlg 대화 상자
class Cipc2023SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(Cipc2023SubDlg)

public:
	Cipc2023SubDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	Cipc2023Dlg* parent;
	void setter(Cipc2023Dlg* dlg);
	virtual ~Cipc2023SubDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private : 
	CComboBox m_Combobox;
	CIPAddressCtrl m_DST_IP;
public:
	CEdit m_unDstAddr;
	afx_msg void OnCbnSelchangeProxyCombo();
	afx_msg void OnEnChangeProxyEdit();
	afx_msg void OnClose();
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	void SetComboboxlist();
	virtual void OnOK();
	afx_msg void OnBnClickedCancel();
};
