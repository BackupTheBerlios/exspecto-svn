#ifndef CSCANNER_H_
#define CSCANNER_H_

#include <string>
#include <vector>

class CScanner
{
public:
	CScanner(){};
	virtual ~CScanner(){};
		
	virtual void Scan( IN std::string strAddress, OUT std::vector< std::string >& vecResList ) = 0;
	
	virtual const char* GetProtocolName() = 0;

};
#endif //CSCANNER_H_
