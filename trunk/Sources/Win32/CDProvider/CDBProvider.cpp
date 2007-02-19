/////////////////////////////////////////////////////////////////////////////
// Схема базы данных                                                       //
//                                                                         //
// TableHostes  таблица описывает найденые в сети компьютеры               //
//   IDhost        INT            Индекс компьютера                        //
//   IPNum         CHAR(15)       IP компьютера                            //
//   HostName      TINYTEXT       Имя компьютера                           //
//   DateRef       DATETIME       Время последненго обновления информации  //
// ----------------------------------------------------------------------- //
// TableFiles   описание файлов и привязка их к компьютеру                 //
//   IDfile        INT            Индекс файла                             //
//   IDhost        INT            Индекс компьютера                        //
//   FileName      TINYTEXT       Имя файла                                //
//   FileSize      INT            Размер файла                             //
//   FileTimeCr    DATETIME       Время создания файла                     //
// ----------------------------------------------------------------------- //
// TableWords   таблица всех найденых слов                                 //
// { Пути к файлам разбиваются на слова (идиомы) и заносятся в таблицу.  } //
// { Это сделано для ускорения поиска выражений.                         } //
//   IDword        INT            Индекс слова                             //
//   Word          TINYTEXT       "Идиома" (слово)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   соответствие слов файлам и флаг принадлежности слова //
//   IDword        INT            Индекс слова                             //
//   IDfile        INT            Индекс файла                             //
//   IsPath        BOOL           "Идиома" - часть пути?                   //
/////////////////////////////////////////////////////////////////////////////

#include "CDBProvider.h"

CDBProvider::CDBProvider(const char* szFile)
{
	db.open(szFile); // Открываю файл базы данных
	// Создаю таблицы при их отсутствии
	if(!db.tableExists("TableHostes"))
		db.execDML("create table TableHostes(IDhost AUTO_INCREMENT INT NOT NULL, IPNum char(15) NOT NULL, HostName tinytext, DateRef datetime, PRIMARY KEY (IPNum));");
	if(!db.tableExists("TableFiles"))
		db.execDML("create table TableFiles(IDfile AUTO_INCREMENT INT NOT NULL, IDhost int NOT NULL, FileName tinytext NOT NULL, FileSize int, FileTimeCr datetime, PRIMARY KEY (IDfile));");
	if(!db.tableExists("TableWords"))
		db.execDML("create table TableWords(IDword AUTO_INCREMENT INT NOT NULL, Word tinytext NOT NULL, PRIMARY KEY (Word));");
	if(!db.tableExists("TableWordInFiles"))
		db.execDML("create table TableWordInFiles(IDword INT NOT NULL, IDfile INT NOT NULL, IsPath BOOL NOT NULL DEFAULT false, PRIMARY KEY (IDword));");
}

CDBProvider::~CDBProvider()
{
}

char* CDBProvider::GetTimeStr(time_t iTime, char* strRes)
{
	struct tm *nt;
	time_t iTm;
	if (iTime == 0){
		time( &iTm );}
	else{ iTm = iTime;}
	nt = gmtime( &iTm );
	sprintf(strRes, "%.4d.%.2d.%.2d %.2d:%.2d:%.2d\x0",nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday,
					nt->tm_hour, nt->tm_min, nt->tm_sec);
	return strRes;
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddFile(fileStr* fs, const char* aHostName, const char* aIPnum)
// Тестовая функция, в реализации присутствовать не будет
/*void CDBProvider::AddFile(fileStr* fs, const char* aHostName, const char* aIPnum)
{
	int i=-1;
	char strTmp[255];
    CppSQLite3Buffer bufSQL;
    printf("%s\n",bufSQL.format("select * from TableHostes where IPNum = %Q;", aIPnum));
	CppSQLite3Query q = db.execQuery(bufSQL);
    if(!q.eof())
    	i = q.getIntField("IDhost", -1);
    q.finalize();
    
    if(i >= 0)
    {
    	printf("%s\n",bufSQL.format("delete from TableFiles where IDhost = %d;", i));
    	db.execDML(bufSQL);

    	printf("%s\n",bufSQL.format("update TableHostes set DateRef = %Q where IDhost = %d;",GetTimeStr(0, strTmp),i));
    	db.execDML(bufSQL);
    }
    else
    {
    	if (0 != db.execScalar("select count(0) from TableHostes;"))
    		i = db.execScalar("select MAX(IDhost) from TableHostes;");
    	i++;
    	printf("%s\n",bufSQL.format("insert into TableHostes values(%d, %Q, %Q, %Q);", i, aIPnum, aHostName, GetTimeStr(0, strTmp)));
    	db.execDML(bufSQL);
    }
    
    int j = -1;
   	if (0 != db.execScalar("select count(0) from TableFiles;"))
   		j = db.execScalar("select MAX(IDfile) from TableFiles;");
   	j++;
   	printf("%s\n",bufSQL.format("insert into TableFiles values(%d, %d, %Q, %d, %Q);", j, i, fs->FileName, fs->FileSize, GetTimeStr(fs->FileTime, strTmp)));
   	db.execDML(bufSQL);
}*/
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddFiles(pFilesStr aFileList, const char* aHostName, const char* aIPnum)
// Добавляет список файлов в БД
// aFileList  однонаправленый список на основе структуры с параметрами файла
// aHostName  имя компьютера на которм находился представленый список
// aIPnum     IP адресс компьютера
void CDBProvider::AddFiles(pFilesStr aFileList, const char* aHostName, const char* aIPnum)
{
	int i=-1; // хранит ID таблицы хостов
	char strTmp[255]; // вспомогвтельный массив (введен для хранения даты в строковой форме)
	filesStr::iterator Cur; // указатель на текущий файл списка
    CppSQLite3Buffer bufSQL; // буфер для формирования запроса
    bufSQL.format("select * from TableHostes where IPNum = %Q;", aIPnum);
	CppSQLite3Query q = db.execQuery(bufSQL); // запрос
    if(!q.eof())
    	i = q.getIntField("IDhost", -1); // получаю номер ID указующий на существующую запись
    									 // с номером IP совпадающим с aIPnum
    q.finalize();
    
    if(i >= 0) // если запись существует
    {
    	bufSQL.format("delete from TableWordInFiles where IDfile = (select IDfile from TableFiles where IDhost = %d);", i);
   		db.execDML(bufSQL); // удаляю все записи соответствующего ID из таблицы (файл-слов)
	    
    	bufSQL.format("delete from TableFiles where IDhost = %d;", i);
    	db.execDML(bufSQL); // удаляю все записи соответствующего ID из таблицы файлов

    	bufSQL.format("update TableHostes set DateRef = %Q where IDhost = %d;",GetTimeStr(0, strTmp),i);
    	db.execDML(bufSQL); // обнавляю дату/время последнего обнавления
    }
    else // иначе
    {
    	if (0 != db.execScalar("select count(0) from TableHostes;"))
    		i = db.execScalar("select MAX(IDhost) from TableHostes;");
    	i++;  // определяю следующий номер ID табдицы хостов
    	bufSQL.format("insert into TableHostes values(%d, %Q, %Q, %Q);", i, aIPnum, aHostName, GetTimeStr(0, strTmp));
    	db.execDML(bufSQL); // добавляю новую запись в таблицу хостов
    }
    
    int j = -1;
   	if (0 != db.execScalar("select count(0) from TableFiles;"))
   		j = db.execScalar("select MAX(IDfile) from TableFiles;");
   	j++; // определяю следующий номер ID таблицы файлов
   	
 	for ( Cur = (*aFileList).begin(); Cur != (*aFileList).end(); Cur++ )
	{
//   		printf("insert into TableFiles values(%d, %d, %s, %d, %s);", j, i, (*Cur).FileName.c_str(),(*Cur).FileSize, GetTimeStr((*Cur).FileTime, strTmp));
   		bufSQL.format("insert into TableFiles values(%d, %d, %Q, %d, %Q);", j, i, (*Cur).FileName.c_str(),(*Cur).FileSize, GetTimeStr((*Cur).FileTime, strTmp));
   		db.execDML(bufSQL);
   		AddWord(j,(*Cur).FileName);
   		j++;
   	}
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWord(int aID, string aPath)
// Разбивает имя файла на идиомы и добавляет в список слов и соответствия файла словам
// aID    номер файла в таблице файлов
// aPath  полное имя файла
void CDBProvider::AddWord(int aID, string aPath)
{
	string sep ="\\. ";
	list<string> wrd;
	int n = aPath.rfind("\\"); //ищет первый разделительный слешь справо
	string tmp;
	if( n>=0 ) // если слешь найден по фраза правее слеша это путь
	{
		tmp = aPath.substr(0,n);
		Split(tmp, sep, wrd, null); 
		AddWordInTable(aID, wrd, true);
	}

	if( (n+1)<aPath.size() ) //если полск слеша есть символы то ето имя файла
	{
		tmp = aPath.substr(n+1,aPath.size()-(n+1));
		Split(tmp, sep, wrd); 
		AddWordInTable(aID, wrd, false);
	}
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Split(string & text, string & separators, list<string> & words)
// Разбивает имя файла на идиомы
// text        строка, разбиваемая на слова
// separators  символы разделители
// words       список слов изъятых из строки
// varsep      список символов состоящая из последовательно входящих разделитетей
void CDBProvider::Split(string & text, string & separators, list<string> & words, list<string> & varsep)
{
	int n = text.length();
	int start, stop;
	string temp;
	stop = 0;
	start = 0;
//	start = text.find_first_not_of(separators);
	while ((start >= 0) && (start < n))
	{
		start = stop;
		stop = text.find_first_not_of(separators, start);
    	if ((stop < 0) || (stop > n)) stop = n;
    	if (varsep != null)
    	{
    		if (stop == 0) varsep.push_back("#0#")
    		else varsep.push_back(text.substr(start, stop - start));
    	}
		start = stop;
		if (start >= n)return;
    	stop = text.find_first_of(separators, start);
    	if ((stop < 0) || (stop > n)) stop = n;
    	words.push_back(text.substr(start, stop - start));
//    	start = text.find_first_not_of(separators, stop+1);
    }
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWordInTable(int aID, list<string> & words, bool IsPath)
// Добавляет слова в таблицу слов
// aID     номер файла
// words   список слов
// IsPath  слова входят в путь к файлу
void CDBProvider::AddWordInTable(int aID, list<string> & words, bool IsPath)
{
    CppSQLite3Buffer bufSQL; // буфер для формирования запроса
    CppSQLite3Query q;
    list<string>::iterator wptr;
    int i = -1; //индекс идиомы в таблице 
  	for (wptr = words.begin(); wptr != words.end(); ++wptr)
   	{
		bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
		q = db.execQuery(bufSQL); // запрос
   		if(!q.eof())
   			i = q.getIntField("IDword", -1);
   		q.finalize();
	    if(i < 0)
	    {
   			if (0 != db.execScalar("select count(0) from TableWords;"))
   				i = db.execScalar("select MAX(IDhost) from TableWords;");
   			i++;  // определяю следующий номер
   			bufSQL.format("insert into TableWords values(%d, %Q);", i, (*wptr).c_str());
   			db.execDML(bufSQL); // добавляю новую запись в таблицу
   		}
		bufSQL.format("insert into TableWordInFiles values(%d, %d, %Q);", i, aID, IsPath);
		db.execDML(bufSQL); //добавляю новое соответсвия идиомы файлу
   	}
}
//-----------------------------------------------------------------------------
/*
bool CDBProvider::IsSet(string aName, list<string> & aParams)
{
}

string CDBProvider::GetParamV(string aName, list<string> & aParams)
{
}

bool CDBProvider::SetParam(string aName, string aVal, list<string> & aParams)
{
}
*/
///////////////////////////////////////////////////////////////////////////////
// Find(string aText, list<string> & aParams)
// функция ищет цельное выражение в базе данных
// Тоесть передаваемый текст должен соответствовать одному элементу
// запроса. Не допускается присутствие в запросе символов ",;^", отвечающими
// за разбиение строки на элементы и устнавливающие связь между ними.
// Символы кавычек, (, ), +, - и пробел являются обычными символами строки по
// которым производится поиск. ??????
bool CDBProvider::Find(string aText, map<string,bool> & aParams, list<int> & Result)
{
  CppSQLite3Buffer bufSQL; // буфер для формирования запроса
  CppSQLite3Query q;
	string sep ="\\. *?";
	list<string> wrd; // массив слов из фразы
	list<string> varsep; //массив разделяющих сепараторов
	list<string>::iterator wptr, sptr;
	list<int> IDWord;
	list<int> IDFile;
	list<int>::iterator idwPtr, idfPtr;
	string temp;
	Split(aText, sep, wrd, varsep);
// создать список слов и в случае наличия маски в начале или конце слова добавить соответствующую маску
// к началу или концу слова нового массива
  for (wptr=wrd.begin(); wptr != wrd.end(); ++wptr)
  {
    temp="";
    //привести wptr к индексу элемента массива i
    //добавить к temp i-й сепаратор или ничего если он равен #0#
      //добавляется только строка символов в которой справа налева символы
      // являются непрерывной последовательность символов маски
    //добавить к temp i-ое слово
    //добавить к temp i+1-ый сепаратор (если он существует)
      //добавляется символы являющимеся маской, до первого вхождения "немаски"
    //перезаписать i-ы элемент масива wrd новым значением из temp
  }

  for (wptr=wrd.begin(); wptr != wrd.end(); ++wptr)
  {
    bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
	  q = db.execQuery(bufSQL); // запрос
	  if(!q.eof())
	 	  i = q.getIntField("IDword", -1);
	  q.finalize();
	  if (i != -1)
	    IDWord.push_back(i);
	}
  
  idwPtr=IDWord.begin(); 
  bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
  q = db.execQuery(bufSQL); // запрос
  if(!q.eof())
  {
	  //добавляет в массив IDFiles индексы файлов
	 	//    i = q.getIntField("IDword", -1);
 	}
  for (idwPtr=IDWord.begin(); idwPtr != IDWord.end(); ++idwPtr)
  {
    bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
	  q = db.execQuery(bufSQL); // запрос
    //возможно можно пеписать так чтоб записи сравнивались с результатом
    // запроса и при ненахдении в строке ответа удалялись из масива
	  if(!q.eof())
	  {
    //добавляет индексы найденных файлы во временный массив
	 	//    i = q.getIntField("IDword", -1);
	 	}
	  q.finalize();
    //удаляет из масива IDFile записи не существующие во временом масиве
  }
}

///////////////////////////////////////////////////////////////////////////////
// Search(string aText, list<string> & aParams)
// функция разбивает выражение для поиска по элементам (Которые ищутся в базе
// данных) разделеными символами:
// ,; - делят выражение на элементы и устанавливает связь между элементами ИЛИ
// ^  - делит выражение на элементы и устанавливает связь между ними И
// выражение читается слева направо и действие над найдеными элементами
// выполняется последовательно. (приоретет операторов И и ИЛИ один)
bool CDBProvider::Search(string aText, map<string,bool> & aParams, list<int> & Result)
{
}
