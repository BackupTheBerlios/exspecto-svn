//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include "windows.h"
#include <string>
#include <map>
#include <vector>
#include "CAgentHandler.h"
#include "../libCommon/Container.hpp"
#include "AgentsLoadStrategy.h"
#include "CStartScanEventInterface.h"


class CScanServer: public CStartScanEventInterface 
{
public:
	CScanServer(void);
	virtual ~CScanServer(void);
	
	virtual void OnStartScan();

protected:

	Container< CAgentHandler*, AgentsLoadStrategy > m_AgentsContainer;

};
