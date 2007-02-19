/////////////////////////////////////////////////////////////////////////////
// ����� ���� ������                                                       //
//                                                                         //
// TableHostes  ������� ��������� �������� � ���� ����������               //
//   IDhost        INT            ������ ����������                        //
//   IPNum         CHAR(15)       IP ����������                            //
//   HostName      TINYTEXT       ��� ����������                           //
//   DateRef       DATETIME       ����� ����������� ���������� ����������  //
// ----------------------------------------------------------------------- //
// TableFiles   �������� ������ � �������� �� � ����������                 //
//   IDfile        INT            ������ �����                             //
//   IDhost        INT            ������ ����������                        //
//   FileName      TINYTEXT       ��� �����                                //
//   FileSize      INT            ������ �����                             //
//   FileTimeCr    DATETIME       ����� �������� �����                     //
// ----------------------------------------------------------------------- //
// TableWords   ������� ���� �������� ����                                 //
// { ���� � ������ ����������� �� ����� (������) � ��������� � �������.  } //
// { ��� ������� ��� ��������� ������ ���������.                         } //
//   IDword        INT            ������ �����                             //
//   Word          TINYTEXT       "������" (�����)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   ������������ ���� ������ � ���� �������������� ����� //
//   IDword        INT            ������ �����                             //
//   IDfile        INT            ������ �����                             //
//   IsPath        BOOL           "������" - ����� ����?                   //
/////////////////////////////////////////////////////////////////////////////

#include "CDBProvider.h"

CDBProvider::CDBProvider(const char* szFile)
{
	db.open(szFile); // �������� ���� ���� ������
	// ������ ������� ��� �� ����������
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
// �������� �������, � ���������� �������������� �� �����
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
// ��������� ������ ������ � ��
// aFileList  ��������������� ������ �� ������ ��������� � ����������� �����
// aHostName  ��� ���������� �� ������ ��������� ������������� ������
// aIPnum     IP ������ ����������
void CDBProvider::AddFiles(pFilesStr aFileList, const char* aHostName, const char* aIPnum)
{
	int i=-1; // ������ ID ������� ������
	char strTmp[255]; // ��������������� ������ (������ ��� �������� ���� � ��������� �����)
	filesStr::iterator Cur; // ��������� �� ������� ���� ������
    CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
    bufSQL.format("select * from TableHostes where IPNum = %Q;", aIPnum);
	CppSQLite3Query q = db.execQuery(bufSQL); // ������
    if(!q.eof())
    	i = q.getIntField("IDhost", -1); // ������� ����� ID ��������� �� ������������ ������
    									 // � ������� IP ����������� � aIPnum
    q.finalize();
    
    if(i >= 0) // ���� ������ ����������
    {
    	bufSQL.format("delete from TableWordInFiles where IDfile = (select IDfile from TableFiles where IDhost = %d);", i);
   		db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� (����-����)
	    
    	bufSQL.format("delete from TableFiles where IDhost = %d;", i);
    	db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� ������

    	bufSQL.format("update TableHostes set DateRef = %Q where IDhost = %d;",GetTimeStr(0, strTmp),i);
    	db.execDML(bufSQL); // �������� ����/����� ���������� ����������
    }
    else // �����
    {
    	if (0 != db.execScalar("select count(0) from TableHostes;"))
    		i = db.execScalar("select MAX(IDhost) from TableHostes;");
    	i++;  // ��������� ��������� ����� ID ������� ������
    	bufSQL.format("insert into TableHostes values(%d, %Q, %Q, %Q);", i, aIPnum, aHostName, GetTimeStr(0, strTmp));
    	db.execDML(bufSQL); // �������� ����� ������ � ������� ������
    }
    
    int j = -1;
   	if (0 != db.execScalar("select count(0) from TableFiles;"))
   		j = db.execScalar("select MAX(IDfile) from TableFiles;");
   	j++; // ��������� ��������� ����� ID ������� ������
   	
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
// ��������� ��� ����� �� ������ � ��������� � ������ ���� � ������������ ����� ������
// aID    ����� ����� � ������� ������
// aPath  ������ ��� �����
void CDBProvider::AddWord(int aID, string aPath)
{
	string sep ="\\. ";
	list<string> wrd;
	int n = aPath.rfind("\\"); //���� ������ �������������� ����� ������
	string tmp;
	if( n>=0 ) // ���� ����� ������ �� ����� ������ ����� ��� ����
	{
		tmp = aPath.substr(0,n);
		Split(tmp, sep, wrd, null); 
		AddWordInTable(aID, wrd, true);
	}

	if( (n+1)<aPath.size() ) //���� ����� ����� ���� ������� �� ��� ��� �����
	{
		tmp = aPath.substr(n+1,aPath.size()-(n+1));
		Split(tmp, sep, wrd); 
		AddWordInTable(aID, wrd, false);
	}
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Split(string & text, string & separators, list<string> & words)
// ��������� ��� ����� �� ������
// text        ������, ����������� �� �����
// separators  ������� �����������
// words       ������ ���� ������� �� ������
// varsep      ������ �������� ��������� �� ��������������� �������� ������������
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
// ��������� ����� � ������� ����
// aID     ����� �����
// words   ������ ����
// IsPath  ����� ������ � ���� � �����
void CDBProvider::AddWordInTable(int aID, list<string> & words, bool IsPath)
{
    CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
    CppSQLite3Query q;
    list<string>::iterator wptr;
    int i = -1; //������ ������ � ������� 
  	for (wptr = words.begin(); wptr != words.end(); ++wptr)
   	{
		bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
		q = db.execQuery(bufSQL); // ������
   		if(!q.eof())
   			i = q.getIntField("IDword", -1);
   		q.finalize();
	    if(i < 0)
	    {
   			if (0 != db.execScalar("select count(0) from TableWords;"))
   				i = db.execScalar("select MAX(IDhost) from TableWords;");
   			i++;  // ��������� ��������� �����
   			bufSQL.format("insert into TableWords values(%d, %Q);", i, (*wptr).c_str());
   			db.execDML(bufSQL); // �������� ����� ������ � �������
   		}
		bufSQL.format("insert into TableWordInFiles values(%d, %d, %Q);", i, aID, IsPath);
		db.execDML(bufSQL); //�������� ����� ����������� ������ �����
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
// ������� ���� ������� ��������� � ���� ������
// ������ ������������ ����� ������ ��������������� ������ ��������
// �������. �� ����������� ����������� � ������� �������� ",;^", �����������
// �� ��������� ������ �� �������� � �������������� ����� ����� ����.
// ������� �������, (, ), +, - � ������ �������� �������� ��������� ������ ��
// ������� ������������ �����. ??????
bool CDBProvider::Find(string aText, map<string,bool> & aParams, list<int> & Result)
{
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  CppSQLite3Query q;
	string sep ="\\. *?";
	list<string> wrd; // ������ ���� �� �����
	list<string> varsep; //������ ����������� �����������
	list<string>::iterator wptr, sptr;
	list<int> IDWord;
	list<int> IDFile;
	list<int>::iterator idwPtr, idfPtr;
	string temp;
	Split(aText, sep, wrd, varsep);
// ������� ������ ���� � � ������ ������� ����� � ������ ��� ����� ����� �������� ��������������� �����
// � ������ ��� ����� ����� ������ �������
  for (wptr=wrd.begin(); wptr != wrd.end(); ++wptr)
  {
    temp="";
    //�������� wptr � ������� �������� ������� i
    //�������� � temp i-� ��������� ��� ������ ���� �� ����� #0#
      //����������� ������ ������ �������� � ������� ������ ������ �������
      // �������� ����������� ������������������ �������� �����
    //�������� � temp i-�� �����
    //�������� � temp i+1-�� ��������� (���� �� ����������)
      //����������� ������� ����������� ������, �� ������� ��������� "�������"
    //������������ i-� ������� ������ wrd ����� ��������� �� temp
  }

  for (wptr=wrd.begin(); wptr != wrd.end(); ++wptr)
  {
    bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
	  q = db.execQuery(bufSQL); // ������
	  if(!q.eof())
	 	  i = q.getIntField("IDword", -1);
	  q.finalize();
	  if (i != -1)
	    IDWord.push_back(i);
	}
  
  idwPtr=IDWord.begin(); 
  bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
  q = db.execQuery(bufSQL); // ������
  if(!q.eof())
  {
	  //��������� � ������ IDFiles ������� ������
	 	//    i = q.getIntField("IDword", -1);
 	}
  for (idwPtr=IDWord.begin(); idwPtr != IDWord.end(); ++idwPtr)
  {
    bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
	  q = db.execQuery(bufSQL); // ������
    //�������� ����� �������� ��� ���� ������ ������������ � �����������
    // ������� � ��� ���������� � ������ ������ ��������� �� ������
	  if(!q.eof())
	  {
    //��������� ������� ��������� ����� �� ��������� ������
	 	//    i = q.getIntField("IDword", -1);
	 	}
	  q.finalize();
    //������� �� ������ IDFile ������ �� ������������ �� �������� ������
  }
}

///////////////////////////////////////////////////////////////////////////////
// Search(string aText, list<string> & aParams)
// ������� ��������� ��������� ��� ������ �� ��������� (������� ������ � ����
// ������) ����������� ���������:
// ,; - ����� ��������� �� �������� � ������������� ����� ����� ���������� ���
// ^  - ����� ��������� �� �������� � ������������� ����� ����� ���� �
// ��������� �������� ����� ������� � �������� ��� ��������� ����������
// ����������� ���������������. (��������� ���������� � � ��� ����)
bool CDBProvider::Search(string aText, map<string,bool> & aParams, list<int> & Result)
{
}
