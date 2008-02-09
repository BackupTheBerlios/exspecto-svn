//------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: enumCommands type
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Þœô¨Ýœô¨áœô¨âœô¨Ðœô¨Ýœô¨âœô¨ë, œô¨Øœô¨áœô¨ßœô¨Þœô¨Ûœô¨ìœô¨×œô¨ãœô¨Õœô¨Üœô¨ëœô¨Õ œô¨Òœô¨Þ œô¨Òœô¨áœô¨Õœô¨Ü œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Õ
//-------------------------------------------------------------------------------------//

#define PLUGIN_PATH	"Plugins"
//œô¨Àœô¨Ðœô¨×œô¨Üœô¨Õœô¨à œô¨ßœô¨àœô¨Øœô¨Õœô¨Üœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨Ñœô¨ãœô¨äœô¨Õœô¨àœô¨Ð(œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Þœô¨Ò)
#define RECEIVE_BUF_START_SIZE 255
#define RECEIVE_BUF_MAX_SIZE 25000000


//œô¨ºœô¨Þœô¨Ýœô¨áœô¨âœô¨Ðœô¨Ýœô¨âœô¨ë œô¨ßœô¨àœô¨Þœô¨âœô¨Þœô¨Úœô¨Þœô¨Ûœô¨Ð œô¨Þœô¨Ñœô¨Üœô¨Õœô¨Ýœô¨Ð œô¨¿œô¨Ûœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨éœô¨Øœô¨Úœô¨Ð œô¨Ø œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð
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

//œô¨Áœô¨Þœô¨Ñœô¨ëœô¨âœô¨Øœô¨ï
#define SCAN_COMPLETE "ScanComplete"

//œô¨ºœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ôœô¨ë
#define GET_STATUS "GetStatus"
#define START_SCAN "StartScan"
#define GET_DATA "GetData"
#define STOP_SCAN "StopScan"

//œô¨¾œô¨âœô¨Òœô¨Õœô¨âœô¨ë œô¨°œô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð
#define AGENT_RESP_OK "Ok"
#define AGENT_RESP_INPUT_DATA_ERR "InputError"
#define AGENT_RESP_PROC_ERR "ProcessingError"

//œô¨Áœô¨Þœô¨áœô¨âœô¨Þœô¨ïœô¨Ýœô¨Øœô¨ï œô¨°œô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð
#define IDLING "Idling"
#define SCANNING "Scanning"
#define SENDING_DATA "SendingData"

//œô¨¸œô¨Üœô¨Õœô¨Ý œô¨ßœô¨Ðœô¨àœô¨Ðœô¨Üœô¨Õœô¨âœô¨àœô¨Þœô¨Ò œô¨Ò œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Õ Settings.ini
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
