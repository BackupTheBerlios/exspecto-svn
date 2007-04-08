//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CNetBiosScanner class
//Author: Parshin Dmitry
//Description: Класс, реализующий функцию сканирования NetBios
//-------------------------------------------------------------------------------------
#ifndef _CNETBIOSSCANNER
#define _CNETBIOSSCANNER

#include "MemLeakDetector.h"
#include <string>
#include <vector>
#include "CScanner.h"
#include "CLog.h"

#define MOD_NAME "NetBios"

class CNetBiosScanner: public CScanner
{
public:
	CNetBiosScanner();
	virtual ~CNetBiosScanner(){};

	//Сканировать адрес strAddress, результат сложить в vcResList
	virtual void Scan( IN std::string strAddress, OUT filesStr& ResList, HANDLE hCancelEvent ); 

	//Вернуть имя протокола	
	virtual const char* GetProtocolName()
	{
		return MOD_NAME;
	}

protected:

	//Рекурсивная функция, перебирающая все вложенные папки/файлы
	void EnumFiles( IN const char* strSharePath, OUT filesStr& FilesList, HANDLE hCancelEvent );
	
};

#endif
