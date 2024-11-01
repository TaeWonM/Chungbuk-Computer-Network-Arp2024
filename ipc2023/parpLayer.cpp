// parpLayer.cpp
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "parpLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

parpLayer::parpLayer(char* pName)
	: CBaseLayer(pName),
	mp_Dlg(NULL)
{
	ResetHeader();
}

parpLayer::~parpLayer()
{

}

void parpLayer::ResetHeader()
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

BOOL parpLayer::Send(unsigned char* DstIpAddress, int nlength)
{
	memcpy(m_sHeader.target_IP_address, DstIpAddress, nlength);
	memset(m_sHeader.target_ethernet_address, 255, ETHER_ADDRESS_SIZE);
	mp_UnderLayer[0]->SetMacDstAddress(m_sHeader.target_ethernet_address);
	return mp_UnderLayer[0]->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE, 1);
}


BOOL parpLayer::Receive(unsigned char* ppayload)
{
	P_PARP_HEADER parp = (P_PARP_HEADER)ppayload;
	if (is_IPAddress(parp->target_IP_address)) {
		if (ntohs(parp->op_Code) == 1) {
			// A reply
			memcpy(m_replyHeader.target_IP_address, parp->sender_IP_address, IP_ADDRESS_SIZE);
			memcpy(m_replyHeader.target_ethernet_address, parp->sender_ethernet_address, ETHER_ADDRESS_SIZE);
			memcpy(m_replyHeader.sender_IP_address, parp->target_IP_address, IP_ADDRESS_SIZE);
			memcpy(m_replyHeader.sender_ethernet_address, m_sHeader.sender_ethernet_address, ETHER_ADDRESS_SIZE);
			mp_UnderLayer[0]->SetMacDstAddress(m_replyHeader.target_ethernet_address);
			mp_UnderLayer[0]->Send((unsigned char*)&m_replyHeader, ARP_HEADER_SIZE, 1);
			// B request
			memcpy(m_sHeader.target_IP_address, parp->target_IP_address, IP_ADDRESS_SIZE);
			memset(m_sHeader.target_ethernet_address, 255, ETHER_ADDRESS_SIZE);
			memcpy(m_sHeader.sender_IP_address, parp->sender_IP_address, IP_ADDRESS_SIZE);
			memcpy(m_sHeader.sender_ethernet_address, m_sHeader.sender_ethernet_address, ETHER_ADDRESS_SIZE);
			mp_UnderLayer[0]->SetMacDstAddress(m_sHeader.target_ethernet_address);
			mp_UnderLayer[0]->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE, 1);
		}
	}
	return FALSE;
}

void parpLayer::Set_Sender_Address(unsigned char* MACAddr, unsigned char* IpAddress) {
	memcpy(m_sHeader.sender_ethernet_address, MACAddr, 6);
	memcpy(m_sHeader.sender_IP_address, IpAddress, 4);
}

BOOL parpLayer::is_IPAddress(unsigned char* DstIpAddress) {
	return mp_aUpperLayer[0]->Search_Ip(DstIpAddress);
}