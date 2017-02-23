#include"conding.h"
#include<string.h>

namespace leveldb
{
	void encodeFixed32(char*buf, uint32_t value)
	{
#if __BYTE_ORDER == __LITTLE_ENDIAN
		memcpy(buf, &value, sizeof(value));
#else
		buf[0] = value & 0xff;
		buf[1] = (value >> 8) & 0xff;
		buf[2] = (value >> 16) & 0xff;
		buf[3] = (value >> 24) & 0xff;
#endif
	}
	void encodeFixed64(char*buf, uint64_t value)
	{
#if __BYTE_ORDER == __LITTLE_ENDIAN
		memcpy(buf, &value, sizeof(value));
#else
		buf[0] = value & 0xff;
		buf[1] = (value >> 8) & 0xff;
		buf[2] = (value >> 16) & 0xff;
		buf[3] = (value >> 24) & 0xff;
		buf[4] = (value >> 32) & 0xff;
		buf[5] = (value >> 40) & 0xff;
		buf[6] = (value >> 48) & 0xff;
		buf[7] = (value >> 56) & 0xff;
#endif
	}
	void putFixed32(std::string* dst, uint32_t value)
	{
		char buf[sizeof(value)];
		encodeFixed32(buf, value);
		dst->append(buf, sizeof(buf));
	}
	void putFixed64(std::string* dst, uint64_t value)
	{
		char buf[sizeof(value)];
		encodeFixed64(buf, value);
		dst->append(buf, sizeof(buf));
	}
	char* encodeVarint32(char* dst, uint32_t v)
	{
		unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
		static const int B = 128;
		if (v < (1<<7))
		{
			*(ptr++) = v;
		}
		else if (v < (1<<14))
		{
			*(ptr++) = v | B;
			*(ptr++) = v>>7;
		}
		else if (v < (1<<21))
		{
			*(ptr++) = v | B;
			*(ptr++) = (v>>7) | B;
			*(ptr++) = v>>14;
		}
		else if (v < (1<<28))
		{
			*(ptr++) = v | B;
			*(ptr++) = (v>>7) | B;
			*(ptr++) = (v>>14) | B;
			*(ptr++) = (v>>21) | B;
		}
		else 
		{
			*(ptr++) = v | B;
			*(ptr++) = (v>>7) | B;
			*(ptr++) = (v>>14) | B;
			*(ptr++) = (v>>21) | B;
			*(ptr++) = (v>>28);
		}
		return reinterpret_cast<char*>(ptr);
	}

	char* encodeVarint64(char* dst, uint64_t v)
	{
		unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
		static const int B = 128;
		while (v >= B)
		{
			*(ptr++) = (v & (B - 1)) | B;
			v >>= 7;
		}
		*(ptr++) = static_cast<unsigned char>(v);
		return reinterpret_cast<char*>(ptr);
	}

	void putVarint32(std::string* dst, uint32_t value)
	{
		char buf[5];
		char* ptr = encodeVarint32(buf, value);
		dst->append(buf, ptr - buf);
	}

	void putVarint64(std::string* dst, uint64_t value)
	{
		char buf[10];
		char* ptr = encodeVarint32(buf, value);
		dst->append(buf, ptr - buf);
	}
}