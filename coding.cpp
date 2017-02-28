#include"coding.h"
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
			*(ptr++) = (v>>21);
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
		char* ptr = encodeVarint64(buf, value);
		dst->append(buf, ptr - buf);
	}

	void putLengthPrefixedSlice(std::string* dst, const Slice& value) 
	{
		putVarint32(dst, value.size());
		dst->append(value.data(), value.size());
	}

	int varintLength(uint64_t v)
	{
		int len = 1;
		while (v >= 128)
		{
			v >>= 7;
			++len;
		}
		return len;
	}

	const char* getVarint32PtrFallback(const char* p, const char* limit, uint32_t* value)
	{
		uint32_t result = 0;
		for (int shift = 0;shift <= 28 && p < limit; shift += 7)
		{
			uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
			++p;
			if (byte & 128) 
			{
				result |= ((byte & 127) << shift);
			}
			else
			{
				result |= (byte << shift);
				*value = result;
				return reinterpret_cast<const char*>(p);
			}
		}
		return NULL;
	}

	bool getVarint32(Slice* input, uint32_t* value)
	{
		const char* p = input->data();
		const char* limit = p + input->size();
		const char* q = getVarint32Ptr(p, limit, value);
		if (q == NULL)
			return false;
		*input = Slice(q, limit - q);
		return true;
	}

	const char* getVarint64Ptr(const char* p, const char* limit, uint64_t* value)
	{
		uint64_t result = 0;
		for (uint64_t shift = 0; shift <= 63 && p < limit; shift += 7)
		{
			uint64_t byte = *(reinterpret_cast<const unsigned char*>(p));
			++p;
			if (byte & 128)
			{
				result |= ((byte & 127) << shift);
			}
			else
			{
				result |= (byte << shift);
				*value = result;
				return reinterpret_cast<const char*>(p);
			}
		}
		return NULL;
	}

	bool getVarint64(Slice* input, uint64_t* value)
	{
		const char *p = input->data();
		const char *limit = p + input->size();
		const char *q = getVarint64Ptr(p, limit, value);
		if (q == NULL)
			return false;
		*input = Slice(q, limit - q);
		return true;
	}

	const char* getLengthPrefixedSlice(const char* p, const char* limit, Slice* result)
	{
		uint32_t len;
		p = getVarint32Ptr(p, limit, &len);
		if (p == NULL)
			return NULL;
		if (p + len > limit)
			return NULL;
		*result = Slice(p, len);
		return p + len;
	}

	bool getLengthPrefixedSlice(Slice* input, Slice* result)
	{
		uint32_t len;
		if (getVarint32(input, &len) && input->size() >= len)
		{
			*result = Slice(input->data(), len);
			input->remove_prefix(len);
			return true;
		}
		return false;
	}
}