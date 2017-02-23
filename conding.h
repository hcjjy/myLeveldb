// Endian-neutral encoding:
// * Fixed-length numbers are encoded with least-significant byte first(��ѡС��ģʽ)
// * In addition we support variable length "varint" encoding(֧�ֿɱ䳤�ȡ�varint������)
// * Strings are encoded prefixed by their length in varint format(string����Ԥ�ȹ̶����ȵġ�varint����ʽ����)
// * ���������int��Ҫ32λ���룬����varint��һ���ֽڵ����λ��Ϊ��־λ�����ԣ�һ���ֽ�ֻ�ܴ洢7λ��
//��������ر�󣬿�����Ҫ5���ֽ�{(5 * 8 - 5(��־λ)) > 32}����Ϣ�е�����һ��Ƚ�С���ʿ��Խ�ʡ�ռ䡣
#ifndef STORAGE_LEVELDB_UTIL_CODING_H_
#define STORAGE_LEVELDB_UTIL_CODING_H_

#include<stdint.h>
//#include<string.h>
#include "slice.h"
#include"port.h"

namespace leveldb {
	extern void putFixed32(std::string* dst, uint32_t value);
	extern void putFixed64(std::string* dst, uint64_t value);
	extern void putVarint32(std::string* dst, uint32_t value);
	extern void putVarint64(std::string* dst, uint64_t value);
}

#endif // ! STORAGE_LEVELDB_UTIL_CODING_H_