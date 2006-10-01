
class CScanner
{
public:
	CScanner(){};
	
	virtual void Scan( IN std::string strAddress, OUT std::vector< std::string >& vecResList ) = 0;
	
	virtual const char* GetProtocolName();
	
	virtual ~CScanner(){};
};

