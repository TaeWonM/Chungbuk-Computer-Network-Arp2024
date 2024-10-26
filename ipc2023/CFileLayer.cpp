// ChatAppLayer.cpp: implementation of the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "CFileLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLayer::CFileLayer(char* pName)
	: CBaseLayer(pName),
	mp_Dlg(NULL)
{
	ResetHeader();
}
// 가장 차상위 계층인 CChatAppLayer에 대한 클래스입니다.

CFileLayer::~CFileLayer()
{

}

void CFileLayer::ResetHeader()
{
	m_sHeader.fapp_length = 0x00000000;
	m_sHeader.fapp_type = 0x0000;
	m_sHeader.fapp_msg_type = 0x00;
	m_sHeader.unused = 0x00;
	m_sHeader.fapp_seq_num = 0x00000000;
	memset(m_sHeader.app_data, 0, FILE_DATA_SIZE);
	m_ackHeader.fapp_length = 0x00000000;
	m_ackHeader.fapp_type = 0x0004;
	m_ackHeader.fapp_msg_type = 0x00;
	m_ackHeader.unused = 0x00;
	m_ackHeader.fapp_seq_num = 0x00000000;
}
//m_sHeader의 ChatApp 헤더 부분 부분을 초기화하기 위한, Reset함수입니다.

BOOL CFileLayer::Send(unsigned char* ppayload, int nlength)
{
	THREAD_PARAMETER tp = new _FILE_THREAD_PARAMETER;
	tp->curLayer = this;
	tp->nlength = nlength;
	tp->ppayload = new unsigned char[nlength + 1];
	SendTheadRun = TRUE;
	readyToSend = TRUE;
	reSendcount = 0;
	memcpy(tp->ppayload, ppayload, nlength);
	m_pThread = NULL;
	m_pThread = AfxBeginThread(SendThread, (LPVOID)tp);
	return m_pThread ? TRUE : FALSE;
}
// 차상위 계층의 Send 함수입니다. 헤더의 길이를 저장하고, 이를 ppayload에 있는 data를 복사하여 밑 계층(즉, CEthernetLayer)의 Send의 함수에 전달합니다.

UINT CFileLayer::SendThread(LPVOID pParam) {
	_FILE_THREAD_PARAMETER * ntp = (_FILE_THREAD_PARAMETER*)pParam;
	CFileLayer* PID = (CFileLayer*)ntp->curLayer;
	PID->m_sHeader.fapp_length = (unsigned long)ntp->nlength - PID->m_sHeader.unused;
	BOOL bSuccess = TRUE;
	if (ntp->nlength > FILE_DATA_SIZE) {
		PID->sendFragment = ntp->nlength / FILE_DATA_SIZE + (ntp->nlength % (FILE_DATA_SIZE) != 0 ? 1 : 0);
		PID->sendCurFragment = 0;
		//for (int i = 0; i < fragment; i++) 
		while (PID->SendTheadRun) {
			if (!PID->readyToSend) {
				continue;
			}
			PID->m_sHeader.fapp_seq_num = PID->sendCurFragment;
			if (PID->sendCurFragment == 0) {
				PID->m_sHeader.fapp_type = 0x01;
			}
			else if (PID->sendCurFragment == PID->sendFragment - 1) {
				PID->m_sHeader.fapp_length = ntp->nlength % (FILE_DATA_SIZE);
				PID->m_sHeader.fapp_type = 0x03;
			}
			else {
				PID->m_sHeader.fapp_length = FILE_DATA_SIZE;
				PID->m_sHeader.fapp_type = 0x02;
			}
			memcpy(PID->m_sHeader.app_data, &ntp->ppayload[PID->sendCurFragment * FILE_DATA_SIZE], ntp->nlength / (FILE_DATA_SIZE * (PID->sendCurFragment + 1)) > 0 ? FILE_DATA_SIZE : ntp->nlength % (FILE_DATA_SIZE));
			bSuccess = PID->mp_UnderLayer->Send((unsigned char*)&PID->m_sHeader, (ntp->nlength / (FILE_DATA_SIZE * (PID->sendCurFragment + 1)) > 0 ? FILE_DATA_SIZE : ntp->nlength % (FILE_DATA_SIZE)) + FILE_HEADER_SIZE, 2) && bSuccess;
			PID->sendCurFragment++;
			PID->readyToSend = FALSE;
			if (PID->sendCurFragment == PID->sendFragment)
				break;
			PID->upperLayerSetTimer(2);
		}
	}
	else {
		PID->sendFragment = 1;
		PID->sendCurFragment = 1;
		bSuccess = FALSE;
		memcpy(PID->m_sHeader.app_data, ntp->ppayload, ntp->nlength);
		PID->m_sHeader.fapp_type = 0x00;
		bSuccess = PID->mp_UnderLayer->Send((unsigned char*)&PID->m_sHeader, ntp->nlength + FILE_HEADER_SIZE, 2);
	}
	PID->ResetHeader();
	delete[] ntp->ppayload;
	delete ntp;
	if (bSuccess && PID->SendTheadRun) AfxMessageBox(_T("File transfer completed successfully."));
	return 0;
}

BOOL CFileLayer::Receive(unsigned char* ppayload)
{
	// ppayload를 ChatApp 헤더 구조체로 넣는다.
	PFILE_HEADER app_hdr = (PFILE_HEADER)ppayload;
	if (app_hdr->fapp_type == 0) {
		recFragment = 1;
		FileName = (unsigned char*)malloc(sizeof(unsigned char) * (app_hdr->unused + 1));
		Msg = (unsigned char *)malloc(sizeof(unsigned char) * (app_hdr->fapp_length + 1));
		memset(Msg, '\0', app_hdr->fapp_length + 1);
		memset(FileName, '\0', app_hdr->unused + 1);
		// 받은 데이터인 App Header를 분석하여, GetBuff에 data 길이와 APP_DATA_SIZE 길이와 비교하여 정한 길이만큼
		// data를 저장한다.
		memcpy(Msg, &app_hdr->app_data[app_hdr->unused], app_hdr->fapp_length - app_hdr->unused);
		memcpy(FileName, app_hdr->app_data, app_hdr->unused);
		Write_File(_T((char*)FileName), Msg, app_hdr->fapp_length - app_hdr->unused);
		delete[] Msg;
		delete[] FileName;
		mp_aUpperLayer[0]->SetProgressbar(recFragment, 1);
		return TRUE;
	}
	else if (app_hdr->fapp_type == 1) {
		recFragment = app_hdr->fapp_length / FILE_DATA_SIZE + (app_hdr->fapp_length % (FILE_DATA_SIZE) != 0 ? 1 : 0);
		Msg = (unsigned char*)malloc(sizeof(unsigned char) * (app_hdr->fapp_length + 1));
		FileName = (unsigned char*)malloc(sizeof(unsigned char) * (app_hdr->unused + 1));
		memset(Msg, '\0', app_hdr->fapp_length + 1); memset(FileName, '\0', app_hdr->unused + 1);
		memcpy(Msg, &app_hdr->app_data[app_hdr->unused], FILE_DATA_SIZE - app_hdr->unused);
		memcpy(FileName, app_hdr->app_data, app_hdr->unused);
		mp_aUpperLayer[0]->SetProgressbar(recFragment, app_hdr->fapp_seq_num + 1);
		return TRUE;
	}
	else if (app_hdr->fapp_type == 2) {
		memcpy(&Msg[FILE_DATA_SIZE * app_hdr->fapp_seq_num - app_hdr->unused], app_hdr->app_data, app_hdr->fapp_length);
		mp_aUpperLayer[0]->SetProgressbar(recFragment, app_hdr->fapp_seq_num + 1);
		return TRUE;
	}
	else if (app_hdr->fapp_type == 3) {
		memcpy(&Msg[FILE_DATA_SIZE * app_hdr->fapp_seq_num - app_hdr->unused], app_hdr->app_data, app_hdr->fapp_length);
		Write_File(_T((char *) FileName), Msg, app_hdr->fapp_seq_num * FILE_DATA_SIZE - app_hdr->unused + app_hdr->fapp_length);
		delete[] Msg;
		delete[] FileName;
		mp_aUpperLayer[0]->SetProgressbar(recFragment, app_hdr->fapp_seq_num + 1);
		return TRUE;
	}
	else if (app_hdr->fapp_type == 4) {
		readyToSend = TRUE;
		mp_aUpperLayer[0]->upperLayerKillTimer(2);
		reSendcount = 0;
		mp_aUpperLayer[0]->SetProgressbar(sendFragment, sendCurFragment);
		return FALSE;
	}
}
// 차상위 계층의 Receive 함수입니다. 
// 밑 계층에서 전달받은 ppayload를 PCHAT_APP_HEADER에 넣어서 헤더를 얻습니다.
// 그 후, 그 정보를 가지고 자신에게 온 정보인지 확인을 하고, 이를 확인해서 수신 여부를 결정합니다.
// 맞으면 Dlg의 Receive 함수로 넘겨서 List에 저장합니다.
// 자신에게 온 정보가 아니면 폐기합니다.


BOOL CFileLayer::sendAck(unsigned char* ppayload) {
	return mp_UnderLayer->Send((unsigned char*)&m_ackHeader, FILE_HEADER_SIZE, 2);
	
}

BOOL CFileLayer::Get_Is_Ack() {
	return Is_Ack;
}
void CFileLayer::Set_Is_Ack(BOOL value) {
	Is_Ack = value;
}

void CFileLayer::Write_File(const char* filename, unsigned char* data, size_t length)
{
	FILE* file = nullptr;
	if (fopen_s(&file, filename, "wb") != 0) {
		// 파일 열기 실패 처리
		return;
	}

	fwrite(data, sizeof(unsigned char), length, file); // 데이터 쓰기
	fclose(file); // 파일 닫기
}
void CFileLayer::Set_File_length(int N) {
	m_sHeader.unused = N;
}

void CFileLayer::reSend() {
	if (reSendcount == 0 && sendCurFragment)
		sendCurFragment--;
	else if (reSendcount >= 3) {
		SendTheadRun = FALSE;
		AfxMessageBox(_T("File transfer >> Time out"));
		return;
	}
	reSendcount++;
	readyToSend = TRUE;
};