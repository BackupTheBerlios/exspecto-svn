#ifndef CSCANNER_H_
#define CSCANNER_H_
#include "CDBProvider.h"


class CScanner
{
public:
	CScanner(){};
	virtual ~CScanner(){};

	virtual void Scan( IN std::string strAddress, OUT filesStr& ResList, HANDLE hCancelEvent ) = 0;

	virtual const char* GetProtocolName() = 0;

};
#endif //CSCANNER_H_
