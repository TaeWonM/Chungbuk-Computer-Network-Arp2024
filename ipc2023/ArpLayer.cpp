// ChatAppLayer.cpp: implementation of the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "ArpLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ArpLayer::ArpLayer(char* pName)
	: CBaseLayer(pName),
	mp_Dlg(NULL)
{
	ResetHeader();
}
// ���� ������ ������ CChatAppLayer�� ���� Ŭ�����Դϴ�.

ArpLayer::~ArpLayer()
{

}

void ArpLayer::ResetHeader()
{
	m_sHeader.hard_type = htons(0x0001);
	m_sHeader.portocal_type = htons(0x0800);
	m_sHeader.hard_size = 6;
	m_sHeader.portocal_size = 4;
	m_sHeader.op_Code = htons(0x0001);
	memset(m_sHeader.sender_ethernet_address, 0, ETHER_ADDRESS_SIZE);
	memset(m_sHeader.sender_IP_address, 0, IP_ADDRESS_SIZE);
	memset(m_sHeader.target_ethernet_address, 0, ETHER_ADDRESS_SIZE);
	memset(m_sHeader.target_IP_address, 0, IP_ADDRESS_SIZE);
	m_replyHeader.hard_type = htons(0x0001);
	m_replyHeader.portocal_type = htons(0x0800);
	m_replyHeader.hard_size = 6;
	m_replyHeader.portocal_size = 4;
	m_replyHeader.op_Code = htons(0x0002);
	memset(m_replyHeader.sender_ethernet_address, 0, ETHER_ADDRESS_SIZE);
	memset(m_replyHeader.sender_IP_address, 0, IP_ADDRESS_SIZE);
	memset(m_replyHeader.target_ethernet_address, 0, ETHER_ADDRESS_SIZE);
	memset(m_replyHeader.target_IP_address, 0, IP_ADDRESS_SIZE);
	memset(BroadingCastAddr, 255, ETHER_ADDRESS_SIZE);
}
//m_sHeader�� ChatApp ��� �κ� �κ��� �ʱ�ȭ�ϱ� ����, Reset�Լ��Դϴ�.


BOOL ArpLayer::Send(unsigned char* DstIpAddress, int nlength)
{
	memcpy(m_sHeader.sender_ethernet_address, m_macAddr, ETHER_ADDRESS_SIZE);
	memcpy(m_sHeader.sender_IP_address, m_ipAddr, IP_ADDRESS_SIZE);
	memcpy(m_sHeader.target_IP_address, DstIpAddress, nlength);
	memset(m_sHeader.target_ethernet_address, 255, ETHER_ADDRESS_SIZE);
	mp_UnderLayer[0]->SetMacDstAddress(m_sHeader.target_ethernet_address);
	mp_UnderLayer[0]->SetMacSrcAddress(m_sHeader.sender_ethernet_address);
	return mp_UnderLayer[0]->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE, 1);
}
// ������ ������ Send �Լ��Դϴ�. ����� ���̸� �����ϰ�, �̸� ppayload�� �ִ� data�� �����Ͽ� �� ����(��, CEthernetLayer)�� Send�� �Լ��� �����մϴ�.


BOOL ArpLayer::Receive(unsigned char* ppayload) {
	BOOL is_in = FALSE;
	P_ARP_HEADER arp = (P_ARP_HEADER)ppayload;
	// GARP 패킷인지 확인: 송신자의 IP 주소 = IP 주소, 대상 MAC 주소 = 브로드캐스트 주소인지 확인
	if (memcmp(arp->target_IP_address, arp->sender_IP_address, 4) == 0) {
		if (memcmp(arp->target_IP_address, m_ipAddr, 4) == 0) {
			if (ntohs(arp->op_Code) == 1) {
				memcpy(m_replyHeader.target_IP_address, m_ipAddr, IP_ADDRESS_SIZE);// 대상 ip주소 = 송신자의 ip주소
				memcpy(m_replyHeader.target_ethernet_address, BroadingCastAddr, ETHER_ADDRESS_SIZE);// 대상 이더넷 주소 = 송신자의 이더넷 주소
				memcpy(m_replyHeader.sender_IP_address, m_ipAddr, IP_ADDRESS_SIZE);// 송신 ip 주소 = 자신의 ip주소
				memcpy(m_replyHeader.sender_ethernet_address, m_macAddr, ETHER_ADDRESS_SIZE); // 송신 이더넷 주소 = 자신의 이더넷 주소
				mp_UnderLayer[0]->SetMacDstAddress(m_replyHeader.target_ethernet_address);
				mp_UnderLayer[0]->Send((unsigned char*)&m_replyHeader, ARP_HEADER_SIZE, 1);
			}
			else {
				AfxMessageBox(_T("Duplicated GARP!!"));
			}
			return TRUE;
		}
		// ARP 캐시 업데이트
		mp_aUpperLayer[0]->UpdateArpCahe(arp->sender_IP_address, arp->sender_ethernet_address);

		return TRUE; // GARP 패킷 처리 완료
	}

	if (memcmp(m_ipAddr, arp->target_IP_address, 4) == 0) {
		if (ntohs(arp->op_Code) == 1) {
			memcpy(m_replyHeader.target_IP_address, arp->sender_IP_address, IP_ADDRESS_SIZE);// 대상 ip주소 = 송신자의 ip주소
			memcpy(m_replyHeader.target_ethernet_address, arp->sender_ethernet_address, ETHER_ADDRESS_SIZE);// 대상 이더넷 주소 = 송신자의 이더넷 주소
			memcpy(m_replyHeader.sender_IP_address, m_ipAddr, IP_ADDRESS_SIZE);// 송신 ip 주소 = 자신의 ip주소
			memcpy(m_replyHeader.sender_ethernet_address, m_macAddr, ETHER_ADDRESS_SIZE); // 송신 이더넷 주소 = 자신의 이더넷 주소
			mp_UnderLayer[0]->SetMacDstAddress(m_replyHeader.target_ethernet_address);
			mp_UnderLayer[0]->Send((unsigned char*)&m_replyHeader, ARP_HEADER_SIZE, 1);
		}
		is_in = TRUE;
	}
	unsigned char* payload;
	if (is_in) {
		payload = (unsigned char*)malloc(sizeof(unsigned char) * (IP_ADDRESS_SIZE + ETHER_ADDRESS_SIZE));
		memcpy(payload, arp->sender_IP_address, IP_ADDRESS_SIZE);
		memcpy(&payload[IP_ADDRESS_SIZE], arp->sender_ethernet_address, ETHER_ADDRESS_SIZE);
	}
	else {
		payload = (unsigned char*)malloc(sizeof(unsigned char) * (IP_ADDRESS_SIZE + ETHER_ADDRESS_SIZE));
		memcpy(payload, arp->target_IP_address, IP_ADDRESS_SIZE);
		memcpy(&payload[IP_ADDRESS_SIZE], arp->target_ethernet_address, ETHER_ADDRESS_SIZE);
	}
	if (mp_aUpperLayer[0]->Receive(payload, is_in) && !is_in) {
		memcpy(m_replyHeader.target_IP_address, arp->sender_IP_address, IP_ADDRESS_SIZE);// 대상 ip주소 = 송신자의 ip주소
		memcpy(m_replyHeader.target_ethernet_address, arp->sender_ethernet_address, ETHER_ADDRESS_SIZE);// 대상 이더넷 주소 = 송신자의 이더넷 주소
		memcpy(m_replyHeader.sender_IP_address, arp->target_IP_address, IP_ADDRESS_SIZE);// 송신 ip 주소 = 자신의 ip주소
		memcpy(m_replyHeader.sender_ethernet_address, m_macAddr, ETHER_ADDRESS_SIZE); // 송신 이더넷 주소 = 자신의 이더넷 주소
		mp_UnderLayer[0]->SetMacDstAddress(m_replyHeader.target_ethernet_address);
		mp_UnderLayer[0]->Send((unsigned char*)&m_replyHeader, ARP_HEADER_SIZE, 1);
	}
	free(payload);
	return FALSE;
}

void ArpLayer::Set_Sender_Address(unsigned char* MACAddr, unsigned char *IpAddress) {
	Set_Ip_Address(IpAddress);
	Set_Mac_Address(MACAddr);
}

BOOL ArpLayer::SendGARP(const unsigned char* macAddr) {
    // Broadcast 주소로 대상 MAC 주소 설정
    memset(m_sHeader.target_ethernet_address, 255, ETHER_ADDRESS_SIZE); // Broadcast MAC 주소 설정
	
	mp_UnderLayer[0]->SetMacDstAddress(m_sHeader.target_ethernet_address);
    // 자신의 IP를 대상 IP로 설정하여 GARP 생성
    memcpy(m_sHeader.target_IP_address, m_ipAddr, IP_ADDRESS_SIZE); // 자신의 IP로 설정
	memcpy(m_sHeader.sender_IP_address, m_ipAddr, IP_ADDRESS_SIZE); // 자신의 IP로 설정

    // 전달받은 MAC 주소를 ARP 헤더의 송신 MAC 주소로 설정
    memcpy(m_sHeader.sender_ethernet_address, macAddr, ETHER_ADDRESS_SIZE); // ARP 헤더의 송신 MAC 주소 설정

    // EthernetLayer를 통해 GARP 패킷 전송
    return mp_UnderLayer[0]->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE,1);
}


void ArpLayer::Set_Mac_Address(unsigned char* MACAddr) {
	memcpy(m_macAddr, MACAddr , 6);
}
void ArpLayer::Set_Ip_Address(unsigned char* IpAddr) {
	memcpy(m_ipAddr, IpAddr, 4);
}