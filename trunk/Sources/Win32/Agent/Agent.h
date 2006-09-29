//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#pragma once
#include "windows.h"
#include "..\libNet\packet.h"
#include "CScanner.h"
#include <map>

class CAgent
{
public:
	
	//Конструктор,strSchedulerAddress - адрес планировщика
	CAgent( std::string strSchedulerAddress );
	
	~CAgent(void);

protected:

	//Заполняет m_vecScanners и m_mapLibraries,возвращает кол-во найденных plugin-ов сканирования
	int FillScannersList();
	//Очищает m_vecScanners и m_mapLibraries
	void ClearScannersList();
	
	//Критическая секция на запись переменной текущего состояния
	CRITICAL_SECTION m_csCurState;

	//Критическая секция на выполнение команд
	CRITICAL_SECTION m_csCommandExec;

	//Тип, описывающий возможные состояния агента
	enum enumStates{
		Idling = 0x1,
		Scanning = 0x2,
		SendingData = 0x3,
	};

	//Текущее состояние агента
	enumStates m_CurState;

	//Адрес планировщика
	std::string m_strSchedulerAddress;

	//Структура, описывающая параметры,передаваемые функции fnProcessThreadProc
	struct ProcessParam{
		CAgent*	pThis;
		BYTE*	pbBuf;
		int		iCount;
		CSocket*	client_sock;
	};

	//Поток обработки входящих сообщений
	static DWORD WINAPI fnProcessThreadProc( LPVOID pParameter );

	//Поток ожидания входящих соединений
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );
	
	//Типы функций, содержащихся в dll с plugin-ом
	typedef CScanner* (*fnGetScannerFunc)();
	typedef void (*fnReleaseScannerFunc)();
	
	//Вектор адресов объектов классов сканирования (загруженных из плагинов)
	std::vector< CScanner* > m_vecScanners;
	
	typedef std::map< HINSTANCE , fnReleaseScannerFunc > LibrariesMapType;
	//Вектор дескрипторов открытых библиотек для дальнейшего их закрытия
	LibrariesMapType m_mapLibraries;
};
