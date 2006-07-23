#pragma once
#include <string>
#include <vector>

class CScanner
{
public:
	CScanner(void);
	~CScanner(void);

	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

protected:

	//Рекурсивная функция, перебирающая все вложенные папки/файлы
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );

	
};
