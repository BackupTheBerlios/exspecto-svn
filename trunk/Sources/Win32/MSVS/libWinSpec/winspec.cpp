#include "Os_Spec.h"
#include <errno.h>


std::string get_basepath()
{
	char str[255];
	//Получаем имя файла текущего процесса и составляем из него имя файла журнала
	GetModuleFileName( NULL, str, sizeof(str) );

	return str;
}


