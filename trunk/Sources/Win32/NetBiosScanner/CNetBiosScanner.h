//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CNetBiosScanner class
//Author: Parshin Dmitry
//Description: Класс, реализующий функцию сканирования NetBios
//-------------------------------------------------------------------------------------
#pragma once
#include <string>
#include <vector>
#include "CScanner.h"

class CNetBiosScanner: public CScanner
{
public:
	CNetBiosScanner(){};
	virtual ~CNetBiosScanner(){};

	//Сканировать адрес strAddress, результат сложить в vcResList
	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

	//Вернуть имя протокола	
	const char* GetProtocolName(){ return "NetBios"; }

protected:

	//Рекурсивная функция, перебирающая все вложенные папки/файлы
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );
	
};


