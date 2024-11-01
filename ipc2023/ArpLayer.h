#pragma once
// ArpLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_ARPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define ARP_CACHE_SIZE 256

#include "BaseLayer.h"
#include "pch.h"
class ArpLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;

	struct ArpEntry {
		unsigned char ip_address[4]; // IP 林家
		unsigned char mac_address[6]; // MAC 林家
		time_t timestamp;
	};

	ArpEntry arp_cache[ARP_CACHE_SIZE];
	int arp_cache_count;

public:
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* DstIpAddress, int nlength);
	void			Set_Sender_Address(unsigned char* MACAddr, unsigned char* IpAddress);
	ArpLayer(char* pName);
	virtual ~ArpLayer();
	void			SendGARP(const unsigned char* macAddr);

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

	void UpdateCacheTable(unsigned char* ipAddress, unsigned char* macAddress);

protected:
	ARP_HEADER		m_sHeader;
	ARP_HEADER		m_replyHeader;

	enum {
		DATA_TYPE_CONT = 0x01,
		DATA_TYPE_END = 0x02
	};
};


#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










