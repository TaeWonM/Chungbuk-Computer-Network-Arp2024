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
}
//m_sHeader�� ChatApp ��� �κ� �κ��� �ʱ�ȭ�ϱ� ����, Reset�Լ��Դϴ�.

BOOL ArpLayer::Send(unsigned char* DstIpAddress, int nlength)
{
	memcpy(m_sHeader.target_IP_address, DstIpAddress, nlength);
	memset(m_sHeader.target_ethernet_address, 255, ETHER_ADDRESS_SIZE);
	return mp_UnderLayer->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE, 1);
}
// ������ ������ Send �Լ��Դϴ�. ����� ���̸� �����ϰ�, �̸� ppayload�� �ִ� data�� �����Ͽ� �� ����(��, CEthernetLayer)�� Send�� �Լ��� �����մϴ�.


BOOL ArpLayer::Receive(unsigned char* ppayload)
{
	P_ARP_HEADER arp = (P_ARP_HEADER)ppayload;
	if (memcmp(m_sHeader.sender_IP_address, arp->target_IP_address, 4) == 0) {
		if (arp->op_Code == 1) {
			arp->op_Code = 2;
			memcpy(m_replyHeader.target_IP_address, arp->sender_IP_address, IP_ADDRESS_SIZE);
			memcpy(m_replyHeader.target_ethernet_address, arp->sender_ethernet_address, ETHER_ADDRESS_SIZE);
			memcpy(m_replyHeader.sender_IP_address, arp->target_IP_address, IP_ADDRESS_SIZE);
			memcpy(m_replyHeader.target_ethernet_address, arp->target_ethernet_address, ETHER_ADDRESS_SIZE);
			mp_UnderLayer->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE, 1);
		}
		if (arp->op_Code == 2) {
			unsigned char* ppayload = (unsigned char*)malloc(sizeof(unsigned char) * (IP_ADDRESS_SIZE + ETHER_ADDRESS_SIZE));
			memcpy(ppayload, arp->sender_IP_address, IP_ADDRESS_SIZE);
			memcpy(&ppayload[IP_ADDRESS_SIZE], arp->sender_ethernet_address, ETHER_ADDRESS_SIZE);
			mp_aUpperLayer[0]->Receive(ppayload);
		}
	}
	return FALSE;
}

void ArpLayer::Set_Sender_Address(unsigned char* MACAddr, unsigned char *IpAddress) {
	memcpy(m_sHeader.sender_ethernet_address, MACAddr, 6);
	memcpy(m_sHeader.sender_IP_address, IpAddress, 4);
}