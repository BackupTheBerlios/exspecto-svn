#include "Os_Spec.h"
#include <errno.h>
#include "windows.h"


std::string get_basepath()
{
	char str[255];
	//�������� ��� ����� �������� �������� � ���������� �� ���� ��� ����� �������
	GetModuleFileName( NULL, str, sizeof(str) );

	return str;
}


