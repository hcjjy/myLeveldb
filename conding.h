// Endian-neutral encoding:
// * Fixed-length numbers are encoded with least-significant byte first(优选小端模式)
// * In addition we support variable length "varint" encoding(支持可变长度“varint”编码)
// * Strings are encoded prefixed by their length in varint format(string采用预先固定长度的“varint”格式编码)
// * 正常情况下int需要32位编码，但是varint用一个字节的最高位作为标志位。所以，一个字节只能存储7位，
//如果整数特别大，可能需要5个字节{(5 * 8 - 5(标志位)) > 32}。消息中的数字一般比较小，故可以节省空间。
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