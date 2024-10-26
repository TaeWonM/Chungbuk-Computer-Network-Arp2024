#pragma once
// LayerManager.cpp: implementation of the CLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "LayerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLayerManager::CLayerManager()
	: m_nLayerCount(0),
	mp_sListHead(NULL),
	mp_sListTail(NULL),
	m_nTop(-1)
{

}

CLayerManager::~CLayerManager()
{

}

void CLayerManager::AddLayer(CBaseLayer* pLayer)
{
	mp_aLayers[m_nLayerCount++] = pLayer;
}
// mp_aLayers라는 계층들을 모은 배열에 새로운 계층을 넣는 setter 함수입니다.

CBaseLayer* CLayerManager::GetLayer(int nindex)
{
	return mp_aLayers[nindex];
}
// mp_aLayers라는 계층들을 모은 배열에 있는 계층을 얻는 getter 함수입니다.

CBaseLayer* CLayerManager::GetLayer(char* pName)
{
	for (int i = 0; i < m_nLayerCount; i++)
	{
		if (!strcmp(pName, mp_aLayers[i]->GetLayerName()))
			return mp_aLayers[i];
	}

	return NULL;
}
// 계층을 정리한 pName string을 통해서 pName 이름을 갖는 계층을 만드는 함수입니다.

void CLayerManager::ConnectLayers(char* pcList)
{
	MakeList(pcList);
	LinkLayer(mp_sListHead);
	int arr;
	arr = 3;
}
// 계층을 정리한 pcList string을 통해서 계층을 만드는 함수입니다.

void CLayerManager::MakeList(char* pcList)
{
	while (pcList != (char*)0x01)
	{
		char sBuff[100];
		sscanf_s(pcList, "%s", sBuff, sizeof(sBuff));
		pcList = strchr(pcList, ' ') + 1;

		PNODE pNode = AllocNode(sBuff);
		AddNode(pNode);
	}
}
// 계층을 정리한 pcList string를 string 단위로 읽어서 새로운 Pnode에 계층을 만드는 함수입니다.

CLayerManager::PNODE CLayerManager::AllocNode(char* pcName)
{
	PNODE node = new NODE;
	ASSERT(node);

	strcpy_s(node->token, pcName);
	node->next = NULL;

	return node;
}
// pNode를 동적 할당하는 부분입니다.
// node는 이름과 next로 설정되어 있습니다.

void CLayerManager::AddNode(PNODE pNode)
{
	if (!mp_sListHead)
	{
		mp_sListHead = mp_sListTail = pNode;
	}
	else
	{
		mp_sListTail->next = pNode;
		mp_sListTail = pNode;
	}
}
// pNode를 넣는 부분입니다. 먼저 온 순서대로 이어지는 구조입니다.


void CLayerManager::Push(CBaseLayer* pLayer)
{
	if (m_nTop >= MAX_LAYER_NUMBER)
	{
#ifdef _DEBUG
		TRACE("The Stack is full.. so cannot run the push operation.. \n");
#endif
		return;
	}

	mp_Stack[++m_nTop] = pLayer;
}
// layer를 mp_Stack에 넣는 Push 함수입니다.

CBaseLayer* CLayerManager::Pop()
{
	if (m_nTop < 0)
	{
#ifdef _DEBUG
		TRACE("The Stack is empty.. so cannot run the pop operation.. \n");
#endif
		return NULL;
	}

	CBaseLayer* pLayer = mp_Stack[m_nTop];
	mp_Stack[m_nTop] = NULL;
	m_nTop--;

	return pLayer;
}
// mp_ Stack에 있는 layer를 빼는 pop 함수입니다.

CBaseLayer* CLayerManager::Top()
{
	if (m_nTop < 0)
	{
#ifdef _DEBUG
		TRACE("The Stack is empty.. so cannot run the top operation.. \n");
#endif
		return NULL;
	}

	return mp_Stack[m_nTop];
}
// mp_Stack의 top부분을 호출하는 부분입니다.

void CLayerManager::LinkLayer(PNODE pNode)
{
	CBaseLayer* pLayer = NULL;

	while (pNode)
	{
		if (!pLayer)
			pLayer = GetLayer(pNode->token);
		else
		{
			if (*pNode->token == '(')
				Push(pLayer);
			else if (*pNode->token == ')')
				Pop();
			else
			{
				char cMode = *pNode->token;
				char* pcName = pNode->token + 1;

				pLayer = GetLayer(pcName);

				switch (cMode)
				{
				case '*': Top()->SetUpperUnderLayer(pLayer); break;
				case '+': Top()->SetUpperLayer(pLayer); break;
				case '-': Top()->SetUnderLayer(pLayer); break;
				}
			}
		}

		pNode = pNode->next;
	}
}
// pNode를 인수로 받아 token을 확인해 어떻게 설정할지 확인하는 함수입니다.
// * 이면 위 아래로 + 이면 위만, - 이면 아래만 설정하는 함수입니다.

void CLayerManager::DeAllocLayer()
{
	for (int i = 0; i < this->m_nLayerCount; i++)
		delete this->mp_aLayers[i];
}
// 동적 할당된 mp_aLayers을 해제하는 합수입니다. 
