#ifndef STRINGSTREAMTEST_H_
#define STRINGSTREAMTEST_H_

#include<iostream>
#include<sstream>

namespace leveldb {
	inline void stringstreamTest()
	{
		std::istringstream istr;
		istr.str("1   56.7");
		std::cout << istr.str() << std::endl;
		int a;
		float b;
		istr >> a;
		std::cout << a << std::endl; //1
		istr >> b;
		std::cout << b << std::endl; //56.7
		std::ostringstream ostr("ccc");
		ostr.put('d');
		ostr.put('e');
		ostr << "fg";
		std::string gstr = ostr.str();
		std::cout << gstr << std::endl;
	}
}

#endif // !STRINGSTREAMTEST_H_
