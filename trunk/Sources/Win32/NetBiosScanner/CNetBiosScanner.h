//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CNetBiosScanner class
//Author: Parshin Dmitry
//Description: Класс, реализующий функцию сканирования NetBios
//-------------------------------------------------------------------------------------
#ifndef _CNETBIOSSCANNER
#define _CNETBIOSSCANNER

#include <string>
#include <vector>
#include "CScanner.h"
#include "CLog.h"

class CNetBiosScanner: public CScanner
{
public:
	CNetBiosScanner();
	virtual ~CNetBiosScanner(){};

	//Сканировать адрес strAddress, результат сложить в vcResList
	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

	//Вернуть имя протокола	
	const char* GetProtocolName()
	{
		log.Trace( 1, "dsfs %s", m_strProtoName ); 
		return m_strProtoName; 
	}

protected:

	//Рекурсивная функция, перебирающая все вложенные папки/файлы
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );
	
	char m_strProtoName[ 8 ];
	
};

#endif
