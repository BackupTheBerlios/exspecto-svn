#define BYTE unsigned char
class CLog
{
public:
	CLog();
	~CLog();
	void Trace(int level, char* trace_text, ...);
	void Dump(int level, BYTE* pbDumpData, int iDataSize, char* about, ...);
};
