//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: enumCommands type
//Author: Parshin Dmitry
//Description: Набор команд,используемый планировщиком и агентом
//-------------------------------------------------------------------------------------//

#ifndef COMMANDS_H_
#define COMMANDS_H_

enum enumCommands{
	GET_STATUS = 0x01,
	START_SCAN = 0x02,
	GET_DATA = 0x03,
	STOP_SCAN = 0x04
};

enum enumAgentResponse{
	RESP_OK = 0x00,
	RESP_INPUT_DATA_ERR,
	RESP_PROC_ERR
};

enum enumAgentState{
	Idling = 0x01,
	Scanning = 0x02,
	SendingData = 0x03,
	SendingStatus = 0x04
};

enum enumAgentEvents{
	ScanComplete = 0x01
};

#endif
