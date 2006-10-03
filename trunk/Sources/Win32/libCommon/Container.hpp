//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Abstract Container
//Author: Parshin Dmitry
//Description: ���������� ����� ����������
//-------------------------------------------------------------------------------------//
#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>

/*����������� ������� ������ ��������: 
	class T - ��� �������� ��������
	class LoadStrategy - �����, ����������� ��������� �������� � ������������ ����������
	
	LoadStrategey ������ ����� ����������� ����:
	LoadStrategy( std::vector< T >& vecStorage );
	
	� vecStorage ������ ����������� ��������
*/
template< class T, class LoadStrategy >
class Container
{
public:

	//��� ��������� ��� ����������� � ���������� ����������
	typedef typename std::vector< T >::iterator iterator;

	Container()
	{
		m_Loader = new LoadStrategy( m_vecStorage );
	}
	
	virtual ~Container()
	{
		 delete m_Loader; 
	}

	//���������� �������� ������� �������� ����������	
	iterator begin()
	{
		return m_vecStorage.begin();
	}
	
	//���������� �������� ���������� �� ��������� �������� ����������	
	iterator end()
	{
		return m_vecStorage.end();
	}
	
	//���������� ���-�� ��������� ����������
	int count()
	{	
		return m_vecStorage.size();
	}
	
private:
	
	LoadStrategy* m_Loader;
		
	std::vector< T > m_vecStorage;
};

#endif /*CONTAINER_H_*/
