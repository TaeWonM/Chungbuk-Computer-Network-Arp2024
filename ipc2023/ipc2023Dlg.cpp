// ipc2023Dlg.cpp: 구현 파일
//
// 새로이 해야 할 부분 : 소스 IP가 빈 상태로 set 하면 메세지 띄우기

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "pch.h"
#include "framework.h"
#include "ipc2023.h"
#include "ipc2023Dlg.h"
#include "afxdialogex.h"
#include <tchar.h>
////////////////OCT.11added///////////
#include <afxdlgs.h> //안에 포함된 CFileDialog사용하기 위해 추가
//////////////////////////////////////

#define LIST_CONTROLL_IP_COLUMN 1 
#define LIST_CONTROLL_MAC_COLUMN 2 
#define LIST_CONTROLL_STATUS_COLUMN 3


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();	// cAboutDlg의 접근 수준은 public으로 외부접근 가능

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원하여 데이터의 전송 및 검증에 이용
	// MFC에서의 데이터 바인딩 수행

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()	// 메세지 맵을 이용해 MFC 상의 UI에서 사용자가 버튼클릭이나 값 입력 등 
		// 사용자 이용에 대한 응답을 정의한다.
public:

};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
// 클래스의 생성자 -> 대화상자의 초기화에 이용

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // 데이터 전송 및 검증 과정 
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	// 사용자가 원하는 기능의 메세지 처리기 추가 가능 -> 새로운 입력 칸, 버튼...

END_MESSAGE_MAP()
// 메세제 맵의 시작과 끝


// Cipc2023Dlg 대화 상자



Cipc2023Dlg::Cipc2023Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IPC2023_DIALOG, pParent)
	// 부모 클래스의 생성자 호출 및 대화 상자 ID 설정
	, CBaseLayer("ChatDlg")
	// CBaseLayer 의 생성자 호출 후 ChatDlg라는 이름으로 초기화
	, m_bSendReady(FALSE)
	// 전송 준비 상태 표현 변수를 bool값 FALSE로 초기화
	, m_nAckReady(-1)
	// ACK 준비 상태 표현 변수를 -1로 초기화

	/////////////////원본과 차이 있음//////////////////
	, m_unSrcAddr("")
	// Source address 빈문자열로 초기화
	// Destination address 빈문자열로 초기화
	// 주소 자체에 문자가 들어가기에 문자열로 타입 자체를 변경함
	// 그래서 초기화 시 공백으로 하는거
	/////////////////////////////////////////////
	, m_stMessage(_T(""))
	//메세지 문자열을 빈문자열로 초기화
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Protocol Layer Setting
	m_LayerMgr.AddLayer(new ipLayer("Ip")); // ChatAppLayer 추가
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet")); // EthernetLayer 추가
	m_LayerMgr.AddLayer(new CNILayer("NI"));
	m_LayerMgr.AddLayer(new ArpLayer("Arp")); // 10.11 FileAppLayer 추가
	// file 레이어를 NILAyer로 변경
	// 여기서 이렇게 설정해줘서 다른 레이어 시작 시 해당하는 클래스 생성하면
	// 자동으로 pName에 이름이 넘어가는 것
	m_LayerMgr.AddLayer(this);
	// 대화 상자 레이어 추가

	// 레이어를 연결한다. (레이어 생성)
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *Arp ( *Ip ( *ChatDlg ) ) ) ( *Ip ( *ChatDlg ) ) )");
	// 기존에 file 레이어 부분 대신 NI 레이어를 추가함
	////////////////////////추가됨///////////////////////////////
	m_Ip = (ipLayer*)m_LayerMgr.GetLayer("Ip");
	m_EthernetLayer = (CEthernetLayer*)m_LayerMgr.GetLayer("Ethernet");
	// 이더넷 레이어를 직접 참조하도록 코드 추가함
	m_NILayer = (CNILayer*)m_LayerMgr.GetLayer("NI");
	// NI 레이어를 직접 참조하도록 코드 추가함
	//Protocol Layer Setting
	// 위에서 추가한 레이어를 특정 변수에 저장하여 사용
	// 10.11 추가함
	m_Arp = (ArpLayer*)m_LayerMgr.GetLayer("Arp");
}

void Cipc2023Dlg::DoDataExchange(CDataExchange* pDX)
// 부모클래스의 DoDataExchange 호출 -> 데이터 전송 및 검증
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SRC, m_unSrcAddr);
	////////////////////새로 추가/////////////////
	DDX_Control(pDX, IDC_COMBO1, m_Combobox);
	DDX_Control(pDX, IDC_LIST3, m_ListControl);
	DDX_Control(pDX, IDC_DST_IP, m_DstIp);
	DDX_Control(pDX, IDC_SRC_IP, m_SrcIp);
	// 현재 장치의 네트워크 장치를 보여줄 콤보박스 추가함
	// 여러 네트워크 장치를 보여주기 위해 콤보박스 채용
}
// UI상에 구현된 항복들의 각 멤벼변수에 해당하는 컨트롤과 멤버변수 간의 연결 수행


BEGIN_MESSAGE_MAP(Cipc2023Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDR, &Cipc2023Dlg::OnBnClickedButtonAddr)
	// 주소 설정 버튼 클릭 처리기 등록
	ON_BN_CLICKED(IDC_BUTTON_SEND, &Cipc2023Dlg::OnBnClickedButtonSend)
	// 메세지 전송 버튼 클릭 처리기 등록
	ON_WM_TIMER() // 메세지 타이머 처리기 등록


	//////////////새로 추가된 부분////////////////////////////////////////
	// 소스 주소 변경 시 처리기 등록
	ON_CBN_SELCHANGE(IDC_COMBO1, &Cipc2023Dlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_ITEM_DELETE_BTN, &Cipc2023Dlg::OnBnClickedItemDeleteBtn)
	ON_BN_CLICKED(IDC_ALL_DELETE_BTN, &Cipc2023Dlg::OnBnClickedAllDeleteBtn)
<<<<<<< HEAD
	ON_EN_CHANGE(IDC_EDIT_GARP, &Cipc2023Dlg::OnEnChangeEditGarp)
	ON_BN_CLICKED(IDC_GARP_BUTTON_SEND, &Cipc2023Dlg::OnBnClickedGarpButtonSend)
=======
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &Cipc2023Dlg::OnLvnItemchangedList2)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_DST_IP, &Cipc2023Dlg::OnIpnFieldchangedDstIp)
	//ON_NOTIFY(IPN_FIELDCHANGED, IDC_SRC_IP, &Cipc2023Dlg::OnIpnFieldchangedSrcIp)
>>>>>>> sanghyun_branch
END_MESSAGE_MAP()


// Cipc2023Dlg 메시지 처리기

BOOL Cipc2023Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 대화 상자의 초기화 작업 수행

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	// IDM_ABOUTBOX가 시스템 명령 범위 안에 있는지 체크
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE); // 메뉴 핸들 호출
	if (pSysMenu != nullptr)  // 메뉴 핸들이 유효할 경우
	{
		BOOL bNameValid;   // 이름의 유효성 검사 수행
		CString strAboutMenu;
		// "정보..." 메뉴 항목 저장할 CString 변수 선언
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX); //리소스에 문자열 로드
		ASSERT(bNameValid); //문자열 로드 성공여부 확인
		if (!strAboutMenu.IsEmpty())  // 로드할 문자열이 비어있지 않다면,
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetDlgState(IPC_INITIALIZING);
	// 대화 상자의 초기상태를 TNITIALIZING으로 초기화

	/////////오늘 추가된 부분//////////////////////
	m_NILayer->SetAdpterDeivce();
	m_EthernetLayer->SetBroadcasting_address();
	SetComboboxlist();
	InitListControlSet();
	memset(timerIndex, -1, 4 * 10);
	//원래는 여기서 송수신 주소 처리했는데 NI레이어로 함수 이동하고 여기서는 전달만 해줌
	/////////////////////////////////////////////////////////////////////////
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
// 검색된 모든 네트워크 장치를 콤보 박스 안에 추가 후 출력

/////////오늘 추가된 부분//////////////////////
void Cipc2023Dlg::SetComboboxlist() {
	int max = m_NILayer->GetMaxAdapterIndex();
	for (int i = 0; i < max; i++) {
		pcap_if_t* d = m_NILayer->GetAdapter(i);
		m_Combobox.AddString(_T(d->description));
	}
}
//원래는 여기서 송수신 주소 처리했는데 NI레이어로 함수 이동하고 여기서는 전달만 해줌
/////////////////////////////////////////////////////////////////////////

void Cipc2023Dlg::OnSysCommand(UINT nID, LPARAM lParam)
// 시스템의 명령 메세지를 처리하는 함수 정의
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		// 시스템 명령이 "정보..." 인지 확인
	{
		CAboutDlg dlgAbout; // CAboutDlg 클래스의 인스턴스 생성
		dlgAbout.DoModal(); // 모달 대화 상자로 "정보..." 대화 상자 출력
		// 모달 대화 상자: 대화상자를 종료하기 전까지 다른 행동 불가
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void Cipc2023Dlg::OnPaint()
{
	if (IsIconic())
		// 창의 아이콘화 상태 확인 -> 아이콘화
		// 현재 창이 최소화되어 아이콘으로 표시되었는지
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR Cipc2023Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void Cipc2023Dlg::OnBnClickedButtonSend()
{
	UpdateData(TRUE); // 대화 상자의 컨트롤에서 데이터를 가져와 멤벼 변수에 등록
	SendData();	// 데이터를 전송하는 함수의 멤버 변수 호촐
	UpdateData(FALSE); // 멤벼 변수의 값을 대화 상자의 컨트롤에 등록
}
// send button 이 눌러졌을 때 작동하는 함수입니다.
// 메시지가 존재 하였을 경우 타이머를 설정하고, 기존에 있던 메시지를 초기화합니다.
// 또한, Send 신호를 브로드캐스트로 알립니다.


void Cipc2023Dlg::SendData()
{
	CString unDstIpAddrStr; // 메세지 헤더를 저장할 문자열 변수 선언
	unsigned char Ip1, Ip2, Ip3, Ip4;
	m_DstIp.GetAddress(Ip1, Ip2, Ip3, Ip4);
	unsigned char unDst_Ip_Address[] = { Ip1, Ip2, Ip3, Ip4 };
	unDstIpAddrStr.Format(_T("%d.%d.%d.%d"), unDst_Ip_Address[0], unDst_Ip_Address[1], unDst_Ip_Address[2], unDst_Ip_Address[3]);
	if (Ip1 == 0 && Ip2 == 0 && Ip3 == 0 && Ip4 == 0) {
		AfxMessageBox("No Address Set..");
		return;
	}
	else {
		int i = 0;
		for (i = 0; i < m_ListControl.GetItemCount(); i++) {
			if (m_ListControl.GetItemText(i, LIST_CONTROLL_IP_COLUMN) == unDstIpAddrStr) {
				if (m_ListControl.GetItemText(i, LIST_CONTROLL_STATUS_COLUMN) == "complete") {
					int k;
					for (k = 0; k < timerMaxIndex; k++) {
						if (i == timerIndex[k]) break;
					}
					SetTimer(k, 30000, NULL);
					AfxMessageBox(_T("Already Set.."));
					return;
				}
				else {
					m_Ip->Send(unDst_Ip_Address, 4);
					return;
				}
			}
		}
		m_ListControl.InsertItem(i, "");
		m_ListControl.SetItemText(i, LIST_CONTROLL_IP_COLUMN, unDstIpAddrStr);
		m_ListControl.SetItemText(i, LIST_CONTROLL_STATUS_COLUMN, _T("Incomplete"));
		SetTimer(i, 3000, NULL);
		timerIndex[i] = i;
		timerMaxIndex++;
		m_Ip->Send(unDst_Ip_Address, 4);
	}
}
// Send 버튼이 눌리면 다음의 함수를 실행합니다.
// 일단 m_unDstAddr가 ff(즉, 브로딩캐스트 Addr)인지 확인을 하고, 확인되면 m_unSrcAddr와 함께 수신자의 chatDlg에 나타날 문자를 MsgHeader에 포멧팅합니다.
// m_unDstAddr가 ff(즉, 브로딩캐스트 Addr)가 아니면 기존의 m_unSrcAddr과 m_unDstAddr를 이용하여 수신자의 chatDlg에 나타날 문자를 MsgHeader에 포멧팅합니다.
// 그 후 메시지를 저장하고, Dlg계층의 하위 계층인 ChatAppLayer의 Send함수로 넘겨줍니다.

BOOL Cipc2023Dlg::Receive(CString IpAddr, CString MacAddr, BOOL is_In)
{
	/////////////////////////////////수정됨//////////////////////////////////////////
	if (is_In) {
		int i = 0;
		for (i = 0; i < m_ListControl.GetItemCount(); i++) {
			if (m_ListControl.GetItemText(i, LIST_CONTROLL_IP_COLUMN) == IpAddr) {
				m_ListControl.SetItemText(i, LIST_CONTROLL_MAC_COLUMN, MacAddr);
				m_ListControl.SetItemText(i, LIST_CONTROLL_STATUS_COLUMN, _T("complete"));
				int k;
				for (k = 0; k < timerMaxIndex; k++) {
					if (i == timerIndex[k]) break;
				}
				SetTimer(k, 30000, NULL);
			}
			return FALSE;
		}
		return FALSE;
	}
	else {
		int i = m_ListControl.GetItemCount();
		m_ListControl.InsertItem(i, "");
		m_ListControl.SetItemText(i, LIST_CONTROLL_IP_COLUMN, IpAddr);
		m_ListControl.SetItemText(i, 2, MacAddr);
		m_ListControl.SetItemText(i, LIST_CONTROLL_STATUS_COLUMN, _T("complete"));
		for (int k = 0; k < timerMaxIndex; k++) {
			if (timerIndex[k] <= -1) {
				SetTimer(k, 30000, NULL);
				timerIndex[k] = i;
				return FALSE;
			}
		}
		SetTimer(i, 30000, NULL);
		timerIndex[i] = i;
		timerMaxIndex++;
		//////////////////////////////////////////////////////////////////////////////////
		return FALSE;
	}
}
// ppayload를 인수로 받으며 이 부분을 받으면 List 부분에 받은 ppayload를 넣어 사용자의 Dlg에 보이도록 합니다.

BOOL Cipc2023Dlg::PreTranslateMessage(MSG* pMsg)
// 메세지가 호출되기 전에 입력된 메세지를 통해 특정 작업 수행
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
	case WM_KEYDOWN: // 엔터 키가 눌린 경우
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			if (::GetDlgCtrlID(::GetFocus()) == IDC_EDIT3)
				OnBnClickedButtonSend();
			// 현재 포커스가 IDC_EDIT3 인지 확인하고 맞다면, Send버튼 처리 함수인 OnBnClickedButtonSend() 호출
			return FALSE;
		case VK_ESCAPE: return FALSE;
		}
		break;
		// return FALSE를 통해 프로그램의 기본 동작으로 특정 작업 간에 수행한 작업이 전파되지 않도록 예방
	}

	return CDialog::PreTranslateMessage(pMsg);
	// 위의 조건에 해당하지 않는 메세지는 PreTranslateMessage(pMsg)로 전달하여 일반적인 처리과정 거침
}


void Cipc2023Dlg::SetDlgState(int state)
{
	UpdateData(TRUE);

	// 각 UI 요소에 대한 포인터 가져옴
	CButton* pChkButton = (CButton*)GetDlgItem(IDC_CHECK1);

	CButton* pSendButton = (CButton*)GetDlgItem(bt_send);
	CButton* pSetAddrButton = (CButton*)GetDlgItem(bt_setting);
	CButton* pitemDeleteButton = (CButton*)GetDlgItem(IDC_ITEM_DELETE_BTN);
	CButton* pallDeleteButton = (CButton*)GetDlgItem(IDC_ALL_DELETE_BTN);
	///////////////////////////////////////////////////////////
	// 상태에 따른 UI 요소 설정값 변경
	switch (state)
	{
	case IPC_INITIALIZING: // 초기화 상태, 추가한 버튼 3개 다 처음엔 비활성
		pSendButton->EnableWindow(FALSE);
		m_ListControl.EnableWindow(TRUE);
		////////여기서 삭제 버튼 두개 다 비활성, 목적지 입력 비활성
		m_DstIp.EnableWindow(FALSE);
		pitemDeleteButton->EnableWindow(FALSE);
		pallDeleteButton->EnableWindow(FALSE);
		/////////////////////////////초기에 GARP관련 비활성 상태로 되게 설정
		GetDlgItem(IDC_EDIT_GARP)->EnableWindow(FALSE);
		GetDlgItem(IDC_GARP_BUTTON_SEND)->EnableWindow(FALSE);
		////////////////////////////////////////
		break;
	case IPC_READYTOSEND: // 전송 준비 상태, 추가한 버튼 중 파일선택 버튼이랑 주소 입력칸만 활성화
		pSendButton->EnableWindow(TRUE);
		m_ListControl.EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_GARP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GARP_BUTTON_SEND)->EnableWindow(TRUE);
		break;
	case IPC_WAITFORACK:	break; // 수신 대기 상태
	case IPC_ERROR:		break; // 에러 상태
	case IPC_UNICASTMODE: // 유니캐스트 모드
		break;
	case IPC_BROADCASTMODE: // 브로드캐스트 모드
		break;
		// 브로드캐스트와 유니캐스트에 대한 작동 방식 변경됨(삭제)
		// 다른곳에서 처리해서 여기서 제거
	case IPC_ADDR_SET: // 주소 설정 상태
		pSetAddrButton->SetWindowText(_T("재설정(&R)"));
		//pChkButton->EnableWindow(FALSE)
		m_Combobox.EnableWindow(FALSE);
		////////////////오늘 변경됨//////////////////////
		m_NILayer->Set_is_set(true);
		m_SrcIp.EnableWindow(FALSE);
		/////////////////////////////////////////////
		// NI레이어의 Set_is_set true 로 설정 (추가됨)
		if (!m_NILayer->Receive()) SetDlgState(IPC_ADDR_RESET);
		// NI레이어의 receive가 false인 경우 SetDlgState(IPC_ADDR_RESET)을 진행
		// 여기서 삭제 버튼 활성화, 상대 주소 입력되게 하기
		m_DstIp.EnableWindow(TRUE);
		pitemDeleteButton->EnableWindow(TRUE);
		pallDeleteButton->EnableWindow(TRUE);
		//////////////////////////////////////////
		break;
	case IPC_ADDR_RESET: // 주소 재설정 상태
		pSetAddrButton->SetWindowText(_T("설정(&O)"));
		m_Combobox.EnableWindow(TRUE);
		// 네트워크 장치 콤보박스에 다시 접근할 수 있도록 설정
		///////////////오늘 변경됨/////////////////////
		m_NILayer->Set_is_set(false);
		m_SrcIp.EnableWindow(TRUE);//재설정하면 소스 IP 입력할 수 있게 설정
		// NI레이어의 Set_is_set false 로 설정 (추가됨)
		/////////////////////////////////////////////
		// is_set 플래그 false로 설정(추가됨)
		// 기존 코드에서 브로드캐스트 체크박스 관련 코드 삭제
		m_DstIp.EnableWindow(FALSE);
		pitemDeleteButton->EnableWindow(FALSE);
		pallDeleteButton->EnableWindow(FALSE);
		// 다시 버튼 2개 비활성, 목적지 비활성, 내 ip 칸 활성
		break;
	}

	UpdateData(FALSE);
}

// Dlg의 상태를 한 함수에 정의하여 작동을 하는 부분입니다. 
// 특히 IPC_BROADCASTMODE로 변경이 되면 DstAddr를 쓰는 부분이 FALSE가 되어 쓸 수 없고, m_unDstAddr가 ff로 바뀌는 것을 볼 수 있습니다.


void Cipc2023Dlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer(); // 레이어 매니저에서 할당된 레이어를 해제
}


void Cipc2023Dlg::OnTimer(UINT nIDEvent)
{
	if (timerIndex[nIDEvent] <= -1) {
		KillTimer(nIDEvent);
	}
	else {
		KillTimer(nIDEvent);
		m_Ip->RemoveItem(m_ListControl.GetItemText(timerIndex[nIDEvent], 1), m_ListControl.GetItemText(timerIndex[nIDEvent], 2));
		m_ListControl.DeleteItem(timerIndex[nIDEvent]);
		timerIndex[nIDEvent] = -1;
		for (int k = nIDEvent + 1; k < timerMaxIndex; k++) {
			timerIndex[k]--;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
// 타이머가 시간이 끝나면 ListDlg에 타임아웃 메시지를 띄웁니다.
// 동시에 타이머를 죽이고, m_nAckReady를 -1로 설정하여서 Recive부분의 if문에 걸려 확인하는 부분입니다.

void Cipc2023Dlg::OnBnClickedButtonAddr()
{
	UpdateData(TRUE);

	if ((m_unSrcAddr.IsEmpty()))
		// 자기 자신을 목적지로 설정 할 수 없게 함(추가)
	{
		AfxMessageBox(_T("주소를 설정 오류발생",
			"경고"),
			MB_OK | MB_ICONSTOP); // 수신자, 목적지 주소가 비어있는 경우 설정한 오류 메세지 출력

		return;
	}
	// 전송 준비 확인
	if (m_bSendReady) {
		SetDlgState(IPC_ADDR_RESET);
		SetDlgState(IPC_INITIALIZING);
	}
	else {
		////////////////////////////////////추가된 부분///////////////////////////////////
		unsigned char* SrcAddr = MacAddr2HexInt(m_unSrcAddr);
		if (SrcAddr == nullptr) {
			return;
		}
		// MacAddr2HexInt함수의 소스 주소 또는 목적지 주소가 잘못 설정되어 
		// nullptr을 반환한 경우 실행을 중단하도록 추가
		m_EthernetLayer->SetSourceAddress(SrcAddr);
		unsigned char IpAddress[4];
		m_SrcIp.GetAddress(IpAddress[0], IpAddress[1], IpAddress[2], IpAddress[3]);
		m_Arp->Set_Sender_Address(SrcAddr, IpAddress);
		// 이더넷 레이어에서 선언한 SetSourceAddress 함수를 가져와 사용하고 있음
		// dlg 헤더 파일을 보면 m_EthernetLayer가 CEthernetLayer* 즉 포인터임을
		// 확인 가능함
		// MacAddr2HexInt는 여기 아래에 정의된 함수임
		// 16진수로 맥 주소를 변환하는 함수다
		/////////////////////////////////////////////////////////////////////////////////

		SetDlgState(IPC_ADDR_SET);
		SetDlgState(IPC_READYTOSEND);
	}
	// MAC 주소를 16진수로 변환하여 송신, 수신 측 주소를 설정하고 설정 준비 상태로 변경
	m_bSendReady = !m_bSendReady;
}
// 주소 설정 버튼를 눌렀을 때 예외 및 설정하는 함수입니다.
// m_unDstAddr 혹은 m_unSrcAddr가 0이면 오류를 설정합니다.
// 초기에는 m_bSendReady가 False로 설정되어 있습니다. 
// 그래서 OnBnClickedButtonAddr이 오류 없이 눌리면 SetDlgState를 이용해 IPC_ADDR_SET와 IPC_READYTOSEND로 바꾸고
// m_bSendReady을 True로 바꿉니다. 또한, 다시한면 눌리면 IPC_ADDR_RESET과 IPC_INITIALIZING를 해 다시 보낼 수 있도록 재시작을 합니다.

// 체크 박스 상태에 따른 전송 모드 처리. 체크 되어있으면 브로드캐스트, 안되어있으면 유니캐스트로 처리
// BROADCAST 버튼이 눌렸을 때 SetDlgState을 이용해 IPC_BROADCASTMODE를 설정하거나 IPC_UNICASTMODE을 이용해 IPC_BROADCASTMODE를 해제합니다.


///아래는 전부 추가된 부분//////////
void Cipc2023Dlg::OnCbnSelchangeCombo1()
{
	bpf_u_int32 net, mask;	// 네트워크 주소와 마스크 주소 담을 변수 설정
	char errbuf[101];	// 오류 메세지 담을 버퍼 설정
	PPACKET_OID_DATA OidData;	// OID 데이터 담을 포인터 설정
	LPADAPTER adapter = NULL;	// 어뎁터 포인터 설정

	// OID 구조체 메모리 설정(추가됨)
	OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PPACKET_OID_DATA));
	OidData->Oid = OID_802_3_CURRENT_ADDRESS;	// OID 설정
	OidData->Length = 6;	// OID 크기 길이 설정


	int i = 0;
	int indexnum = m_Combobox.GetCurSel();	// 콤보 박스에서 선택한 네트워크 장치의 인덱스 가져오기
	//오늘 변경된 부분///////////////////////////////////////////////////////
	m_NILayer->SetCurAdapterIndex(indexnum);	// 현재 어뎁터의 인덱스 설정
	if (pcap_lookupnet(m_NILayer->GetAdapter(indexnum)->name, &net, &mask, errbuf) < 0) {
		return;
		printf("error");
	} // 현재 선택한 어뎁터의 네트워크 주소와 마스크 주소 조회
	adapter = PacketOpenAdapter(m_NILayer->GetAdapter(indexnum)->name);
	/////////////////// getter함수들로 변경됨 직접 참조 막아서 그럼////////////
	if (!adapter || adapter->hFile == INVALID_HANDLE_VALUE) {
		return;
	}
	// 어뎁터 열기 실패하면 함수 종료

	PacketRequest(adapter, FALSE, OidData);// MAC 주소 가져오기

	// MAC 주소를 문자열로 포멧
	CString Address_mssage = "";
	Address_mssage.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
		OidData->Data[0], OidData->Data[1], OidData->Data[2],
		OidData->Data[3], OidData->Data[4], OidData->Data[5]);
	SetDlgItemText(IDC_EDIT_SRC, Address_mssage); // 포멧된 MAC주소를 송신자 주소로 설정
	PacketCloseAdapter(adapter); // 어뎁터 닫기
}



unsigned char* Cipc2023Dlg::MacAddr2HexInt(CString Mac_address)
{
	CString TempToken; // MAC 주소의 각 부분을 저장할 변수 설정
	unsigned char* ether = (unsigned char*)malloc(sizeof(unsigned char) * 6);

	// MAC 주소를 ':' 로 구분하여 16진수 변환
	for (int i = 0; i < 6; i++) {
		if (AfxExtractSubString(TempToken, Mac_address, i, ':')) {
			ether[i] = (unsigned char)strtoul(TempToken.GetString(), NULL, 16);
		}
		else {
			AfxMessageBox(_T("주소를 설정 오류발생",
				"경고"),
				MB_OK | MB_ICONSTOP);
			free(ether);
			return nullptr;
			// 주소 설정 시 오류가 발생하면 오류 메세지 출력하고 할당된 메모리 해제 및 NULL 반환
		}
	}
	ether[6] = '\0';  // 종료 문자 추가 '\0'

	return ether;	// 변환된 MAC 주소 반환
}

unsigned char* Cipc2023Dlg::IpAddr2HexInt(CString Ip_address)
{
	CString TempToken; // MAC 주소의 각 부분을 저장할 변수 설정
	unsigned char* ether = (unsigned char*)malloc(sizeof(unsigned char) * 4);

	// MAC 주소를 ':' 로 구분하여 16진수 변환
	for (int i = 0; i < 4; i++) {
		if (AfxExtractSubString(TempToken, Ip_address, i, '.')) {
			ether[i] = (unsigned char)strtoul(TempToken.GetString(), NULL, 10);
		}
		else {
			AfxMessageBox(_T("주소를 설정 오류발생",
				"경고"),
				MB_OK | MB_ICONSTOP);
			free(ether);
			return nullptr;
			// 주소 설정 시 오류가 발생하면 오류 메세지 출력하고 할당된 메모리 해제 및 NULL 반환
		}
	}
	ether[4] = '\0';  // 종료 문자 추가 '\0'

	return ether;	// 변환된 MAC 주소 반환
}

void Cipc2023Dlg::InitListControlSet()
{
	CRect r;
	::GetClientRect(m_ListControl.m_hWnd, r);
	int cx = r.right - r.left;
	CString column[] = { "", "IP Address", "Ethernet Address", "Status" };
	float nColWidth[] = { 0, 0.2, 0.5, 0.3 };
	for (int i = 0; i < 4; i++) {
		m_ListControl.InsertColumn(i, column[i], LVCFMT_CENTER, int(cx * nColWidth[i]));
	}

	m_ListControl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}


void Cipc2023Dlg::OnBnClickedItemDeleteBtn()
{
	int nmark = m_ListControl.GetSelectionMark();
	if (nmark != -1) {
		m_ListControl.DeleteItem(nmark);
		for (int i = nmark + 1; i < timerMaxIndex; i++) {
			timerIndex[i]--;
		}
	}
}


void Cipc2023Dlg::OnBnClickedAllDeleteBtn()
{
	m_ListControl.DeleteAllItems();
	for (int i = 0; i < timerMaxIndex; i++) {
		timerIndex[i] = -1;
	}
}





void Cipc2023Dlg::OnEnChangeEditGarp()
{
	GetDlgItemText(IDC_EDIT_GARP, m_unSrcAddr);
}


void Cipc2023Dlg::OnBnClickedGarpButtonSend()
{
	unsigned char* macAddr = MacAddr2HexInt(m_unSrcAddr);

	if (macAddr == nullptr) {
		AfxMessageBox(_T("Invalid MAC address format."));
		return;
	}

	// MAC 주소 복사 및 설정

	m_EthernetLayer->SetSourceAddress(macAddr);  // 복사된 MAC 주소 전달

	// GARP 전송
	if (m_Arp != nullptr) {
		m_Arp->SendGARP(macAddr);
		AfxMessageBox(_T("GARP request sent successfully."));
	}
	else {
		AfxMessageBox(_T("ArpLayer not initialized."));
	}

<<<<<<< HEAD
}
=======
//void Cipc2023Dlg::OnIpnFieldchangedSrcIp(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	*pResult = 0;
//}
>>>>>>> sanghyun_branch
