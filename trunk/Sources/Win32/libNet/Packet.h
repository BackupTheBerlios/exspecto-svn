//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#ifndef PACKET_H_
#define PACKET_H_

#include "Socket.h"
#include "precomp.h"
#include "../CommonFiles/commands.h"

class CPacket
{
public:
	//Классы исключений, генерируемыз CPacket
	class PacketErr: public std::runtime_error
	{
	public:
		PacketErr( const std::string& strErr ):std::runtime_error( strErr ){};
		virtual ~PacketErr()throw(){};
	};

	class PacketFormatErr: public PacketErr
	{
	public:
		PacketFormatErr( const std::string& strErr ):PacketErr( strErr ){};
		virtual ~PacketFormatErr()throw(){};
	};

	CPacket(void);
	~CPacket(void);

	//Добавить в пакет идентификатор команды
	void BeginCommand( enumCommands Command );

	//Добавить параметр
	//	pbParam - буфер с данными
	//	iSize - размер данных
	void AddParam( const BYTE* pbParam, int iSize );

	//Добавить параметр
	//	dwParam - параметр типа DWORD
	void AddParam( DWORD dwParam );

	//Добавить параметр
	//	strParam - строковый параметр
	void AddParam( std::string strParam );

	//Добавить IP - адрес в пакет
	void AddAddress( std::string strAddress );

	//Добавить метку конца пакета
	void EndCommand();

	//Очистить пакет
	void Clear();

	//Получить адрес буфера для отправки пакета(адрес данных подготовленного пакета)
	void GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize );

	//Установить данные для разбора
	void SetBuffer( IN BYTE* pbBuffer, IN int iSize );

	//Получить идентификатор команды по текущему смещению в пакете
	void GetCommandId( BYTE& pByte );

	//Получить массив байт из пакета по текущему смещению
	void GetParam( BYTE* pbValue, int iSize );

	//Получить параметр типа DWORD по текущему смещению
	void GetParam( DWORD& dwValue );

	//Получить строку длиной iSize из пакета по текущему смещению
	void GetParam( std::string&	strValue, int iSize );

	//Получить IP-адрес из пакета по текущему смещению
	void GetAddress( std::string& strAddress );
	
	//Выяснить закончена обработка пакета или нет
	bool IsDone();

	//Отправить пакет в сокет
	//Синтаксис: CSocket sock;CPacket packet; sock << packet;
	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	//Добавить массив байт к пакету
	void Push( const BYTE *pbData, int iSize );

	//Получить массив байт из пакета
	void Pop( BYTE *pbBuf, int iCount );

	//Внутренний буфер,содержащий пакет
	BYTE* m_pbBuf;

	//Размер данных,содержащихся в буфере
	int	m_iDataSize;

	//Кол-во памяти,отведенной под буфер
	int m_iBufSize;
	
	//Текущее смещение от начала пакета
	int m_iOffset;
};

//Отправить пакет в сокет
//Синтаксис: CSocket sock;CPacket packet; sock << packet;
CPacket& operator <<( CSocket& sock, CPacket& packet );

#endif
