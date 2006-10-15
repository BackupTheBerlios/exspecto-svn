//------------------------------------------------------------------------------------//
//Ётот файл €вл€етс€ частью проекта Exspecto 2006г.
//Module: Abstract Container
//Author: Parshin Dmitry
//Description: ќбобщенный класс контейнера
//-------------------------------------------------------------------------------------//
#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>
#include <memory>

/*ѕараметрами шаблона класса €вл€ютс€: 
	class T - тип хранимых значений
	class LoadStrategy - класс, описывающий стратегию загрузки и освобождени€ контейнера
	
	LoadStrategey должен иметь конструктор типа:
	LoadStrategy( std::vector< T >& vecStorage );
	
	в vecStorage должна происходить загрузка
*/
template< class T, class LoadStrategy >
class Container
{
public:

	//“ип итератора дл€ манипул€ций с содержимым контейнера
	typedef typename std::vector< T >::iterator iterator;

	Container()
	{
		m_pLoader = std::auto_ptr< LoadStrategy >( new LoadStrategy( m_vecStorage ) );
	}
	
	virtual ~Container(){};

	//¬озвращает итератор первого элемента контейнера	
	iterator begin()
	{
		return m_vecStorage.begin();
	}
	
	//¬озвращает итератор следующего за последним элемента контейнера	
	iterator end()
	{
		return m_vecStorage.end();
	}
	
	//¬озвращает кол-во элементов контейнера
	int count()
	{	
		return m_vecStorage.size();
	}
	
private:
	
	std::auto_ptr< LoadStrategy > m_pLoader;
		
	std::vector< T > m_vecStorage;
};

#endif /*CONTAINER_H_*/
