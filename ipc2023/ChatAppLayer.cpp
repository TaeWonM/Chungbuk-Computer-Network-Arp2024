// ChatAppLayer.cpp: implementation of the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "ChatAppLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatAppLayer::CChatAppLayer(char* pName)
	: CBaseLayer(pName),
	mp_Dlg(NULL)
{
	ResetHeader();
}
// ���� ������ ������ CChatAppLayer�� ���� Ŭ�����Դϴ�.

CChatAppLayer::~CChatAppLayer()
{

}

void CChatAppLayer::ResetHeader()
{
	m_sHeader.app_length = 0x0000;
	m_sHeader.app_type = 0x00;
	m_sHeader.app_unused = 0x00;
	m_ackHeader.app_type = 0x04;
	m_ackHeader.app_length = 0x0001;
	m_ackHeader.app_unused = 0x00;
	memset(m_sHeader.app_data, 0, APP_DATA_SIZE);
}
//m_sHeader�� ChatApp ��� �κ� �κ��� �ʱ�ȭ�ϱ� ����, Reset�Լ��Դϴ�.

BOOL CChatAppLayer::Send(unsigned char* ppayload, int nlength)
{
	m_sHeader.app_length = (unsigned short)nlength;
	if (nlength > APP_DATA_SIZE) {
		BOOL bSuccess = TRUE;
		int fragment = nlength / APP_DATA_SIZE + (nlength % (APP_DATA_SIZE) != 0 ? 1 : 0); 
		for (int i = 0; i < fragment; i++) {
			if (i == 0) m_sHeader.app_type = 0x01;
			else if (i == fragment - 1) {
				m_sHeader.app_length = nlength % (APP_DATA_SIZE);
				m_sHeader.app_type = 0x03;
			}
			else {
				m_sHeader.app_length = APP_DATA_SIZE;
				m_sHeader.app_type = 0x02;
			}
			memcpy(m_sHeader.app_data, &ppayload[i * APP_DATA_SIZE], nlength / (APP_DATA_SIZE * (i+1)) > 0 ? APP_DATA_SIZE : nlength % (APP_DATA_SIZE));
			bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, (nlength / (APP_DATA_SIZE * (i+1)) > 0 ? APP_DATA_SIZE : nlength % (APP_DATA_SIZE))+ APP_HEADER_SIZE, 1) && bSuccess;
			Sleep(10);
			mp_aUpperLayer[0]->upperLayerSetTimer(1);
		}
		return bSuccess;
	}
	else {
		BOOL bSuccess = FALSE;
		memcpy(m_sHeader.app_data, ppayload, nlength);
		m_sHeader.app_type = 0x00;
		bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, nlength + APP_HEADER_SIZE, 1);
		mp_aUpperLayer[0]->upperLayerSetTimer(1);
		return bSuccess;
	}
}
// ������ ������ Send �Լ��Դϴ�. ����� ���̸� �����ϰ�, �̸� ppayload�� �ִ� data�� �����Ͽ� �� ����(��, CEthernetLayer)�� Send�� �Լ��� �����մϴ�.

BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{
	// ppayload�� ChatApp ��� ����ü�� �ִ´�.
	PCHAT_APP_HEADER app_hdr = (PCHAT_APP_HEADER)ppayload;
	if (app_hdr->app_type == 0) {
		unsigned char* GetBuff = (unsigned char *)malloc(sizeof(unsigned char) * (app_hdr->app_length + 1));
		memset(GetBuff, '\0', app_hdr->app_length + 1);

		// ���� �������� App Header�� �м��Ͽ�, GetBuff�� data ���̿� APP_DATA_SIZE ���̿� ���Ͽ� ���� ���̸�ŭ
		// data�� �����Ѵ�.
		memcpy(GetBuff, app_hdr->app_data, app_hdr->app_length > APP_DATA_SIZE ? APP_DATA_SIZE : app_hdr->app_length);
		Msg.Format(_T("%s"), (char*)GetBuff);
		delete[] GetBuff;
		mp_aUpperLayer[0]->Receive((unsigned char*)Msg.GetBuffer(0));
		Msg.Empty();
		return TRUE;
	}
	else if (app_hdr->app_type == 1) {
		unsigned char* GetBuff = (unsigned char*)malloc(sizeof(unsigned char) * (APP_DATA_SIZE + 1));
		memset(GetBuff, '\0', APP_DATA_SIZE + 1);
		memcpy(GetBuff, app_hdr->app_data, APP_DATA_SIZE);
		Msg.Format(_T("%s"), (char*)GetBuff);
		return TRUE;
	}
	else if (app_hdr->app_type == 2) {
		unsigned char* GetBuff = (unsigned char*)malloc(sizeof(unsigned char) * (APP_DATA_SIZE + 1));
		memset(GetBuff, '\0', APP_DATA_SIZE + 1);
		memcpy(GetBuff, app_hdr->app_data, APP_DATA_SIZE);
		Msg.AppendFormat(_T("%s"),(char * )GetBuff);
		delete[] GetBuff;
		return TRUE;
	}
	else if (app_hdr->app_type == 3) {
		unsigned char* GetBuff = (unsigned char*)malloc(sizeof(unsigned char) * (app_hdr->app_length + 1));
		memset(GetBuff, '\0', app_hdr->app_length + 1);
		memcpy(GetBuff, app_hdr->app_data, app_hdr->app_length);
		Msg.AppendFormat(_T("%s"), (char*)GetBuff);
		delete[] GetBuff;
		return mp_aUpperLayer[0]->Receive((unsigned char*)Msg.GetBuffer(0));
		Msg.Empty();
		return TRUE;
	}
	else if (app_hdr->app_type == 4) {
		mp_aUpperLayer[0]->upperLayerKillTimer(1);
		return FALSE;
	}
}
// ������ ������ Receive �Լ��Դϴ�. 
// �� �������� ���޹��� ppayload�� PCHAT_APP_HEADER�� �־ ����� ����ϴ�.
// �� ��, �� ������ ������ �ڽſ��� �� �������� Ȯ���� �ϰ�, �̸� Ȯ���ؼ� ���� ���θ� �����մϴ�.
// ������ Dlg�� Receive �Լ��� �Ѱܼ� List�� �����մϴ�.
// �ڽſ��� �� ������ �ƴϸ� ����մϴ�.


BOOL CChatAppLayer::sendAck(unsigned char* ppayload) {
	return mp_UnderLayer->Send((unsigned char*)&m_ackHeader, APP_HEADER_SIZE, 1);
	
}

BOOL CChatAppLayer::Get_Is_Ack() {
	return Is_Ack;
}
void CChatAppLayer::Set_Is_Ack(BOOL value) {
	Is_Ack = value;
}