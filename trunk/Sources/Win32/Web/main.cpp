#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include "CDBProvider.h"
#include "SettingsContainer.h"

using namespace std;

typedef map<string, string> TStrParams;
TStrParams QueryStr;
typedef list<string> TExts;
map<string,TExts> Filters;
static char* pParamTypes[] = {
	LOG_LEVEL, "int",
	ONLY_ACT, "int",
	ONLY_PATH_SRCH,"int",
	TYPE_EXT,"string"
};
int iOnlyPath=-1; // -1:disable;
int iOnlyAct=-1;
string sExt="";

#define ScriptPath "http://www.exspecto.ru/cgi-bin/web.exe?sqrt"
string TmpBuf1;
string TmpBuf2;

void SetVars(); //Заполняет ассоциативный массив значениями полученными из формы
string $(const string& aName); //Возвращает значения переменной
bool IsSet(const string& aName); //Проверяет наличие переменной
void ConvChars(string& aStr); // Конвертирует полученную строку в ANSI
void Spliter(const string& aStr); // Разбивает строку полученную из формы на переменные и добавляет их в ассоциативный массив
void CrFormSq(string& aBuf); // Возвращет строку содержащую HTML код формы запроса
void CrMenu(string& aBuf); // Возвращет строку содержащую HTML код меню
void CrMainForm(string& aBuf); // Возвращет строку содержащую HTML шаблон документа состоящий из 2-х контейнеров, для меню и для формы
void LoadFilter(); // Загружает из строки установок список фильтров для поиска
void CrResult(string& aBuf); // Возвращает строку с результатами поиска

bool TemplateLoad(const string& aFileName, string& aText);
void TemplateAssignVars(string& aCode, const TStrParams& aArr);
void TemplateAssignVar(string& aCode, const string& aName, const string& aVal);
void TemplateAssignBlockVars(string& aText, const string& aName, const vector<TStrParams>& aArr);

int main(int argc, char *argv[], char *envp[])
{
	int iLogLevel;
	//Инициализация установок и логера
	Log::instance().SetModuleName( "Web" );
	Settings::instance().SetModule( "Web", pParamTypes, sizeof( pParamTypes )/sizeof( pParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );	
	Settings::instance().GetParam( ONLY_ACT, iOnlyAct );
	Settings::instance().GetParam( ONLY_PATH_SRCH, iOnlyPath );
	Settings::instance().GetParam( TYPE_EXT, sExt );

	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );

  int i;
  string sFormat, sMenu, sCount;
  char tmp[12];

	try
	{
		Log::instance().Trace( 150, "%s:: Load list filters", __FUNCTION__ );
		LoadFilter();
		Log::instance().Trace( 150, "%s:: Load envirement to array", __FUNCTION__ );
		SetVars();
		Log::instance().Trace( 150, "%s:: Create menu", __FUNCTION__ );
	  CrMenu(sMenu);
		Log::instance().Trace( 150, "%s:: Create stamp HTML document", __FUNCTION__ );
	  CrMainForm(sFormat);
		Log::instance().Trace( 150, "%s:: Create form query", __FUNCTION__ );
		CrFormSq(sCount);
	
		Log::instance().Trace( 150, "%s:: If query fielt not empty, search", __FUNCTION__ );
		if(argc>1 && _stricmp(argv[1],"sqrt") == 0 && IsSet("SqrtText") && $("SqrtText") != "")
		{
			CrResult(sCount);
		}
// DEBUG //////////////////////////////////////////////////////////////////////
//		CrResult(sCount);
///////////////////////////////////////////////////////////////////////////////

		Log::instance().Trace( 150, "%s::Debug information::Exit function CrResult", __FUNCTION__ );

		sCount+="<p>Аргументов ";
		sCount+=itoa(argc,tmp,10);
		sCount+="\n";
		for(i = 0; i<argc; i++)
		{
			sCount+="<br>[";
			sCount+=itoa(i,tmp,10);
			sCount+="]: ";
			sCount+=argv[i];
			sCount+="\n";
		}
		i=0;
		sCount+="<p>Строка окружения:";
		while(envp[i]!=NULL)
		{
			sCount+="<br>[";
			sCount+=itoa(i+1,tmp,10);
			sCount+="]: ";
			sCount+=envp[i];
			sCount+="\n";
			i++;
		}
		sCount+="<p>Буфер: ";
		sCount+=TmpBuf1;
		sCount+="<br />";
		sCount+=TmpBuf2;
	
		TStrParams::iterator p = QueryStr.begin();
		for(;p!=QueryStr.end();++p)
		{
			sCount+="<br />[";
			sCount+=p->first;
			sCount+="]: ";
			sCount+=p->second;
		}	
	}catch(...){
		sCount+="ERROR!!!";
	}
		sCount+="<br />Ver.:08052007.1508";

		printf(sFormat.c_str(), sMenu.c_str(), sCount.c_str());
//*/
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
	return 0;
}

void CrResult(string& aBuf)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );

	typedef void* __declspec(dllimport) ProvType();

	bool bResult;
	CDBProvider* db = NULL;
	HINSTANCE hLib = NULL;
	ProvType *ProvFn;
	hostRecords rec;
	map<string,bool> aParams;
	map<string,TExts>::iterator p;
	hostRecords::iterator idRec;
	TExts::iterator l;
	string tstr, hname, hip;
	int i;
  char tmp[255];
  struct tm *timecr;
  TStrParams StrPar;
  vector<TStrParams> ArrayStrPar;
  
	try
	{
		Log::instance().Trace( 150, "%s:: Load Library", __FUNCTION__ );
		if( NULL == ( hLib = ::LoadLibraryA( "DBSQLite3" ) ) )
		{
			Log::instance().Trace( 10, "%d", GetLastError() );
			aBuf+="ERROR[";
			aBuf+=itoa(GetLastError(),tmp,10);
			aBuf+="]:: Load Library DBSQLite3";
			throw;
		}
		Log::instance().Trace( 150, "%s:: Load Main Function", __FUNCTION__ );
		if( NULL == ( ProvFn = (ProvType *)::GetProcAddress( hLib, "RegisterPlugin" ) ) )
		{
			Log::instance().Trace( 10, "не удалось получить адрес функции RegisterPlugin из библиотеки DBSQLite" ); 
			::FreeLibrary( hLib );
			hLib=NULL;
			aBuf+="ERROR[";
			aBuf+=itoa(GetLastError(),tmp,10);
			aBuf+="]:: не удалось получить адрес функции RegisterPlugin из библиотеки DBSQLite";
			throw;
		}
		Log::instance().Trace( 150, "%s:: Call Class Constructor", __FUNCTION__ );
		db = (CDBProvider*)ProvFn();
		if(db == NULL)
		{
			Log::instance().Trace( 10, "не удалось получить адресс класса провайдера из библиотеки DBSQLite" ); 
			aBuf+="ERROR[";
			aBuf+=itoa(GetLastError(),tmp,10);
			aBuf+="]:: не удалось получить адресс класса провайдера из библиотеки DBSQLite";
			throw;
		}
		Log::instance().Trace( 150, "%s:: Set params search", __FUNCTION__ );
	  aParams["IndexFind"]= false; // Отключение поиска по таблице индексированых слов
		if(Filters.size() > 0)
			for(p = Filters.begin();p!=Filters.end();++p)
				if(IsSet(p->first) && $(p->first) != "")
					for(l=p->second.begin();l!=p->second.end();++l)
					{
						tstr = "$EXT$";
						tstr += l->c_str();
						aParams[tstr]=true;
					}

		Log::instance().Trace( 190, "%s::DEBUG::Plugin name: %s", __FUNCTION__, db->GetNamePlugin() );
//Log::instance().Trace( 150, "%s:: Debug, print plugin name", __FUNCTION__ );
//printf("%s",db->GetNamePlugin());
		Log::instance().Trace( 150, "%s:: Call function Search", __FUNCTION__ );
		bResult = db->Search($("SqrtText"), aParams, rec);
		Log::instance().Trace( 150, "%s:: Test by error", __FUNCTION__ );
		i=db->GetProvError(tstr);
		Log::instance().Trace( 180, "%s:: Проверка кода ошибки", __FUNCTION__ );
		if( i != RESULT_OK )
		{
			Log::instance().Trace( 10, "ERROR[%d]::%s", i, tstr.c_str() ); 
			aBuf+="ERROR[";
			aBuf+=itoa(i,tmp,10);
			aBuf+="]:: ";
			aBuf+=tstr;
			throw;
		}
//*/
		Log::instance().Trace( 150, "%s:: If result then made table results", __FUNCTION__ );
		if( bResult )
		{
			list<fileStr>::iterator idF;
			aBuf+="<table width= \"100%\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\">\n";
			aBuf+="  <caption align=\"top\">\n";
			aBuf+="    Результат поиска:\n";
			aBuf+="  </caption>\n";
			aBuf+="  <tr>\n";
			aBuf+="    <th scope=\"col\">Имя</th>\n";
			aBuf+="    <th scope=\"col\">Хост</th>\n";
			aBuf+="    <th scope=\"col\">Путь</th>\n";
			aBuf+="    <th scope=\"col\">Размер</th>\n";
			aBuf+="    <th scope=\"col\">Дата</th>\n";
			aBuf+="  </tr>\n";
	  	for (idRec=rec.begin(); idRec != rec.end(); ++idRec)
			{
				hname = idRec->HostName;
				hip = idRec->IPNum;
				for (idF = (*idRec).Files.begin(); idF != (*idRec).Files.end(); ++idF)
				{
					aBuf+="  <tr>\n"; // начало ряда
					aBuf+="    <td><a href=\"//"; // имя
					aBuf+=hip;
					aBuf+="\">";
					aBuf+=hname;
					aBuf+="</a></td>\n";
					aBuf+="    <td><a href=\"//"; // адрес
					aBuf+=hip;
					aBuf+="\">";
					aBuf+=hip;
					aBuf+="</a></td>\n";
					aBuf+="    <td><a href=\"//"; // путь
					aBuf+=hip;
					aBuf+="/";
					aBuf+=idF->FileName;
					aBuf+="\">";
					aBuf+=idF->FileName;
					aBuf+="</a></td>\n";
					aBuf+="    <td>"; // размер
					aBuf+=_ui64toa(idF->FileSize,tmp,10);
					aBuf+="</td>\n";
					aBuf+="    <td>"; // дата
					timecr = gmtime(&(idF->FDate.UTS));
					strftime(tmp,254,"%d.%m.%Y %H:%M",timecr);
					aBuf+=tmp;
					aBuf+="</td>\n";
					aBuf+="  </tr>\n";
				}
			}
			aBuf+="</table>\n";
/*			
	  	for (idRec=rec.begin(); idRec != rec.end(); ++idRec)
			{
				hname = idRec->HostName;
				hip = idRec->IPNum;
				for (idF = (*idRec).Files.begin(); idF != (*idRec).Files.end(); ++idF)
				{
					StrPar.clear();
					StrPar["CAP_NAMEHOST"] = hname;
					StrPar["CAP_IPNUM"] = hip;
					tstr = "//";
					tstr+= hip.c_str();
					StrPar["URL_HOST"]=tstr;
					tstr+="/";
					tstr+= idF->FileName.c_str();
					StrPar["URL_LINK"]=tstr;
					StrPar["CAP_FILESIZE"]=_ui64toa(idF->FileSize,tmp,10);
					timecr = gmtime(&(idF->FDate.UTS));
					strftime(tmp,254,"%d.%m.%Y %H:%M",timecr);
					StrPar["CAP_FILEDATE"]=tmp;
					ArrayStrPar.push_back(StrPar);
				}
			}

			if(TemplateLoad("Result.tpl", tmp))
				TemplateAssignBlockVars(tmp, "restabrow", ArrayStrPar);
			aBuf+=tmp;//*/
		}else{aBuf+="Ничего не найдено.";}
//*/		
	}catch(...){aBuf+="Произошла ошибка, дальнейшее выполнение скрипта невозможно. Оббратитесь к админестрации для устранения ошибки.";}

//printf("%s", aBuf.c_str());

	Log::instance().Trace( 150, "%s:: Free class and library", __FUNCTION__ );
	if(db!=NULL)
		delete db;
	if(hLib!=NULL)
		::FreeLibrary( hLib );
//*/
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

string $(const string& aName)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	TStrParams::iterator p = QueryStr.find(aName.c_str());
	if(p != QueryStr.end())return p->second;
	return "";
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
} 

bool IsSet(const string& aName)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	TStrParams::iterator p = QueryStr.find(aName.c_str());
	if(p != QueryStr.end())return true;
	return false;
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

void LoadFilter()
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	Filters.clear();
	if(sExt == "") return;
	
	TExts lExt;
	string Capt;
	string tmp;
  int pStart, i, exS, exE;
  bool bEndExt = false;
  pStart=0;
  i=pStart;
  while(i < (signed)sExt.length())
  {
    switch(sExt[i])
    {
    	case '\\':
	   		i++;
    		break;
   		case '\t':
   			pStart = i+1;
   			break;
   		case '|':
   			Capt = sExt.substr(pStart, i-pStart);
   			i++;
   			pStart = i;
   			while(i <= (signed)sExt.length())
   			{
   				switch(sExt[i])
   				{
   					case '\\':
			  	 		i++;
    					break;
			   		case '\t':
			   		case 0:
			   			bEndExt = true;
   					case ';':
   						tmp=sExt.substr(pStart, i-pStart);
   						exS=tmp.find_first_not_of(" ",0);
   						exE=tmp.find_last_not_of(" ",0);
			   			lExt.push_back(tmp.substr(exS, exE-exS+1));
			   			pStart = i+1;
  	  				break;
   				}
   				i++;
   				if(bEndExt)break;
   			}
   			break;
  	};
  	if(bEndExt)
  	{
  		Filters[Capt]=lExt;
  		lExt.clear();
  		bEndExt = false;
  	}
    i++;
	}
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

void SetVars()
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	QueryStr.clear();
	char *s=getenv("REQUEST_METHOD");
  unsigned int contentlength;
  string buf="";
    
  const char *len1 = getenv("CONTENT_LENGTH");

  if(s!=NULL)
  {
	  if(strcmp(s,"GET")==0)
    {
    	buf = getenv("QUERY_STRING");
    }else if(strcmp(s,"POST")==0)
    {
	    contentlength=strtol(len1, NULL, 10);
	    char* tmp;
	    try
	    {
	    	tmp = new char[contentlength+5];
	    	ZeroMemory(tmp,contentlength+5);
      	fread(tmp, contentlength, 1, stdin);
      	buf = tmp; 
    		delete[] tmp;
    	}catch(...){
    		delete[] tmp;
	    }
  	}
  }
  TmpBuf1=buf;
  ConvChars(buf);
  TmpBuf2=buf;
  Spliter(buf);
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

// Функция изменяет передаваемую строку
void ConvChars(string& aStr) 
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
  unsigned int i;
  string tmp;
  tmp="";
  for (i = 0; i < aStr.length(); i++)
	{
  	switch (aStr[i]) 
    {
    	case '+':
      	tmp+=' ';
        break;
      case '%':
        if( (aStr.substr(i+1,2) == "26") || (aStr.substr(i+1,2) == "3D") )
        	tmp+='\\';
        tmp+= (char)strtol(aStr.substr(i+1,2).c_str(), NULL, 16);
	      i+=2;
        break;
			default:
      	tmp+= aStr[i];
        break;
    }
  }
  aStr=tmp;
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

void Spliter(const string& aStr)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
  int pStart, pEnd, pDel, i;
  pStart=0;
  pDel=0;
  pEnd=0;
  i=pStart;
  while(pStart < (signed)aStr.length())
  {
    switch(aStr[i])
    {
    	case '\\':
	   		i++;
    		break;
   		case '&':
   			pEnd = i;
   			break;
   		case '=':
   			pDel = i;
   			break;
   		case '"':
   		  pEnd = aStr.find_first_of("\"",i+1);
   		  if( pEnd >= 0 && pEnd < (signed)aStr.length() )
   		  	i=pEnd;
  	  	pEnd=pStart;
  	  	break;
   		case '\'':
   		  pEnd = aStr.find_first_of("'",i+1);
   		  if( pEnd >= 0 && pEnd < (signed)aStr.length() )
   		  	i=pEnd;
  	  	pEnd=pStart;
  	  	break;
  	};
    i++;
    if(i >= (signed)aStr.length() && pEnd == pStart)
    	pEnd = i;
    if(pEnd != pStart)
    {
    	if(pDel <= pStart || pDel > pEnd)
    		pDel = pEnd;
    	QueryStr[aStr.substr(pStart, pDel-pStart)]=(pDel==pEnd)?"":aStr.substr(pDel+1, pEnd-pDel-1);
    	pStart = pEnd+1;
    	pEnd = pStart;
    	i=pStart;
    }
	}
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

void CrMenu(string& aBuf)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	aBuf = "&nbsp;";
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

void CrFormSq(string& aBuf)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	map<string,TExts>::iterator p = Filters.begin();
	int iTabId=1;
	char tmp[12];
	aBuf= "<form  action='";
	aBuf+=ScriptPath;
	aBuf+="' method='POST' title='Поиск'>\n";
	aBuf+="  <p>Поиск: <input type='text' name='SqrtText' size='80' maxlength='254' alt='Поиск' tabindex='0' value='";
	if(IsSet("SqrtText") && $("SqrtText") != "")
		aBuf+=$("SqrtText");
	aBuf+="' /><br />\n";
	if(iOnlyPath>=0)
	{
		aBuf+="  <input type='checkbox' name='";
		aBuf+=ONLY_PATH_SRCH;
		aBuf+="' alt='Искать только в названиях каталогов' tabindex='";
		aBuf+=itoa(iTabId, tmp, 10);
		iTabId++;
		aBuf+="'";
		if(IsSet(ONLY_PATH_SRCH) && $(ONLY_PATH_SRCH) != "")
			aBuf+=" checked='checked'";
		aBuf+=" />Искать в названиях каталогов<br />";
	}
	if(Filters.size() > 0)
	{
		aBuf+="  Тип искомых файлов:<br />";
		for(;p!=Filters.end();++p)
		{
			aBuf+="  <input type='checkbox' name='";
			aBuf+=p->first;
			aBuf+="' alt='' tabindex='"; // Добавить список заглавий и ставить в соответствие фильтрам
			aBuf+=itoa(iTabId, tmp, 10);
			iTabId++;
			aBuf+="'";
			if(IsSet(p->first) && $(p->first) != "")
				aBuf+=" checked='checked'";
			aBuf+=" />";
			aBuf+=p->first;
			aBuf+=" &nbsp;";
		}
		aBuf+="<br />";
	}
	if(iOnlyAct>=0)
	{
		aBuf+="  <input type='checkbox' name='";
		aBuf+=ONLY_ACT;
		aBuf+="' alt='Выводить только активные ссылки' tabindex='";
		aBuf+=itoa(iTabId, tmp, 10);
		iTabId++;
		aBuf+="'";
		if(IsSet(ONLY_ACT) && $(ONLY_ACT) != "")
			aBuf+=" checked='checked'";
		aBuf+=" />Выводить только активные ссылки";
	}
	aBuf+="  </p>";
	aBuf+="  <input type='reset' value='Очистить форму' tabindex='";
	aBuf+=itoa(iTabId, tmp, 10);
	iTabId++;
	aBuf+="'>&nbsp;";
	aBuf+="  <input type='submit' name='BtnPost' value='Поиск' tabindex='";
	aBuf+=itoa(iTabId, tmp, 10);
	iTabId++;
	aBuf+="'>";
	aBuf+="</form>";
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

void CrMainForm(string& aBuf)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	aBuf= "Content-Type: text/html\r\n\r\n";
	aBuf+="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";
	aBuf+="<html xmlns=\"http://www.w3.org/1999/xhtml\">";
	aBuf+="<head>";
	aBuf+="<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\" />";
	aBuf+="<title>Untitled Document</title>";
	aBuf+="</head>";

	aBuf+="<body>";
	aBuf+="<div width=\"100%\"><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
	aBuf+="  <tr><td><div class=\"Menu\"> %s </div></td><td><div class=\"SearchCon\"> %s </div></td></tr>";
	aBuf+="</table></div>";
	aBuf+="</body>";
	aBuf+="</html>";
	Log::instance().Trace( 100, "%s [exit]", __FUNCTION__ );
}

bool TemplateLoad(const string& aFileName, string& aText)
{
	FILE *stream;
	char tmp[1024];
	int i;
	
	aText = "";
  if( (stream  = fopen( aFileName.c_str(), "r" )) == NULL )
  {
		Log::instance().Trace( 50, "%s::File template %s don't open!", __FUNCTION__, aFileName.c_str() );
		aText = "File template ";
		aText += aFileName.c_str();
		aText += " don't open!";
    return false;
  }
  
  while( !feof(stream) )
  {
  	fread(tmp, sizeof(char), sizeof(tmp), stream);
 		if( (i=ferror(stream)) )
 		{
			Log::instance().Trace( 50, "%s::ERROR[%d] Read file.", __FUNCTION__, i );
			aText = "Error read file!";
			return false;
 		}
  	aText += tmp;	
  }
  
  if( fclose( stream ) )
		Log::instance().Trace( 50, "%s::File template %s don't close!", __FUNCTION__, aFileName.c_str() );

	return true;
}

void TemplateAssignVars(string& aCode, const TStrParams& aArr)
{
	TStrParams::const_iterator p;
	for(p = aArr.begin(); p != aArr.end(); ++p)
	{
		TemplateAssignVar(aCode, p->first, p->second);
	}
}

void TemplateAssignVar(string& aCode, const string& aName, const string& aVal)
{
	string::size_type iStart;
	string tmp;
	tmp = "{";
	tmp+= aName.c_str();
	tmp+= "}";
	iStart = 0;
	while( (iStart = aCode.find(aName.c_str(),iStart)) != string::npos )
		aCode.replace(iStart, tmp.length(), aVal.c_str());
}
 
void TemplateAssignBlockVars(string& aText, const string& aName, const vector<TStrParams>& aArr)
{
	string rep, cur, tmp;
	string::size_type stRep, stTmp;
	TStrParams::const_iterator p;
	
	cur = "<!-- BEGIN ";
	cur+= aName.c_str();
	cur+= " -->\n";
	if( (stRep = aText.find(cur.c_str(),0)) == string::npos ) return;
	stRep+=cur.length();
	cur = "<!-- END ";
	cur+= aName.c_str();
	cur+= " -->\n";
	if( (stTmp = aText.find(cur.c_str(),stRep)) == string::npos ) return;
	rep = aText.substr(stRep, stTmp-stRep);
	aText.erase(stRep, stTmp-stRep);
	
	for(unsigned int i=0; i < aArr.size(); i++)
	{
		cur = rep.c_str();
		for(p = aArr[i].begin(); p != aArr[i].end(); ++p)
		{
			tmp = aName.c_str();
			tmp+= ".";
			tmp+= p->first;
			TemplateAssignVar(cur, tmp, p->second);
		}
		aText.insert(stRep, cur.c_str());
		stRep+=cur.length();
	}
}

 