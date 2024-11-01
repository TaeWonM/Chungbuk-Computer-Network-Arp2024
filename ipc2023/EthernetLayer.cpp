// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#define ARP_LAYER 1 // �߰���
#define FILE_APP_LAYER 2 // �߰���

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
}
// ������ CEthernetLayer�� ���� Ŭ�����Դϴ�.

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
}
//m_sHeader�� Ethernet ��� �κ��� �ʱ�ȭ�ϱ� ����, Reset�Լ��Դϴ�.

unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}
//enet_srcaddr�� �����ϴ� getter�Դϴ�.

unsigned char* CEthernetLayer::GetDestinAddress()
{
	//////////////////////// fill the blank ///////////////////////////////
	// Ethernet ������ �ּ� return
	return m_sHeader.enet_dstaddr;
	///////////////////////////////////////////////////////////////////////
}

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{
	//////////////////////// fill the blank ///////////////////////////////
		// �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);
	///////////////////////////////////////////////////////////////////////
}
//enet_srcaddr�� �����ϴ� setter�Դϴ�.

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}
//enet_dstaddr�� �����ϴ� setter�Դϴ�.
//--------------------------------------------------- ���� 2024.10.13.-------------------------------------

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength, int DetLayer)
{
	memcpy(m_sHeader.enet_data, ppayload, nlength);
	BOOL bSuccess = FALSE;
	// ACK ó��: ä�� ������ ������ ���
	if (DetLayer == ARP_LAYER) {
		m_sHeader.enet_type = htons(0x0806);
		bSuccess = mp_UnderLayer[0]->Send((unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);
	}
	else if (DetLayer == FILE_APP_LAYER) {
		bSuccess = mp_UnderLayer[0]->Send((unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);
	}
	else {
		return FALSE;
	}

	return bSuccess;
}
// Ethernet ������ Send �Լ��Դϴ�. ����� ���̸� �����ϰ�, �̸� ppayload�� �ִ� data�� �����Ͽ� �� ����(��, CFileLayer)�� Send�� �Լ��� �����մϴ�.
//---------------------------------------------------- ������� ---------------------------------------

//--------------------------------------------------- ���� 2024.10.13.-------------------------------------

BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	BOOL bSuccess = FALSE;
	//////////////////////// fill the blank ///////////////////////////////
		// Chatapp or Fileapp layer�� Ethernet Frame�� data�� �Ѱ��ش�
		// ������ ������ Ÿ�Ե鿡 ���ؼ��� ����
	if ((memcmp(pFrame->enet_dstaddr, m_sHeader.enet_srcaddr, 6) == 0 ||
		(memcmp(pFrame->enet_dstaddr, BROADCASTING_ADDR, 6) == 0 && memcmp(pFrame->enet_srcaddr, m_sHeader.enet_srcaddr, 6) != 0)) &&
		(ntohs(pFrame->enet_type) == 0x0806 || ntohs(pFrame->enet_type) == 0x2081))
	{
		if (memcmp(pFrame->enet_dstaddr, BROADCASTING_ADDR, 6) == 0) is_Broadcast = true;
		else memcpy(m_ReceivedDstAddr, pFrame->enet_srcaddr, 6);

		if (ntohs(pFrame->enet_type) == 0x0806) { // ä�� Ÿ���̸� Chatapplayer�� �ø�
			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pFrame->enet_data);
		}
		else if (ntohs(pFrame->enet_type) == 0x2081) { // ���� Ÿ���̸� Fileapplayer�� �ø�
			bSuccess = mp_aUpperLayer[1]->Receive((unsigned char*)pFrame->enet_data);
		}
	}
	///////////////////////////////////////////////////////////////////////

	return bSuccess;
}
// Ethernet ������ Receive �Լ��Դϴ�. 
// �� �������� ���޹��� ppayload�� PETHERNET_HEADER�� �־ ����� ����ϴ�.
// �� ��, ���� Ȯ���� ���� �ʰ�, �ٷ� ���� ������ ChatApp ������ �����մϴ�.
//---------------------------------------------------- ������� ---------------------------------------

//--------------------------------------------------- ���� 2024.10.13.-------------------------------------
//---------------------------------------------------- ������� ---------------------------------------

void CEthernetLayer::SetBroadcasting_address() {
	memset(BROADCASTING_ADDR, 255, 6);
}

BOOL CEthernetLayer::SetMacDstAddress(unsigned char* ppayload) {
	memcpy(m_sHeader.enet_dstaddr, ppayload, ETHER_ADDRESS_SIZE);
	return TRUE;
}
BOOL CEthernetLayer::SetMacSrcAddress(unsigned char* ppayload) {
	memcpy(m_sHeader.enet_srcaddr, ppayload, ETHER_ADDRESS_SIZE);
	return TRUE;
}