#pragma once
// parpLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_PARPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
class parpLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;

public:
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* DstIpAddress, int nlength);
	void			Set_Sender_Address(unsigned char* MACAddr, unsigned char* IpAddress);
	parpLayer(char* pName);
	virtual ~parpLayer();

	typedef struct _PARP_HEADER {
		unsigned short	hard_type; // total length of the data
		unsigned short	portocal_type; // type of application data
		unsigned char   hard_size;
		unsigned char	portocal_size; // application data
		unsigned short	op_Code;
		unsigned char	sender_ethernet_address[ETHER_ADDRESS_SIZE];
		unsigned char	sender_IP_address[IP_ADDRESS_SIZE];
		unsigned char	target_ethernet_address[ETHER_ADDRESS_SIZE];
		unsigned char	target_IP_address[IP_ADDRESS_SIZE];

	} PARP_HEADER, * P_PARP_HEADER;
	CString Msg;

protected:
	PARP_HEADER		m_sHeader;
	PARP_HEADER		m_replyHeader;

	enum {
		DATA_TYPE_CONT = 0x01,
		DATA_TYPE_END = 0x02
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
