#ifndef LITTLEBIGENDIAN_H_
#define	LITTLEBIGENDIAN_H_
#include<memory>
#include<iostream>
namespace leveldb
{
	inline void littlebigendianTest()
	{
		char chlb[5] = { '1','2','3','4' };
		std::cout << &chlb << " " << (int*)(&chlb[1]) << " " 
				  << (int*)&chlb[2] << " " << (int*)&chlb[3] << std::endl;
		char lbuff[5];
		int value = 0x01020304;
#if __BYTE_ORDER == __LITTLE_ENDIAN
		//memcpy(lbuff, &value, 4);
		lbuff[0] = value & 0xff;
		lbuff[1] = value>>8 & 0xff;
		lbuff[2] = value>>16 & 0xff;
		lbuff[3] = value>>24 & 0xff;

#else
		std::cout << "big endian." << std::endl;
#endif
		std::cout << (int)((char*)&value)[0] << std::endl;
		std::cout << (int)((char*)&value)[1] << std::endl;
		std::cout << (int)((char*)&value)[2] << std::endl;
		std::cout << (int)((char*)&value)[3] << std::endl;
		std::cout << (int)lbuff[0] << std::endl;
		std::cout << (int)lbuff[1] << std::endl;
		std::cout << (int)lbuff[2] << std::endl;
		std::cout << (int)lbuff[3] << std::endl;
	}
}
#endif // !LITTLEBIGENDIAN_H_
