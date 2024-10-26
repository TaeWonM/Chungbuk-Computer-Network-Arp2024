#pragma once
// ArpLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_ARPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
class ArpLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;
	BOOL			Is_Ack = FALSE;

public:
	BOOL			sendAck(unsigned char* ppayload);
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);
	BOOL			Get_Is_Ack();
	void			Set_Is_Ack(BOOL value);
	ArpLayer(char* pName);
	virtual ~ArpLayer();

	typedef struct _CHAT_APP_HEADER {
		unsigned short	app_length; // total length of the data
		unsigned char	app_type; // type of application data
		unsigned char   app_unused;
		unsigned char	app_data[APP_DATA_SIZE]; // application data

	} CHAT_APP_HEADER, * PCHAT_APP_HEADER;
	CString Msg;

protected:
	CHAT_APP_HEADER		m_sHeader;
	CHAT_APP_HEADER		m_ackHeader;

	enum {
		DATA_TYPE_CONT = 0x01,
		DATA_TYPE_END = 0x02
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










