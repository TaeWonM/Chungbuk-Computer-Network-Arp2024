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

	typedef struct _ARP_HEADER {
		unsigned short	hard_type; // total length of the data
		unsigned short	portocal_type; // type of application data
		unsigned char   hard_size;
		unsigned char	portocal_size; // application data
		unsigned short	op_Code;
		unsigned char	sender_ethernet_address[ETHER_ADDRESS_SIZE];
		unsigned char	sender_IP_address[IP_ADDRESS_SIZE];
		unsigned char	target_ethernet_address[ETHER_ADDRESS_SIZE];
		unsigned char	target_IP_address[IP_ADDRESS_SIZE];

	} ARP_HEADER, * P_ARP_HEADER;
	CString Msg;

protected:
	ARP_HEADER		m_sHeader;
	ARP_HEADER		m_ackHeader;

	enum {
		DATA_TYPE_CONT = 0x01,
		DATA_TYPE_END = 0x02
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)









