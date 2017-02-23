#include<iostream>
#include<stdlib.h>
#include"slice.h"
#include"arena.h"
#include"status.h"
#include"random.h"
#include"templateTest.h"
#include"skiplist.h"
#include"stringstreamTest.h"
#include"littleBigEndian.h"
using namespace std;
int main()
{
	cout << "Slice Test==========================" << endl;
	leveldb::Slice slice("hahha");
	leveldb::Slice slice2 = slice;
	leveldb::Slice slice3;
	slice3 = "dsdsd";
	if (slice != slice3)
		cout << "!=" << endl;
	cout << "Arena Test==========================" << endl;
	leveldb::Arena arena;
	char * allocate = arena.allocate(1010);
	char * ch= new (allocate)char[1010];
	ch[0] = '1';
	ch[1] = '2';
	ch[2] = '\0';
	allocate = arena.allocate(4000);
	char * ch2 = new (allocate)char[4000];
	ch2[0] = '3';
	ch2[1] = '\0';
	allocate = arena.allocateAligned(3086);
	char *ch3 = new (allocate)char[3086];
	ch3[0] = '4';
	ch3[1] = '\0';
	cout << "Status Test==========================" << endl;
	leveldb::Status status = leveldb::Status::notFound("why not found?", "i don't know.");
	cout << status.toString() << endl;
	cout << status.isCorruption() << endl;
	leveldb::Status status1;
	status1 = status;
	cout << status1.isNotFound() << endl;
	cout << "Random Test==========================" << endl;
	leveldb::Random random(222);
	int randomCnt = 20;
	while (randomCnt--)
		cout << random.uniform(100) <<" "<<random.oneIn(60)<<" "<<random.Skewed(5) << endl;
	cout << leveldb::addFun(1,vector< int >{1, 2}) << endl;
	cout << leveldb::subFun(1.2, 2) << endl;
	cout << "Template Test==========================" << endl;
	leveldb::TemplateTest<int, int> typenameTest;
	cout << typenameTest.addClass(1, 2) << endl;
	cout<<typenameTest.subClass(1.3, 2)<<endl;
	typenameTest.pushList(1);
	leveldb::Derived<int> derived(1);
	cout<<derived.add(1, 2)<<endl;
	int swapi = 1, swapj = 2;
	leveldb::swap(swapi, swapj);
	leveldb::Job job1 = { "hcjjy",80 }, job2 = {"deer", 90};
	leveldb::swap(job1, job2);
	leveldb::Arrary<leveldb::Job>arrary;
	arrary.setJ(1);
	cout << "stringstream Test==========================" << endl;
	leveldb::stringstreamTest();
	cout << "littleBigEndian Test==========================" << endl;
	leveldb::littlebigendianTest();
	const char *ch1 = "";
	const char *ch22 = NULL;
	if (ch1 != NULL)
		cout << ch1 << endl;
	if (ch1 == '\0')
		cout << "ch2" << endl;
	return 0;
}