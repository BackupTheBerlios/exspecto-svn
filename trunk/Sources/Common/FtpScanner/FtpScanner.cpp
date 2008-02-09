#include "ClientSocket.h"
#include "FtpScanner.h"
#include "CLog.h"
#include "MemLeakDetector.h"
#include <vector>
#include <string>
#include "ftpparse.h"


DllExport const char* GetProtocolName()
{
	static const char* str = MOD_NAME;
	return str;
}

#define packet 2692

using namespace std;

vector< string > DIR;
vector< unsigned int > Level;
vector< bool > Flag;


string ReceiveAns (CClientSocket &sock){
	string strAns = "";
	
try{
		
	vector< char > chAns;
	chAns.resize( packet );
	int bytes = sock.Receive(&chAns[0], packet);
	chAns.resize( bytes );
	strAns.insert( strAns.begin(), chAns.begin(), chAns.end() );
	
}catch( std::exception& e ){
	Log::instance().Trace( 0, "ReceiveAns ����������؜��ќ��ڜ��� %s ", e.what() );
}catch( ... ){
	Log::instance().Trace( 0, "ReceiveAns �������՜��؜��ל��Ҝ��՜��������ݜ��М��� ���ޜ�����؜��ќ��ڜ���");
}

	return( strAns );
}

void ReceiveAnsLIST (CClientSocket &sock, IN const char* strAddress, IN StoreDataFunc pStoreFunc, string strPath,
					unsigned int ItemLevel){
try{ 
	int bytes = packet;
	string stik = "";
	unsigned int IDlinaStika = 0;
	struct ftpparse fp ;
	string name, stroka;
	bool flagDIR, flagStik = false;

	string strAns;
	while ( bytes == packet){
		strAns = ReceiveAns( sock );
		bytes = (int)strAns.size();
		int LastPos = 0;
		int NewPos = (int)strAns.find("\r");
					
		while (NewPos != -1){
			if ( stik == "" ) stroka = strAns.substr( LastPos, NewPos - LastPos );
			else{										
				if (strAns[0] == '\n')	stik.resize( stik.size() - 1 );
				else 					stik += strAns.substr( LastPos, NewPos );
				
				stroka = stik;
				stik = "";
			}
			if ( flagStik ){
				ftpparse( &fp, &stroka[0], (int)stroka.size());
				flagStik = false;
			}
			else ftpparse( &fp, &stroka[0], (int)stroka.size());				
			
			name.clear();
			for (int p=0; p<fp.namelen; p++)
				name += fp.name[p];
			name.resize( fp.namelen );
					
			if ( fp.flagtrycwd == 1 ) flagDIR = true;
			else  flagDIR = false;
			
			if ( flagDIR && name != "." && name != ".." ){
				DIR.push_back(name);
				Flag.push_back( false );
				Level.push_back( ItemLevel );
			}
			
			if ( !flagDIR ){
				name = strPath + "/" + name;
				pStoreFunc( strAddress, MOD_NAME, name.c_str(), fp.size , 0, 0 );
			}
						
			LastPos = NewPos + 2;
			NewPos = (int)strAns.find( "\r" , LastPos );				
		}
		if ( bytes == packet && LastPos < packet ){
			stik = strAns.substr( LastPos, packet - LastPos);
			IDlinaStika = packet - LastPos;
			flagStik = true;
		}
		
	}
	
}catch( std::exception& e ){
	Log::instance().Trace( 0, "FTP::ReceiveAnsLIST: ����������؜��ќ��ڜ��� ���Ҝ��� ���Ҝ������՜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���М��Ԝ������՜������ %s: %s", strAddress, e.what() );
}catch( ... ){
	Log::instance().Trace( 0, "FTP::ReceiveAnsLIST: �������՜��؜��ל��Ҝ��՜��������ݜ��М��� ���ޜ�����؜��ќ��ڜ��� ���Ҝ��� ���Ҝ������՜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���М��Ԝ������՜������ %s", strAddress );
}	
}

unsigned int SendCommand (std::string Command, CClientSocket &sock)
{
try{	
	
	string strAns;
	Command = Command + "\r\n";	
	sock.Send((void *)Command.c_str(), (int)Command.size());
	
	if (Command.substr(0,3) != "CWD")
		strAns = ReceiveAns( sock );
		
	if (Command == "PASV\r\n") 
	{
		string substring;
		int i=1;
		int Pos2 = (int)strAns.find_last_of( ',' );
		int Pos3 = (int)strAns.find_last_of( ')' );
		while ( strAns[Pos2-i] != ',' ) i++;
		int Pos1 = Pos2 - i;
		substring = strAns.substr( Pos1+1, Pos2-Pos1 - 1 );
		int hi = atoi( substring.c_str() );
		substring = strAns.substr( Pos2+1, Pos3-Pos2 - 1 );
		int low = atoi( substring.c_str() );
		return ( 256*hi + low );
	}
	
}
catch( std::exception& e ){ Log::instance().Trace( 0, "FTP::SendCommand: ����������؜��ќ��ڜ��� %s",e.what() ); }
catch( ... ){ Log::instance().Trace( 0, "FTP::SendCommand: �������՜��؜��ל��Ҝ��՜��������ݜ��М��� ���ޜ�����؜��ќ��ڜ���" ); }
	
	return 0;
}


DllExport bool Scan( IN const char* strAddress, IN StoreDataFunc pStoreFunc, IN HANDLE hCancelEvent )
{
try{
		unsigned int Port;
		CClientSocket ctrl_sock, data_sock;
		unsigned int k;
		unsigned int ItemLevel = 0;
		string strAns, strPath = "";

		try{ ctrl_sock.Connect(strAddress, 21);
		}catch(...){
			Log::instance().Trace( 0, "FTP::Scan: %s:21 - ���ݜ��� ���Ԝ��ޜ�����������ߜ��՜���.", strAddress );
			return 0;
		}
		Log::instance().Trace( 0, "FTP::Scan: %s:21 - ���Ԝ��ޜ�����������ߜ��՜���. �������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ݜ��М�����М������.", strAddress );
				
		ReceiveAns( ctrl_sock );
		
		string CommandCWD;
		string strCommand [] = {	"USER anonymous",			//	0
									"PASS nobody@nomail.com",	//	1
									"TYPE A",					//	2
									"PWD",						//	3
									"CWD ",						//	4
									"PASV",						//	5
									"LIST",						//	6
									"QUIT"};					//	7
										
				SendCommand (strCommand[0], ctrl_sock);
				SendCommand (strCommand[1], ctrl_sock);
				SendCommand (strCommand[2], ctrl_sock);		
		Port = 	SendCommand (strCommand[5], ctrl_sock);
		data_sock.Connect( strAddress, Port );
				SendCommand (strCommand[6], ctrl_sock);
			
					try{ ReceiveAnsLIST( data_sock, strAddress, pStoreFunc,  strPath, ItemLevel);
					}catch(...){}
					data_sock.Close();
		
				ReceiveAns( ctrl_sock );
			
		unsigned int Last_k = 0;
	
		// ���ߜ��ޜ��ڜ��� ���՜��������� ���ڜ��М�����М��ۜ��ޜ��Ӝ��� ���Ԝ��ۜ��� ���ޜ��ќ������М��ќ��ޜ�����ڜ���	
		while ( DIR.size() ){
			// ���؜�����՜��� ���ޜ�����՜������՜��Ԝ��ݜ��ޜ��� ���ڜ��М�����М��ۜ��ޜ��� ���� ItemLevel
			for ( k = Last_k; k < (unsigned int)Level.size(); k++)
				if ( Level[k] == ItemLevel) break;
			
			while ( DIR.size() ){
				
				if ( k != (unsigned int)Level.size() ) break;
					// ���Ԝ��ޜ��������؜��Ӝ��ݜ������ ���ߜ��ޜ�����ۜ��՜��Ԝ��ݜ��؜��� ���ڜ��М�����М��ۜ��ޜ��� ���� ������ߜ��؜�����ڜ���
					// ���؜��ۜ��� ���ݜ��՜��� ���ڜ��М�����М��ۜ��ޜ��Ӝ��� ���� ItemLevel
					
					// ���؜�����՜��� ���ߜ��՜������Ҝ������ ���� ���ڜ��ޜ��ݜ������ �������������������������������������������������˜��� ���ڜ��М�����М��ۜ��ޜ���
					for ( k-- ; k >= 0; k-- )
						if ( Flag[k] == false) break;
					// ���Ҝ��������ޜ��Ԝ��؜��� ���؜��� ������՜��ڜ��������՜��Ӝ���, ������М��� ���������� ���ޜ��ڜ��М��ל�����Ҝ��М��՜��ܜ������ 
					// ���ݜ��� 1 ����������ޜ��Ҝ��՜��ݜ��� ���Ҝ��������� ���������������������������������������������������������� ���ڜ��М�����М��ۜ��ޜ��Ӝ���
					while ( Level[k] < ItemLevel ){
						CommandCWD = strCommand[4]+ "..";
						SendCommand (CommandCWD, ctrl_sock);
						ReceiveAns( ctrl_sock );
						// ������ܜ��՜��ݜ��������М��՜��� ����������ޜ��Ҝ��՜��ݜ��� ���Ҝ��ۜ��ޜ��֜��՜��ݜ��ݜ��ޜ���������
						ItemLevel--;
					}
					// ���Ҝ������ ���������� ���ݜ��؜��֜��� ���ߜ��՜������Ҝ��ޜ��Ӝ��� ���� ���ڜ��ޜ��ݜ������ ���������������������������������������������������������� ���ڜ��М�����М��ۜ��ޜ��Ӝ���
					// ������֜��� ���ޜ��ќ������М��ќ��ޜ�����М��ݜ���. �������ۜ��՜��Ԝ��М��Ҝ��М�����՜��ۜ�����ݜ���, ������Ԝ��М��ۜ�����՜���!
					if ( k != DIR.size()){
						DIR.erase(DIR.begin() + k+1, DIR.end());
						Flag.erase(Flag.begin() + k+1, Flag.end());
						Level.erase(Level.begin() + k+1, Level.end());
					}
			}
			
			if ( DIR.size() ){	
				ItemLevel++;
				Flag[k] = true;
				Last_k = k+1;
				
				CommandCWD = strCommand[4]+ DIR[k];
				SendCommand (CommandCWD, ctrl_sock);
				strAns = ReceiveAns( ctrl_sock );
			}
		
			if ( DIR.size() ){
				strPath = strAns.substr( strAns.find_first_of("\"") + 1, strAns.find_last_of("\"") - strAns.find_first_of("\"") - 1); 
		Port = 	SendCommand (strCommand[5], ctrl_sock);
				data_sock.Connect( strAddress, Port);
				SendCommand (strCommand[6], ctrl_sock);
					
					try{ ReceiveAnsLIST( data_sock, strAddress, pStoreFunc, strPath, ItemLevel);
					}catch(...){}
					data_sock.Close();
						
				ReceiveAns( ctrl_sock );
			}
		}
	
		Log::instance().Trace( 0, "FTP::Scan: %s:21 �������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ڜ��ޜ��ݜ�����՜��ݜ���.", strAddress );
		SendCommand (strCommand[7], ctrl_sock);
		return true;
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "FTP::Scan: ����������؜��ќ��ڜ��� ���Ҝ��� ���Ҝ������՜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���М��Ԝ������՜������ %s: %s", strAddress, e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "FTP::Scan: �������՜��؜��ל��Ҝ��՜��������ݜ��М��� ���ޜ�����؜��ќ��ڜ��� ���Ҝ��� ���Ҝ������՜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���М��Ԝ������՜������ %s", strAddress );
	}
	return false;
}

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( DLL_PROCESS_ATTACH == ul_reason_for_call )
	{
		try{	
			Log::instance().SetModuleName( MOD_NAME );
		}catch( std::exception& e)
		{
			Log::instance().Trace( 0, "DllMain: ����������؜��ќ��ڜ���: %s", e.what() );
			return;
		}catch( ... )
		{
			Log::instance().Trace( 0, "DllMain: �������՜��؜��ל��Ҝ��՜��������ݜ��М��� ������؜��ќ��ڜ���" );
			return;
		}
	}
	if( DLL_PROCESS_DETACH == ul_reason_for_call )
	{
		DumpMemLeaks();
	}
}
#else
void _init()
{
  try{	
	Log::instance().SetModuleName( MOD_NAME );
  }catch( std::exception& e)
	{
	  Log::instance().Trace( 0, "DllMain: ����������؜��ќ��ڜ���: %s", e.what() );
	  return;
	}catch( ... )
	{
	  Log::instance().Trace( 0, "DllMain: �������՜��؜��ל��Ҝ��՜��������ݜ��М��� ������؜��ќ��ڜ���" );
	  return;
	}
}

void _fini()
{
  DumpMemLeaks();
}
#endif


