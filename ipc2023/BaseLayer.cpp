// BaseLayer.cpp: implementation of the CBaseLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ipc2023.h"
#include "BaseLayer.h"
#include "pch.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseLayer::CBaseLayer(char* pName)
	: m_nUpperLayerCount(0), m_nUnderLayerCount(0)
{
	m_pLayerName = pName;
}

CBaseLayer::~CBaseLayer()
{

}

void CBaseLayer::SetUnderUpperLayer(CBaseLayer* pUULayer)
{
	if (!pUULayer) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE("[CBaseLayer::SetUnderUpperLayer] The variable , 'pUULayer' is NULL");
#endif
		return;
	}

	//////////////////////// fill the blank ///////////////////////////////
		// 인자로 받은 계층은 현재 계층의 Under로 놓고
		// 현재 계층을 인자로 받은 계층의 Upper로 놓는다.
	SetUnderLayer(pUULayer);
	pUULayer->SetUpperLayer(this);
	///////////////////////////////////////////////////////////////////////
}
// 받은 layer를 아래 계층으로 설정하고, 받은 layer의 윗 계층에 현재의 계층을 설정합니다.

void CBaseLayer::SetUpperUnderLayer(CBaseLayer* pUULayer)
{
	if (!pUULayer) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE("[CBaseLayer::SetUpperUnderLayer] The variable , 'pUULayer' is NULL");
#endif
		return;
	}

	//////////////////////// fill the blank ///////////////////////////////
		// 인자로 받은 계층을 Upper에 놓고
		// 현재 계층은 Upper로 놓은 계층의 Under로 놓는다.
	SetUpperLayer(pUULayer);
	pUULayer->SetUnderLayer(this);
	///////////////////////////////////////////////////////////////////////
}
// 위의 함수와 반대로 받은 layer를 윗 계층으로 설정하고, 받은 layer의 아래 계층에 현재의 계층을 설정합니다.

void CBaseLayer::SetUpperLayer(CBaseLayer* pUpperLayer)
{
	if (!pUpperLayer) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE("[CBaseLayer::SetUpperLayer] The variable , 'pUpperLayer' is NULL");
#endif
		return;
	}

	// UpperLayer is added..
	this->mp_aUpperLayer[m_nUpperLayerCount++] = pUpperLayer;
}
// 위의 계층을 설정하는 함수입니다. mp_aUpperLayer라는 배열에 pUpperLayer를 넣습니다.

void CBaseLayer::SetUnderLayer(CBaseLayer* pUnderLayer)
{
	if (!pUnderLayer) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE("[CBaseLayer::SetUnderLayer] The variable , 'pUnderLayer' is NULL\n");
#endif
		return;
	}

	// UnderLayer assignment..
	this->mp_UnderLayer[m_nUnderLayerCount++] = pUnderLayer;
}
// 아래의 계층을 설정하는 함수입니다. mp_aUpperLayer라는 배열에 pUpperLayer를 넣습니다.


CBaseLayer* CBaseLayer::GetUpperLayer(int nindex)
{
	if (nindex < 0 ||
		nindex > m_nUpperLayerCount ||
		m_nUpperLayerCount < 0)
	{
#ifdef _DEBUG
		TRACE("[CBaseLayer::GetUpperLayer] There is no UpperLayer in Array..\n");
#endif 
		return NULL;
	}

	return mp_aUpperLayer[nindex];
}
// index를 통해서 계층을 얻는 getter함수입니다.

CBaseLayer* CBaseLayer::GetUnderLayer(int index)
{
	if (index < 0 ||
		index > m_nUnderLayerCount ||
		m_nUnderLayerCount < 0)
	{
#ifdef _DEBUG
		TRACE("[CBaseLayer::GetUnderLayer] There is not a UnerLayer..\n");
#endif 
		return NULL;
	}

	return mp_UnderLayer[index];
}
// 현재 기준으로 아래 계층이라고 설정된 값을 출력하는 부분입니다.

char* CBaseLayer::GetLayerName()
{
	return m_pLayerName;
}
// 현재 계층의 이름을 확인하는 getter함수입니다.

