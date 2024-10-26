﻿// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#define CHAT_APP_LAYER 1 // �߰���
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
	memset(m_sCHeader.enet_dstaddr, 0, 6);
	memset(m_sCHeader.enet_srcaddr, 0, 6);
	memset(m_sCHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sCHeader.enet_type = htons(0x2080);
	memset(m_sFHeader.enet_dstaddr, 0, 6);
	memset(m_sFHeader.enet_srcaddr, 0, 6);
	memset(m_sFHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sFHeader.enet_type = htons(0x2081);
}
//m_sHeader�� Ethernet ��� �κ��� �ʱ�ȭ�ϱ� ����, Reset�Լ��Դϴ�.

unsigned char* CEthernetLayer::GetCSourceAddress()
{
	return m_sCHeader.enet_srcaddr;
}
//enet_srcaddr�� �����ϴ� getter�Դϴ�.

unsigned char* CEthernetLayer::GetCDestinAddress()
{
	//////////////////////// fill the blank ///////////////////////////////
	// Ethernet ������ �ּ� return
	return m_sCHeader.enet_dstaddr;
	///////////////////////////////////////////////////////////////////////
}
unsigned char* CEthernetLayer::GetFSourceAddress()
{
	return m_sFHeader.enet_srcaddr;
}
//enet_srcaddr�� �����ϴ� getter�Դϴ�.

unsigned char* CEthernetLayer::GetFDestinAddress()
{
	//////////////////////// fill the blank ///////////////////////////////
	// Ethernet ������ �ּ� return
	return m_sFHeader.enet_dstaddr;
	///////////////////////////////////////////////////////////////////////
}
//enet_dstaddr�� �����ϴ� getter�Դϴ�.

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{
	//////////////////////// fill the blank ///////////////////////////////
		// �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sCHeader.enet_srcaddr, pAddress, 6);
	memcpy(m_sFHeader.enet_srcaddr, pAddress, 6);
	memcpy(m_ackHeader.enet_srcaddr, pAddress, 6);
	///////////////////////////////////////////////////////////////////////
}
//enet_srcaddr�� �����ϴ� setter�Դϴ�.

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	memcpy(m_sCHeader.enet_dstaddr, pAddress, 6);
	memcpy(m_sFHeader.enet_dstaddr, pAddress, 6);
}
//enet_dstaddr�� �����ϴ� setter�Դϴ�.
//--------------------------------------------------- ���� 2024.10.13.-------------------------------------

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength, int DetLayer)
{
	BOOL bSuccess = FALSE;
	// ACK ó��: ä�� ������ ������ ���
	if ((DetLayer == CHAT_APP_LAYER && nlength == APP_HEADER_SIZE)||(DetLayer == FILE_APP_LAYER && nlength == FILE_HEADER_SIZE)) {
		memcpy(m_ackHeader.enet_data, ppayload, nlength);
		return mp_UnderLayer->Send((unsigned char*)&m_ackHeader, nlength + ETHER_HEADER_SIZE);
	}

	// ������ ��ó�� ���� ���� Ÿ�� ����
	if (DetLayer == CHAT_APP_LAYER) {
		memcpy(m_sCHeader.enet_data, ppayload, nlength); // ä�� Ÿ������ ����
	}
	else if (DetLayer == FILE_APP_LAYER) {
		memcpy(m_sFHeader.enet_data, ppayload, nlength); // ���� Ÿ������ ����
	}
	else {
		return FALSE;
	}

	// Ethernet ������ ������ ����

	// Ethernet Data + Ethernet Header�� ����� ���� ũ�⸸ŭ�� Ethernet Frame��
	// �ش� ���̾�� ������.
	if (DetLayer == CHAT_APP_LAYER) {
		bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sCHeader, nlength + ETHER_HEADER_SIZE);
	}
	else if (DetLayer == FILE_APP_LAYER) {
		bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sFHeader, nlength + ETHER_HEADER_SIZE);
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
	if ((memcmp(pFrame->enet_dstaddr, m_sCHeader.enet_srcaddr, 6) == 0 ||
		(memcmp(pFrame->enet_dstaddr, BROADCASTING_ADDR, 6) == 0 && memcmp(pFrame->enet_srcaddr, m_sCHeader.enet_srcaddr, 6) != 0)) &&
		(ntohs(pFrame->enet_type) == 0x2080 || ntohs(pFrame->enet_type) == 0x2081))
	{
		if (memcmp(pFrame->enet_dstaddr, BROADCASTING_ADDR, 6) == 0) is_Broadcast = true;
		else memcpy(m_ReceivedDstAddr, pFrame->enet_srcaddr, 6);

		if (ntohs(pFrame->enet_type) == 0x2080) { // ä�� Ÿ���̸� Chatapplayer�� �ø�
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
BOOL CEthernetLayer::sendAck(unsigned char* ppayload) {
	PETHERNET_HEADER m_preHeader = (PETHERNET_HEADER)ppayload;
	BOOL ackSuccess = FALSE;
	memcpy(m_ackHeader.enet_dstaddr, m_preHeader->enet_srcaddr, 6);
	if (ntohs(m_preHeader->enet_type) == 0x2080) { // ä�� Ÿ���� ���
		m_ackHeader.enet_type = htons(0x2080); // Ack�� Ÿ�Ե� ä�� Ÿ�԰� ���� �����ϰ� Chatapplayer�� Ack ����
		ackSuccess = mp_aUpperLayer[0]->sendAck((unsigned char*)m_ackHeader.enet_data);
	}
	else if (ntohs(m_preHeader->enet_type) == 0x2081) { // ���� Ÿ���� ���
		m_ackHeader.enet_type = htons(0x2081); // Ack�� Ÿ�Ե� ���� Ÿ�԰� ���� �����ϰ� Fileapplayer�� Ack ���� 
		ackSuccess = mp_aUpperLayer[1]->sendAck((unsigned char*)m_ackHeader.enet_data);
	}
	return ackSuccess;
}
//---------------------------------------------------- ������� ---------------------------------------

void CEthernetLayer::SetBroadcasting_address() {
	memset(BROADCASTING_ADDR, 255, 6);
}