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

namespace leveldb 
{
	//Standard Put... routines append to a string.
	extern void putFixed32(std::string* dst, uint32_t value);
	extern void putFixed64(std::string* dst, uint64_t value);
	extern void putVarint32(std::string* dst, uint32_t value);
	extern void putVarint64(std::string* dst, uint64_t value);
	extern void putLengthPrefixedSlice(std::string* dst, const Slice& value);

	// Standard Get... routines parse a value from the beginning of a Slice
	// and advance the slice past the parsed value.
	extern bool getVarint32(Slice* input, uint32_t* value);
	extern bool getVarint64(Slice* input, uint64_t* value);
	extern bool getLengthPrefixedSlice(Slice* input, Slice* result);

	extern const char* getVarint32Ptr(const char* p, const char* limit, uint32_t* v);
	extern const char* getVarint64Ptr(const char* p, const char* limit, uint64_t* v);

	extern int varintLength(uint64_t v);

	extern void encodeFixed32(char* dst, uint32_t value);
	extern void encodeFixed64(char* dst, uint64_t value);

	extern char* encodeVarint32(char* dst, uint32_t value);
	extern char* encodeVarint64(char* dst, uint64_t value);

	inline uint32_t decodeFixed32(const char* ptr)
	{
		if (port::kLittleEndian)
		{
			uint32_t result;
			memcpy(&result, ptr, sizeof(result));
			return result;
		}
		return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0])))
			| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8)
			| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16)
			| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
	}

	inline uint64_t decodeFixed64(const char* ptr)
	{
		if (port::kLittleEndian)
		{
			uint64_t result;
			memcpy(&result, ptr, sizeof(result));
			return result;
		}
		uint64_t lo = decodeFixed32(ptr);
		uint64_t hi = decodeFixed64(ptr + 4);
		return (hi << 32) | lo;
	}

	extern const char* getVarint32PtrFallback(const char* p,
											  const char* limit, 
											  uint32_t* value);
	
	inline const char* getVarint32Ptr(const char* p,
		                              const char* limit,
		                              uint32_t* value)
	{
		if (p < limit) {
			uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
			if ((result & 128) == 0)
			{
				*value = result;
				return p + 1;
			}
		}
		return getVarint32PtrFallback(p, limit, value);
	}
}

#endif // ! STORAGE_LEVELDB_UTIL_CODING_H_