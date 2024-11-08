// ipc2023Dlg.h: 헤더 파일
//

#pragma once

#include "LayerManager.h"	// Added by ClassView
#include "ArpLayer.h"	// Added by ClassView
#include "ipLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "CNILayer.h"	// Added by ClassView
#include "Cipc2023SubDlg.h"
#include <pcap.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <Packet32.h>
#pragma comment(lib, "packet.lib")
// Cipc2023Dlg 대화 상자
class Cipc2023SubDlg;

class Cipc2023Dlg : public CDialogEx, public CBaseLayer
{
	// 생성입니다.
public:
	Cipc2023Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.



	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPC2023_DIALOG };
#endif

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//	UINT m_unDstAddr;
	//	UINT unSrcAddr;
	//	CString m_stMessage;
	//	CListBox m_ListChat;

	afx_msg void Cipc2023Dlg::OnTimer(UINT nIDEvent);


public:
	BOOL			Receive(CString IpAddr, CString MacAddr, BOOL is_in);
	inline void		SendData();

private:
	CLayerManager	m_LayerMgr;
	int				m_nAckReady;

	enum {
		IPC_INITIALIZING,
		IPC_READYTOSEND,
		IPC_WAITFORACK,
		IPC_ERROR,
		IPC_BROADCASTMODE,
		IPC_UNICASTMODE,
		IPC_ADDR_SET,
		IPC_ADDR_RESET
	};

	void			SetDlgState(int state);
	inline void		EndofProcess();
	BOOL			m_bSendReady;
	UINT_PTR		TimerHandler;

	// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;
public:
	// Object App
	ipLayer* m_Ip;
	CEthernetLayer* m_EthernetLayer;
	CNILayer* m_NILayer;
	ArpLayer* m_Arp;	// 추가함
	unsigned char* MacAddr2HexInt(CString Mac_address);
	unsigned char* IpAddr2HexInt(CString Ip_address);
	afx_msg void OnBnClickedButtonAddr();
	afx_msg void OnBnClickedButtonSend();
	////////원본과 다름//////////
	CString m_unSrcAddr;
	CString m_unGarpAddr;
	// 원본에서는 UINT 타입이었던 변수를 MFC에서 제공한느 문자열
	// 클래스로 바꿈
	/////////////////////////////
	CString m_stMessage;
	CComboBox m_Combobox;
	CListCtrl m_ListControl;
	CListCtrl m_ProxyListControl;
	CIPAddressCtrl m_SrcIp;
	CIPAddressCtrl m_DstIp;
	Cipc2023SubDlg * ProxyDlg;
	int timerIndex[101];
	int timerMaxIndex = 0;
	afx_msg void OnCbnSelchangeCombo1();
	//////////////////////////////////////
	///////////오늘 추가된 부분//////////
	void SetComboboxlist();
	// 콤보박스의 값을 처리하는 함수가 이동되서 추가된 함수
	////////////////////////////////////
private:
	CString m_strMacAddr;
public:

	/*BOOL upperLayerKillTimer(int num);
	BOOL upperLayerSetTimer(int num);
	afx_msg void OnBnClickedButton2();*/
	void InitListControlSet();
	void InitProxyListControlSet();
	BOOL UpdateArpCahe(unsigned char* ipAddr, unsigned char* macAddr);
	afx_msg void OnBnClickedItemDeleteBtn();
	afx_msg void OnBnClickedAllDeleteBtn();
	afx_msg void OnLvnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIpnFieldchangedDstIp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedProxyDeleteBtn();
	afx_msg void OnBnClickedProxyItemAddBtn();
	afx_msg void OnEnChangeEditGarp();
	afx_msg void OnBnClickedGarpButtonSend();
};
