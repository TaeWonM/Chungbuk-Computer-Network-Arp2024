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
// 가장 차상위 계층인 CChatAppLayer에 대한 클래스입니다.

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
//m_sHeader의 ChatApp 헤더 부분 부분을 초기화하기 위한, Reset함수입니다.

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
// 차상위 계층의 Send 함수입니다. 헤더의 길이를 저장하고, 이를 ppayload에 있는 data를 복사하여 밑 계층(즉, CEthernetLayer)의 Send의 함수에 전달합니다.

BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{
	// ppayload를 ChatApp 헤더 구조체로 넣는다.
	PCHAT_APP_HEADER app_hdr = (PCHAT_APP_HEADER)ppayload;
	if (app_hdr->app_type == 0) {
		unsigned char* GetBuff = (unsigned char *)malloc(sizeof(unsigned char) * (app_hdr->app_length + 1));
		memset(GetBuff, '\0', app_hdr->app_length + 1);

		// 받은 데이터인 App Header를 분석하여, GetBuff에 data 길이와 APP_DATA_SIZE 길이와 비교하여 정한 길이만큼
		// data를 저장한다.
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
// 차상위 계층의 Receive 함수입니다. 
// 밑 계층에서 전달받은 ppayload를 PCHAT_APP_HEADER에 넣어서 헤더를 얻습니다.
// 그 후, 그 정보를 가지고 자신에게 온 정보인지 확인을 하고, 이를 확인해서 수신 여부를 결정합니다.
// 맞으면 Dlg의 Receive 함수로 넘겨서 List에 저장합니다.
// 자신에게 온 정보가 아니면 폐기합니다.


BOOL CChatAppLayer::sendAck(unsigned char* ppayload) {
	return mp_UnderLayer->Send((unsigned char*)&m_ackHeader, APP_HEADER_SIZE, 1);
	
}

BOOL CChatAppLayer::Get_Is_Ack() {
	return Is_Ack;
}
void CChatAppLayer::Set_Is_Ack(BOOL value) {
	Is_Ack = value;
}