//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include "CScheduler.h"
#include "windows.h"

CScheduler::CScheduler(void)
{
	m_pTrigger = new CTimer( this );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
}

void CScheduler::OnStartScan()
{
	//TODO:
	MessageBox( NULL, "asdasd", "Asdasd", MB_OK );
}

