#pragma once
// CFileLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFILELAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_CFILELAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
class CFileLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;
	BOOL			Is_Ack = FALSE;
	int recFragment = 0;
	int recCurfragment = 0;
	int sendFragment = 0;
	int sendCurFragment = 0;
	BOOL readyToSend = TRUE;
	int reSendcount = 0;
	BOOL SendTheadRun = TRUE;

public:
	BOOL			sendAck(unsigned char* ppayload);
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);
	BOOL			Get_Is_Ack();
	void			Set_Is_Ack(BOOL value);
	void			Write_File(const char* filename, unsigned char* data, size_t length);
	void			Set_File_length(int N);
	void			reSend();
	static UINT		SendThread(LPVOID pParam);
	CFileLayer(char* pName);
	virtual ~CFileLayer();

	typedef struct _FILE_HEADER {
		unsigned long	fapp_length; // total length of the data
		unsigned short	fapp_type; // type of application data
		unsigned char	fapp_msg_type; //type of message data
		unsigned char	unused; // unused
		unsigned long   fapp_seq_num; // fragmentation order
		unsigned char	app_data[FILE_DATA_SIZE]; // application data

	} FILE_HEADER, * PFILE_HEADER;
	typedef struct _FILE_THREAD_PARAMETER {
		unsigned char* ppayload;
		int nlength;
		CFileLayer* curLayer;
	} *THREAD_PARAMETER;
	unsigned char* Msg;
	unsigned char* FileName;
	CWinThread* m_pThread;

protected:
	FILE_HEADER		m_sHeader;
	FILE_HEADER		m_ackHeader;

	enum {
		DATA_TYPE_CONT = 0x01,
		DATA_TYPE_END = 0x02
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










