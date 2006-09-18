//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScanner class
//Author: Parshin Dmitry
//Description: Класс, реализующий функцию сканирования
//-------------------------------------------------------------------------------------
#pragma once
#include <string>
#include <vector>

class CScanner
{
public:
	CScanner(void);
	~CScanner(void);

	//Сканировать адрес strAddress, результат сложить в vcResList
	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

protected:

	//Рекурсивная функция, перебирающая все вложенные папки/файлы
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );

	
};
