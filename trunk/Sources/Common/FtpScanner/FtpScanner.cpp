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
	Log::instance().Trace( 0, "ReceiveAns Ошибка %s ", e.what() );
}catch( ... ){
	Log::instance().Trace( 0, "ReceiveAns Неизвестная ошибка");
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
	Log::instance().Trace( 0, "FTP::ReceiveAnsLIST: Ошибка во время сканирования адреса %s: %s", strAddress, e.what() );
}catch( ... ){
	Log::instance().Trace( 0, "FTP::ReceiveAnsLIST: Неизвестная ошибка во время сканирования адреса %s", strAddress );
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
catch( std::exception& e ){ Log::instance().Trace( 0, "FTP::SendCommand: Ошибка %s",e.what() ); }
catch( ... ){ Log::instance().Trace( 0, "FTP::SendCommand: Неизвестная ошибка" ); }
	
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
			Log::instance().Trace( 0, "FTP::Scan: %s:21 - не доступен.", strAddress );
			return 0;
		}
		Log::instance().Trace( 0, "FTP::Scan: %s:21 - доступен. Сканирование начато.", strAddress );
				
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
	
		// пока есть каталоги для обработки	
		while ( DIR.size() ){
			// ищем очередной каталог с ItemLevel
			for ( k = Last_k; k < (unsigned int)Level.size(); k++)
				if ( Level[k] == ItemLevel) break;
			
			while ( DIR.size() ){
				
				if ( k != (unsigned int)Level.size() ) break;
					// достигнут последний каталог в списке
					// или нет каталога с ItemLevel
					
					// ищем первый с конца НЕОБРАБОТАННЫЙ каталог
					for ( k-- ; k >= 0; k-- )
						if ( Flag[k] == false) break;
					// выходим из текущего, так что оказываемся 
					// на 1 уровень выше НЕОБРАБОТАННОГО каталога
					while ( Level[k] < ItemLevel ){
						CommandCWD = strCommand[4]+ "..";
						SendCommand (CommandCWD, ctrl_sock);
						ReceiveAns( ctrl_sock );
						// уменьшаем уровень вложенности
						ItemLevel--;
					}
					// всё что ниже первого с конца НЕОБРАБОТАННОГО каталога
					// уже обработано. Следавательно, удаляем!
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
	
		Log::instance().Trace( 0, "FTP::Scan: %s:21 Сканирование закончено.", strAddress );
		SendCommand (strCommand[7], ctrl_sock);
		return true;
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "FTP::Scan: Ошибка во время сканирования адреса %s: %s", strAddress, e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "FTP::Scan: Неизвестная ошибка во время сканирования адреса %s", strAddress );
	}
	return false;
}

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
			Log::instance().Trace( 0, "DllMain: Ошибка: %s", e.what() );
			return false;
		}catch( ... )
		{
			Log::instance().Trace( 0, "DllMain: Неизвестная шибка" );
			return false;
		}
	}
	if( DLL_PROCESS_DETACH == ul_reason_for_call )
	{
		DumpMemLeaks();
	}
    return true;
}




