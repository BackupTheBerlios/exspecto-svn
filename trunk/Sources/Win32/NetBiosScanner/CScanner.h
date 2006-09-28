
class CScanner
{
public:
	CScanner(){};
	
	virtual void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ) = 0;
	
	virtual ~CScanner(){};
};

