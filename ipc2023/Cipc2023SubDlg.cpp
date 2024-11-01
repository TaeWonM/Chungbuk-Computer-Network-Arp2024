// Cipc2023SubDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ipc2023.h"
#include "afxdialogex.h"
#include "Cipc2023SubDlg.h"
#include "ipc2023Dlg.h"


#define PROXY_LIST_CONTROLL_DEVICE_ID 1 
#define PROXY_LIST_CONTROLL_IP_ADDRESS 2 
#define PROXY_LIST_CONTROLL_MAC_ADDRESS 3



// Cipc2023SubDlg 대화 상자

IMPLEMENT_DYNAMIC(Cipc2023SubDlg, CDialogEx)

Cipc2023SubDlg::Cipc2023SubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
}

Cipc2023SubDlg::~Cipc2023SubDlg()
{
}

void Cipc2023SubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROXY_EDIT, m_unDstAddr);
	DDX_Control(pDX, IDC_PROXY_COMBO, m_Combobox);
	DDX_Control(pDX, IDC_PROXY_DST_IP, m_DST_IP);
}


BEGIN_MESSAGE_MAP(Cipc2023SubDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_PROXY_COMBO, &Cipc2023SubDlg::OnCbnSelchangeProxyCombo)
	ON_EN_CHANGE(IDC_PROXY_EDIT, &Cipc2023SubDlg::OnEnChangeProxyEdit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &Cipc2023SubDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// Cipc2023SubDlg 메시지 처리기



void Cipc2023SubDlg::OnCbnSelchangeProxyCombo()
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
	if (pcap_lookupnet(parent->m_NILayer->GetAdapter(indexnum)->name, &net, &mask, errbuf) < 0) {
		return;
		printf("error");
	} // 현재 선택한 어뎁터의 네트워크 주소와 마스크 주소 조회
	adapter = PacketOpenAdapter(parent->m_NILayer->GetAdapter(indexnum)->name);
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
	SetDlgItemText(IDC_PROXT_SRC_EDIT, Address_mssage); // 포멧된 MAC주소를 송신자 주소로 설정
	PacketCloseAdapter(adapter); // 어뎁터 닫기
}



void Cipc2023SubDlg::OnEnChangeProxyEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void Cipc2023SubDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialogEx::OnClose();
	DestroyWindow();
}

void Cipc2023SubDlg::setter(Cipc2023Dlg* dlg){
	parent = dlg;
}

void Cipc2023SubDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	parent->ProxyDlg = NULL;
	delete this;
	CDialogEx::PostNcDestroy();
}


BOOL Cipc2023SubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetComboboxlist();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void Cipc2023SubDlg::SetComboboxlist() {
	int max = parent->m_NILayer->GetMaxAdapterIndex();
	for (int i = 0; i < max; i++) {
		pcap_if_t* d = parent->m_NILayer->GetAdapter(i);
		m_Combobox.AddString(_T(d->description));
	}
}

void Cipc2023SubDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	unsigned char Ip1, Ip2, Ip3, Ip4;
	m_DST_IP.GetAddress(Ip1, Ip2, Ip3, Ip4);
	unsigned char unDst_Ip_Address[] = { Ip1, Ip2, Ip3, Ip4 };
	CString unDstIpAddrStr;
	unDstIpAddrStr.Format(_T("%d.%d.%d.%d"), unDst_Ip_Address[0], unDst_Ip_Address[1], unDst_Ip_Address[2], unDst_Ip_Address[3]);
	CString MacAddr;
	m_unDstAddr.GetWindowTextA(MacAddr);
	if (MacAddr.IsEmpty()) {
		AfxMessageBox(_T("주소를 설정 오류발생",
			"경고"),
			MB_OK | MB_ICONSTOP); // 수신자, 목적지 주소가 비어있는 경우 설정한 오류 메세지 출력
		return;
	}
	unsigned char* SrcAddr = parent->MacAddr2HexInt(MacAddr);
	if (SrcAddr == nullptr) {
		return;
	}
	CString deviceID;
	deviceID.Format(_T("%d"), m_Combobox.GetCurSel());
	int ItemIndex = parent->m_ProxyListControl.GetItemCount();
	parent->m_ProxyListControl.InsertItem(ItemIndex, "");
	parent->m_ProxyListControl.SetItemText(ItemIndex, PROXY_LIST_CONTROLL_DEVICE_ID, deviceID);
	parent->m_ProxyListControl.SetItemText(ItemIndex, PROXY_LIST_CONTROLL_IP_ADDRESS, unDstIpAddrStr);
	parent->m_ProxyListControl.SetItemText(ItemIndex, PROXY_LIST_CONTROLL_MAC_ADDRESS, MacAddr);
	parent->m_Ip->AddProxyItem(unDstIpAddrStr, MacAddr);
	CDialogEx::OnOK();
	PostNcDestroy();
}


void Cipc2023SubDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
	PostNcDestroy();
}
