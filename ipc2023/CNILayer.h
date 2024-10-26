// FileLayer.h: interface for the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
#define AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
#include <pcap.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <Packet32.h>
#pragma comment(lib, "packet.lib")
#include <thread>

using namespace std;

class CNILayer
	: public CBaseLayer
{
private :
	bool is_set = false;
	pcap_if_t* m_AdapterList[7];
	int m_Maxadapter = 0;
	int curAdapterIndex = 0;
	pcap_t* fp = NULL;
public:
	BOOL	Receive();
	BOOL	Send(unsigned char* ppayload, int nlength);
	BOOL    send_packet(pcap_t* handle, u_char* message, int len);
	int GetMaxAdapterIndex();
	void SetAdpterDeivce();
	pcap_if_t* GetAdapter(int index);
	void Set_is_set(bool value);
	bool Get_is_set();
	int GetCurAdapterIndex();
	void SetCurAdapterIndex(int value);
	CNILayer(char* pName);
	static UINT ReceiveThread(LPVOID pParam);
	CWinThread * m_pThread = NULL;
	virtual ~CNILayer();
};

#endif // !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
