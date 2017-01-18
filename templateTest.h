#ifndef STORAGE_LEVELDB_TEMPLATETEST_H_
#define STORAGE_LEVELDB_TEMPLATETEST_H_
#include<vector>
#include<list>
namespace leveldb
{
	template<typename T1, class T2>
	T1 addFun(T1 a, T2 b);
	template<class T1>
	T1 subFun(T1 a, int b);
	template<typename T1, class T2>
	T1 addFun(T1 a, T2 b)
	{
		//依赖类型名，但是非嵌套依赖类型名，(没有定义在类内)无需typename
		std::vector<T1>::iterator vecT = b.begin();
		T2::const_iterator iter = b.begin();
		return a + *vecT;
	}
	template<class T1>
	T1 subFun(T1 a, int b)
	{
		return a - b;
	}
	
	template<typename Key, class T>
	class TemplateTest
	{
	public:
		Key addClass(Key a, T b);
		Key subClass(Key a, Key b);
		void pushList(Key a);
	private:
		  typename std::vector<Key>::iterator vecT;//嵌套依赖类型需要typename
		  std::list<Key> listT;
	};
	template<typename Key, class T>
	Key TemplateTest<Key, T>::addClass(Key a, T b)
	{
		return a + b;
	}
	template<typename Key, class T>
	Key TemplateTest<Key, T>::subClass(Key a, Key b)
	{
		return a - b;
	}
	template<typename Key, class T>
	void TemplateTest<Key, T>::pushList(Key a)
	{
		listT.push_back(a);
	}
	template<typename T>
	class Base
	{
	public:
		class Nested
		{
		public:
			Nested() {}
			Nested(int x) {}
			T add(T a, T b) { return a + b; }
		};
	};
	template<typename T>
	class Derived :public Base<T>::Nested//继承是一个例外不需要typename
	{
	public:
		Derived(int x) :Base<T>::Nested(x) {}//初始化列表是一个例外不需要typename
		typename Base<T>::Nested tmpNested; //需要typename
	};
	struct Job
	{
		char name[100];
		int salary;
	};
	////////////////////////////////////////////////////////////
	//对函数
	inline void swap(int& a, int& b)
	{
		int temp;
		temp = a;
		a = b;
		b = temp;
	}
	template <class T>
	void swap(T& a, T& b)
	{
		T temp;
		temp = a;
		a = b;
		b = temp;
	}
	template void swap<int>(int& a, int& b);//显示实例化,只需声明
	template<> inline void swap<Job>(Job& a, Job& b)//显示具体化（必须有定义）
	{
		int salary;
		salary = a.salary;
		a.salary = b.salary;
		b.salary = salary;
	}

	//对类模板
	template<typename T>
	class Arrary
	{
	private:
		T* ar;
		int l;
	};
	template class Arrary<int>;//explicit instantiation显示实例化，只能用class,不能用typename
	template<> class Arrary<Job>
	{
	public:
		void setJ(int j) { this->j = j; };
	private:
		Job* ar;
		int l;
		int j;
	};//expicit specialization 显示具体化，类定义提可以不同于类模板Array
}
#endif // !STORAGE_LEVELDB_TEMPLATETEST_H_
