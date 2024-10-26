
// ipc2023Dlg.h: 헤더 파일
//

#pragma once

#include "LayerManager.h"	// Added by ClassView
#include "ArpLayer.h"	// Added by ClassView
#include "ipLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "CNILayer.h"	// Added by ClassView
#include <pcap.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <Packet32.h>
#pragma comment(lib, "packet.lib")
// Cipc2023Dlg 대화 상자
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
	BOOL			Receive(unsigned char* ppayload);
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
	inline void		SetRegstryMessage();
	LRESULT			OnRegSendMsg(WPARAM wParam, LPARAM lParam);
	LRESULT			OnRegAckMsg(WPARAM wParam, LPARAM lParam);
	unsigned char*  MacAddr2HexInt(CString Mac_address);
	BOOL			m_bSendReady;
	UINT_PTR		TimerHandler;

	// Object App
	ipLayer* m_Ip;
	CEthernetLayer* m_EthernetLayer;
	CNILayer* m_NILayer;
	ArpLayer* m_Arp;	// 추가함
	// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;
public:
	afx_msg void OnBnClickedButtonAddr();
	afx_msg void OnBnClickedButtonSend();
	////////원본과 다름//////////
	CString m_unSrcAddr;
	CString m_unDstAddr;
	// 원본에서는 UINT 타입이었던 변수를 MFC에서 제공한느 문자열
	// 클래스로 바꿈
	/////////////////////////////
	CString m_stMessage;
	CListBox m_ListChat;
	CComboBox m_Combobox;
	CProgressCtrl m_Progress_Bar;
	afx_msg void OnBnClickedCheckToall();
	/////////////////새로 만든 GUI에 대응되는 부분
	afx_msg void OnEnChangeEditDst();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnChangeEditDst2();
	//////////////////////////////////////
	///////////오늘 추가된 부분//////////
	void SetComboboxlist();
	// 콤보박스의 값을 처리하는 함수가 이동되서 추가된 함수
	////////////////////////////////////
public:
	CString m_strFilePath;
	CString m_strFileName;
	inline void		SendFData();	// File 전달용 함수

	afx_msg void OnBnClickedButtonFopen();
	afx_msg void OnBnClickedButtonFsend();
	afx_msg void OnEnChangeEditFpath();
	afx_msg void OnNMCustomdrawProgressFtransfer(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL upperLayerKillTimer(int num);
	BOOL upperLayerSetTimer(int num);
	BOOL SetProgressbar(int max, int cur);
	afx_msg void OnLvnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedItemDeleteBtn();
};
