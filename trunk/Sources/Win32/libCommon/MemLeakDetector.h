//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Memory leak detection routines
//Author: Parshin Dmitry
//Description: ��������, ����������� ��� ������ ������ ������
//-------------------------------------------------------------------------------------//
#ifndef MEMLEAKDETECTOR_H_
#define MEMLEAKDETECTOR_H_

//������ ����� ����������� ������ � ���������� ������ ���������
#ifndef NDEBUG

#include <new>
#include <iostream>



//����������� �������� new � ���������� ������������ ����, ��������� ��� �����������
//strFile - ��� �����
//iLine - ����� ������
//strFuncName - ��� ������� 
void* operator new( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc );

//����������� ��� ������ ��������� delete
void operator delete( void* address )throw();
void operator delete( void *address , size_t bytes );

//��������������� �������, ��� �������� �������������� ����������( � ������� ��������� delete )
void erase( void** pArray, int iSize, void* pSrc );

//���� �������� ������� ��� �� ��������� ������� ������ �� ������ ������
void DumpMemLeaks();

//��������� ������ new � ��������������� �����������, ����� ���������������� ��� �� ���������
#define new new(__FILE__, __LINE__, __FUNCTION__ )

      
#endif
#endif /*MEMLEAKDETECTOR_H_*/
