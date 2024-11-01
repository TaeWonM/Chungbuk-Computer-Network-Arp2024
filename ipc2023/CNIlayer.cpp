// FileLayer.cpp: implementation of the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "CNILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNILayer::CNILayer(char* pName)
	: CBaseLayer(pName)
{
}

CNILayer::~CNILayer()
{
	TRY
	{
		//////////////////////// fill the blank ///////////////////////////////
				CFile::Remove(_T("IpcBuff.txt")); // 파일 제거
		///////////////////////////////////////////////////////////////////////
	}
		CATCH(CFileException, e)
	{
#ifdef _DEBUG
		afxDump << "File cannot be removed\n";
#endif
	}
	END_CATCH
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength)
{
	char errbuf[1001];
	if ((fp = pcap_open_live(m_AdapterList[curAdapterIndex]->name, 65536, 0, 1000, errbuf)) == NULL) {
		AfxMessageBox(_T("Unable to open the adapter"));
		return false;
	}
	if (!send_packet(fp, ppayload, nlength)) {
		AfxMessageBox(_T("ERROR SENDING THE PACKET"));
		return false;
	}

	return true;
}
// 가장 아래 계층인 File 계층의 Send 함수 입니다. IpcBuff.txt라는 파일을 만들어서 적습니다

BOOL CNILayer::Receive()
{
	char errbuf[1001];
	if ((fp = pcap_open_live(m_AdapterList[curAdapterIndex]->name, 65536, 0, 1000, errbuf)) == NULL) {
		AfxMessageBox(_T(errbuf));
		is_set = false;
		return false;
	}
	m_pThread = AfxBeginThread(ReceiveThread, (LPVOID)this);
	return true;
}
// 가장 아래 계층인 File 계층의 Receive 함수 입니다. IpcBuff.txt라는 파일을 읽습니다.
// 그 후, 크기에 맞게 이를 확인하고, 위의 헤더인 Ethernet 계층에 전해줍니다.

BOOL  CNILayer::send_packet(pcap_t* handle, u_char* message, int len) {
	if (pcap_sendpacket(handle, message, len) != 0) {
		AfxMessageBox(_T("Not Send"));
		return false;
	}
	else return true;
}

UINT CNILayer::ReceiveThread(LPVOID pParam) {
	CNILayer* PID = (CNILayer*)pParam;
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	int result;
	while (PID->is_set) {
		result = pcap_next_ex(PID->fp, &header, &pkt_data);
		if (result == 0) {

		}
		else if (result == 1) {
			if (PID->mp_aUpperLayer[0]->Receive((u_char*)pkt_data)) {
			}
		}
	}
	return 0;
}

void CNILayer::SetAdpterDeivce() {
	pcap_if_t* alldevs, * d;
	char errbuff[1001];
	if (pcap_findalldevs(&alldevs, errbuff) == -1 || !alldevs) {
		AfxMessageBox(_T("Any NIC not detected"));
		exit(1);
	}
	for (d = alldevs; d; d = d->next) {
		m_AdapterList[m_Maxadapter] = d;
		m_Maxadapter++;
	}
}

pcap_if_t* CNILayer::GetAdapter(int index) {
	return m_AdapterList[index];
}

int CNILayer::GetMaxAdapterIndex() {
	return m_Maxadapter;
}

void CNILayer::Set_is_set(bool value) {
	is_set = value;
}
bool CNILayer::Get_is_set() {
	return is_set;
}

int CNILayer::GetCurAdapterIndex() {
	return curAdapterIndex;
}
void CNILayer::SetCurAdapterIndex(int value) {
	curAdapterIndex = value;
}