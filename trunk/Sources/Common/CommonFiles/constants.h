//------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: enumCommands type
//Author: Parshin Dmitry
//Description: �������ޜ��ݜ��������М��ݜ������, ���؜�����ߜ��ޜ��ۜ�����ל�����՜��ܜ������ ���Ҝ��� ���Ҝ�����՜��� ���ߜ������ޜ��՜��ڜ������
//-------------------------------------------------------------------------------------//

#define PLUGIN_PATH	"Plugins"
//�������М��ל��ܜ��՜��� ���ߜ������؜��՜��ܜ��ݜ��ޜ��Ӝ��� ���ќ��������՜�������(������ޜ��ڜ��՜�����ޜ���)
#define RECEIVE_BUF_START_SIZE 255
#define RECEIVE_BUF_MAX_SIZE 25000000


//�������ޜ��ݜ��������М��ݜ������ ���ߜ������ޜ�����ޜ��ڜ��ޜ��ۜ��� ���ޜ��ќ��ܜ��՜��ݜ��� �������ۜ��М��ݜ��؜������ޜ��Ҝ�����؜��ڜ��� ���� ���М��Ӝ��՜��ݜ������
#define COMMAND_ID "commandid"
#define EVENT_ID "eventid"
#define FILES_COUNT "RecordsCount"
#define COMMAND_STAT "commandstatus"
#define AGENT_STATUS "agentstatus"
#define FILES_LEFT "filesleft"
#define FILE_TAG "file"
#define HOST_TAG "host"
#define ADDR_COUNT "count"
#define HOST_ADDR "ipaddress"
#define HOST_NAME "hostname"
#define PROTO_NAME "protoname"
#define FILE_DATE "filedate"
#define FILE_SIZE "filesize"
#define FILE_PATH "filepath"

//�������ޜ��ќ��������؜���
#define SCAN_COMPLETE "ScanComplete"

//�������ޜ��ܜ��М��ݜ��Ԝ���
#define GET_STATUS "GetStatus"
#define START_SCAN "StartScan"
#define GET_DATA "GetData"
#define STOP_SCAN "StopScan"

//����������Ҝ��՜������ �������Ӝ��՜��ݜ������
#define AGENT_RESP_OK "Ok"
#define AGENT_RESP_INPUT_DATA_ERR "InputError"
#define AGENT_RESP_PROC_ERR "ProcessingError"

//�������ޜ��������ޜ�����ݜ��؜��� �������Ӝ��՜��ݜ������
#define IDLING "Idling"
#define SCANNING "Scanning"
#define SENDING_DATA "SendingData"

//�������ܜ��՜��� ���ߜ��М������М��ܜ��՜���������ޜ��� ���� ������М��ٜ��ۜ��� Settings.ini
#define TIMER_VALUE	"TimerValue"
#define LOG_LEVEL	"LogLevel"
#define SCHEDULER_ADDRESS "SchedulerAddress"
#define AGENT_LIST "AgentList"
#define SCAN_AREA "ScanArea"
#define DB_FILE "DBFileName"
#define DB_HOST "DBHost"
#define DB_NAME "DBDataBaseName"
#define DB_USER "DBUserName"
#define DB_PASSWORD "DBPassword"
#define DB_AUTOINDEX_WORD "DBAutoIndexWord"
#define SCAN_THREADS_COUNT "ScanThreadsCount"
#define DB_PROV_NAME "DbProviderLib"
#define PING_ON "CheckAvailPing"
#define RESOLVE_HOST "ResolveHostName"
#define LISTEN_PORT	"ListenPort"
#define SCHEDULER_EVENT_PORT		"SchedulerEventPort"
#define EVENT_PORT "EventPort"
#define AGENT_LISTEN_PORT "AgentListenPort"
#define POLLING_INTERVAL "PollingInterval"
#define ONLY_ACT "OnlyActive"
#define ONLY_PATH_SRCH "OnlyPathSearch"
#define TYPE_EXT "TypeExtension"
#define TYPE_CAPT "TypeExtensionCaption"
#define SCRIPT_PATH "ScriptPath"
#define QUERY_TEXT "QueryText"
