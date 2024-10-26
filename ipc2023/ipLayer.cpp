// ipLayer.cpp: implementation of the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "ipLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ipLayer::ipLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
}

ipLayer::~ipLayer()
{
	TRY
	{
		//////////////////////// fill the blank ///////////////////////////////
				CFile::Remove(_T("IpcBuff.txt")); // ���� ����
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

void ipLayer::ResetHeader() {
	//m_IpAddrmap.clear();
}
BOOL ipLayer::Send(unsigned char* ppayload, int nlength)
{	
	CString unIpAddr;
	unIpAddr.Format("%d.%d.%d.%d", ppayload[0], ppayload[1], ppayload[2], ppayload[3]);
	/*if (m_IpAddrmap.find(unIpAddr) != m_IpAddrmap.end()) return TRUE;
	else {
		mp_UnderLayer->Send(ppayload, 4);
	}*/
	return true;
}
// ���� �Ʒ� ������ File ������ Send �Լ� �Դϴ�. IpcBuff.txt��� ������ ���� �����ϴ�

BOOL ipLayer::Receive(unsigned char* ppayload)
{
	return TRUE;
}