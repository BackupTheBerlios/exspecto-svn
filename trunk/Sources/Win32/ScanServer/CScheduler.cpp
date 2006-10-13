//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
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

