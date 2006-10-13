//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include ".\ScanServer.h"
#include "windows.h"

CScanServer::CScanServer(void)
{
	m_pTrigger = new CTimer( this );
	m_pTrigger->Start();
}

CScanServer::~CScanServer(void)
{
}

void CScanServer::OnStartScan()
{
	//TODO:
	MessageBox( NULL, "asdasd", "Asdasd", MB_OK );
}

int _tmain(int argc, _TCHAR* argv[])
{
	CScanServer shed;
	Sleep(20000);
	return 0;
}
