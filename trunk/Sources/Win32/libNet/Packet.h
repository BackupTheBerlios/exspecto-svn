//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#ifndef PACKET_H_
#define PACKET_H_

#include "precomp.h"
#include "Socket.h"
#include "../CommonFiles/commands.h"

class CPacket
{
public:
	//Классы исключений, генерируемыз CPacket
	class PacketErr: public std::runtime_error
	{
	public:
		PacketErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PacketErr()throw(){};
	};

	class PacketFormatErr: public PacketErr
	{
	public:
		PacketFormatErr( const std::string& strErr )throw():PacketErr( strErr ){};
		virtual ~PacketFormatErr()throw(){};
	};

	CPacket(void)throw( PacketErr );
	~CPacket(void);

	//Добавить в пакет идентификатор команды
	void BeginCommand( enumCommands Command )throw( PacketErr );

	//Добавить параметр
	//	pbParam - буфер с данными
	//	iSize - размер данных
	void AddParam( BYTE* pbParam, int iSize )throw( PacketErr );

	//Добавить параметр
	//	dwParam - параметр типа DWORD
	void AddParam( DWORD dwParam )throw( PacketErr );

	//Добавить параметр
	//	strParam - строковый параметр
	void AddParam( std::string strParam )throw( PacketErr );

	//Добавить IP - адрес в пакет
	void AddAddress( std::string strAddress )throw( PacketErr, PacketFormatErr );

	//Добавить метку конца пакета
	void EndCommand()throw( PacketErr );

	//Очистить пакет
	void Clear();

	//Получить адрес буфера для отправки пакета(адрес данных подготовленного пакета)
	void GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize );

	//Установить данные для разбора
	void SetBuffer( IN BYTE* pbBuffer, IN int iSize )throw( PacketErr );

	//Получить идентификатор команды по текущему смещению в пакете
	void GetCommandId( BYTE& pByte )throw( PacketFormatErr );

	//Получить массив байт из пакета по текущему смещению
	void GetParam( BYTE* pbValue, int iSize )throw( PacketFormatErr );

	//Получить параметр типа DWORD по текущему смещению
	void GetParam( DWORD& dwValue )throw( PacketFormatErr );

	//Получить строку длиной iSize из пакета по текущему смещению
	void GetParam( std::string&	strValue, int iSize )throw( PacketFormatErr );

	//Получить IP-адрес из пакета по текущему смещению
	void GetAddress( std::string& strAddress )throw( PacketFormatErr );

	//Отправить пакет в сокет
	//Синтаксис: CSocket sock;CPacket packet; sock << packet;
	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	//Добавить массив байт к пакету
	void Push( BYTE *pbData, int iSize )throw( PacketErr );

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
