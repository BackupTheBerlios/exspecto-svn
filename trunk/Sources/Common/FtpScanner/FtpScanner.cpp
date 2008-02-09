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
	Log::instance().Trace( 0, "ReceiveAns œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð %s ", e.what() );
}catch( ... ){
	Log::instance().Trace( 0, "ReceiveAns œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Ðœô¨ï œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð");
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
	Log::instance().Trace( 0, "FTP::ReceiveAnsLIST: œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨Òœô¨Þ œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s: %s", strAddress, e.what() );
}catch( ... ){
	Log::instance().Trace( 0, "FTP::ReceiveAnsLIST: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Ðœô¨ï œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨Òœô¨Þ œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s", strAddress );
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
catch( std::exception& e ){ Log::instance().Trace( 0, "FTP::SendCommand: œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð %s",e.what() ); }
catch( ... ){ Log::instance().Trace( 0, "FTP::SendCommand: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Ðœô¨ï œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð" ); }
	
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
			Log::instance().Trace( 0, "FTP::Scan: %s:21 - œô¨Ýœô¨Õ œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Õœô¨Ý.", strAddress );
			return 0;
		}
		Log::instance().Trace( 0, "FTP::Scan: %s:21 - œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Õœô¨Ý. œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Ýœô¨Ðœô¨çœô¨Ðœô¨âœô¨Þ.", strAddress );
				
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
	
		// œô¨ßœô¨Þœô¨Úœô¨Ð œô¨Õœô¨áœô¨âœô¨ì œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Óœô¨Ø œô¨Ôœô¨Ûœô¨ï œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ø	
		while ( DIR.size() ){
			// œô¨Øœô¨éœô¨Õœô¨Ü œô¨Þœô¨çœô¨Õœô¨àœô¨Õœô¨Ôœô¨Ýœô¨Þœô¨Ù œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Ó œô¨á ItemLevel
			for ( k = Last_k; k < (unsigned int)Level.size(); k++)
				if ( Level[k] == ItemLevel) break;
			
			while ( DIR.size() ){
				
				if ( k != (unsigned int)Level.size() ) break;
					// œô¨Ôœô¨Þœô¨áœô¨âœô¨Øœô¨Óœô¨Ýœô¨ãœô¨â œô¨ßœô¨Þœô¨áœô¨Ûœô¨Õœô¨Ôœô¨Ýœô¨Øœô¨Ù œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Ó œô¨Ò œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Õ
					// œô¨Øœô¨Ûœô¨Ø œô¨Ýœô¨Õœô¨â œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Óœô¨Ð œô¨á ItemLevel
					
					// œô¨Øœô¨éœô¨Õœô¨Ü œô¨ßœô¨Õœô¨àœô¨Òœô¨ëœô¨Ù œô¨á œô¨Úœô¨Þœô¨Ýœô¨æœô¨Ð œô¨½œô¨µœô¨¾œô¨±œô¨Àœô¨°œô¨±œô¨¾œô¨Âœô¨°œô¨½œô¨½œô¨Ëœô¨¹ œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Ó
					for ( k-- ; k >= 0; k-- )
						if ( Flag[k] == false) break;
					// œô¨Òœô¨ëœô¨åœô¨Þœô¨Ôœô¨Øœô¨Ü œô¨Øœô¨× œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Õœô¨Óœô¨Þ, œô¨âœô¨Ðœô¨Ú œô¨çœô¨âœô¨Þ œô¨Þœô¨Úœô¨Ðœô¨×œô¨ëœô¨Òœô¨Ðœô¨Õœô¨Üœô¨áœô¨ï 
					// œô¨Ýœô¨Ð 1 œô¨ãœô¨àœô¨Þœô¨Òœô¨Õœô¨Ýœô¨ì œô¨Òœô¨ëœô¨èœô¨Õ œô¨½œô¨µœô¨¾œô¨±œô¨Àœô¨°œô¨±œô¨¾œô¨Âœô¨°œô¨½œô¨½œô¨¾œô¨³œô¨¾ œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Óœô¨Ð
					while ( Level[k] < ItemLevel ){
						CommandCWD = strCommand[4]+ "..";
						SendCommand (CommandCWD, ctrl_sock);
						ReceiveAns( ctrl_sock );
						// œô¨ãœô¨Üœô¨Õœô¨Ýœô¨ìœô¨èœô¨Ðœô¨Õœô¨Ü œô¨ãœô¨àœô¨Þœô¨Òœô¨Õœô¨Ýœô¨ì œô¨Òœô¨Ûœô¨Þœô¨Öœô¨Õœô¨Ýœô¨Ýœô¨Þœô¨áœô¨âœô¨Ø
						ItemLevel--;
					}
					// œô¨Òœô¨áœô¨ñ œô¨çœô¨âœô¨Þ œô¨Ýœô¨Øœô¨Öœô¨Õ œô¨ßœô¨Õœô¨àœô¨Òœô¨Þœô¨Óœô¨Þ œô¨á œô¨Úœô¨Þœô¨Ýœô¨æœô¨Ð œô¨½œô¨µœô¨¾œô¨±œô¨Àœô¨°œô¨±œô¨¾œô¨Âœô¨°œô¨½œô¨½œô¨¾œô¨³œô¨¾ œô¨Úœô¨Ðœô¨âœô¨Ðœô¨Ûœô¨Þœô¨Óœô¨Ð
					// œô¨ãœô¨Öœô¨Õ œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Ðœô¨Ýœô¨Þ. œô¨Áœô¨Ûœô¨Õœô¨Ôœô¨Ðœô¨Òœô¨Ðœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨Þ, œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨ïœô¨Õœô¨Ü!
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
	
		Log::instance().Trace( 0, "FTP::Scan: %s:21 œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Õœô¨Ýœô¨Þ.", strAddress );
		SendCommand (strCommand[7], ctrl_sock);
		return true;
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "FTP::Scan: œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨Òœô¨Þ œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s: %s", strAddress, e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "FTP::Scan: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Ðœô¨ï œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨Òœô¨Þ œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s", strAddress );
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
			Log::instance().Trace( 0, "DllMain: œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð: %s", e.what() );
			return;
		}catch( ... )
		{
			Log::instance().Trace( 0, "DllMain: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Ðœô¨ï œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð" );
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
	  Log::instance().Trace( 0, "DllMain: œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð: %s", e.what() );
	  return;
	}catch( ... )
	{
	  Log::instance().Trace( 0, "DllMain: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Ðœô¨ï œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð" );
	  return;
	}
}

void _fini()
{
  DumpMemLeaks();
}
#endif


