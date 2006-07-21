#pragma once
#include <string>
#include <vector>

class CScanner
{
public:
	CScanner(void);
	~CScanner(void);

	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 
};
