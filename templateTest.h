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
		//���������������Ƿ�Ƕ��������������(û�ж���������)����typename
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
		  typename std::vector<Key>::iterator vecT;//Ƕ������������Ҫtypename
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
	class Derived :public Base<T>::Nested//�̳���һ�����ⲻ��Ҫtypename
	{
	public:
		Derived(int x) :Base<T>::Nested(x) {}//��ʼ���б���һ�����ⲻ��Ҫtypename
		typename Base<T>::Nested tmpNested; //��Ҫtypename
	};
	struct Job
	{
		char name[100];
		int salary;
	};
	////////////////////////////////////////////////////////////
	//�Ժ���
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
	template void swap<int>(int& a, int& b);//��ʾʵ����,ֻ������
	template<> inline void swap<Job>(Job& a, Job& b)//��ʾ���廯�������ж��壩
	{
		int salary;
		salary = a.salary;
		a.salary = b.salary;
		b.salary = salary;
	}

	//����ģ��
	template<typename T>
	class Arrary
	{
	private:
		T* ar;
		int l;
	};
	template class Arrary<int>;//explicit instantiation��ʾʵ������ֻ����class,������typename
	template<> class Arrary<Job>
	{
	public:
		void setJ(int j) { this->j = j; };
	private:
		Job* ar;
		int l;
		int j;
	};//expicit specialization ��ʾ���廯���ඨ������Բ�ͬ����ģ��Array
}
#endif // !STORAGE_LEVELDB_TEMPLATETEST_H_
