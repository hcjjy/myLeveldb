#include"coding.h"
#include"testharness.h"
#include<vector>
#include<iostream>

namespace leveldb 
{
	class Coding {};
	TEST(Coding, Fixed32) {
		std::string s;
		for (uint32_t v = 0; v < 100000; ++v)
		{
			putFixed32(&s, v);
		}
		const char* p = s.data();
		for (uint32_t v = 0; v < 100000; ++v)
		{
			uint32_t actual = decodeFixed32(p);
			ASSERT_EQ(v, actual);
			p += sizeof(uint32_t);
		}
	}
	TEST(Coding, Fixed64) {
		std::string s;
		for (int power = 0; power <= 63; ++power)
		{
			uint64_t v = static_cast<uint64_t>(1) << power;
			putFixed64(&s, v - 1);
			putFixed64(&s, v + 0);
			putFixed64(&s, v + 1);
		}
		const char* p = s.data();
		for (int power = 0; power <= 63; ++power)
		{
			uint64_t v = static_cast<uint64_t>(1) << power;
			uint64_t actual = decodeFixed64(p);
			ASSERT_EQ(v - 1, actual);
			p += sizeof(uint64_t);

			actual = decodeFixed64(p);
			ASSERT_EQ(v + 0, actual);
			p += sizeof(uint64_t);

			actual = decodeFixed64(p);
			ASSERT_EQ(v + 1, actual);
			p += sizeof(uint64_t);
		}
	}

	TEST(Coding, Varint32) {
		std::string s;
		for (uint32_t i = 0; i < (32 * 32); ++i)
		{
			uint32_t v = (i / 32) << (i % 32);
			putVarint32(&s, v);
		}
		const char* p = s.data();
		const char* limit = p + s.size();
		for (uint32_t i = 0; i < (32 * 32); ++i)
		{
			uint32_t expected = (i / 32) << (i % 32);
			uint32_t actual;
			const char* start = p;
			p = getVarint32Ptr(p, limit, &actual);
			if (p == NULL)
			{
				std::cout << i << " " << expected << " " << actual << " "<<std::endl;
			}
			ASSERT_TRUE(p != NULL);
			ASSERT_EQ(expected, actual);
			ASSERT_EQ(varintLength(actual), p - start);
		}
		ASSERT_EQ(p, limit);
	}

	TEST(Coding, Varint64) {
		std::vector<uint64_t> values;
		values.push_back(0);
		values.push_back(100);
		values.push_back(~static_cast<uint64_t>(0));
		values.push_back(~static_cast<uint64_t>(0) - 1);
		for (uint32_t k = 0; k < 64; ++k)
		{
			const uint64_t power = 1ull << k;
			values.push_back(power);
			values.push_back(power - 1);
			values.push_back(power + 1);
		}

		std::string s;
		for (int i = 0; i < values.size(); ++i)
		{
			putVarint64(&s, values[i]);
		}

		const char* p = s.data();
		const char* limit = p + s.size();
		for (int i = 0; i < values.size(); ++i)
		{
			ASSERT_TRUE(p < limit);
			uint64_t actual;
			const char* start = p;
			p = getVarint64Ptr(p, limit, &actual);
			ASSERT_TRUE(p != NULL);
			ASSERT_EQ(values[i], actual);
			ASSERT_EQ(varintLength(actual), p - start);
		}
		ASSERT_EQ(p, limit);
	}

	TEST(Coding, Varint32Overflow) 
	{
		uint32_t result;
		std::string input("\x81\x82\x83\x84\x85\x11");
		ASSERT_TRUE(getVarint32Ptr(input.data(), input.data() + input.size(), &result) == NULL);
	}

	TEST(Coding, Varint32Truncation)
	{
		uint32_t large_value = (1u << 31) + 100;
		std::string s;
		putVarint32(&s, large_value);
		uint32_t result;
		for (int len = 0; len < s.size() - 1; ++len)
		{
			ASSERT_TRUE(getVarint32Ptr(s.data(), s.data() + len, &result) == NULL);
		}
		ASSERT_TRUE(getVarint32Ptr(s.data(), s.data() + s.size(), &result) != NULL);
		ASSERT_EQ(large_value, result);
	}

	TEST(Coding, Varint64Overflow) 
	{
		uint64_t result;
		std::string input("\x81\x82\x83\x84\x85\x81\x82\x83\x84\x85\x11");
		ASSERT_TRUE(getVarint64Ptr(input.data(), input.data() + input.size(), &result) == NULL);
	}

	TEST(Coding, Varint64Truncation)
	{
		uint64_t large_value = (1ull << 63) + 100ull;
		std::string s;
		putVarint64(&s, large_value);
		uint64_t result;
		for (int len = 0; len < s.size() - 1; ++len)
		{
			ASSERT_TRUE(getVarint64Ptr(s.data(), s.data() + len, &result) == NULL);
		}
		ASSERT_TRUE(getVarint64Ptr(s.data(), s.data() + s.size(), &result) != NULL);
		ASSERT_EQ(large_value, result);
	}

	TEST(Coding, Strings) {
		std::string s;
		putLengthPrefixedSlice(&s, Slice(""));
		putLengthPrefixedSlice(&s, Slice("foo"));
		putLengthPrefixedSlice(&s, Slice("bar"));
		putLengthPrefixedSlice(&s, Slice(std::string(200, 'x')));

		Slice input(s);
		Slice v;
		ASSERT_TRUE(getLengthPrefixedSlice(&input, &v));
		ASSERT_EQ("", v.toString());
		ASSERT_TRUE(getLengthPrefixedSlice(&input, &v));
		ASSERT_EQ("foo", v.toString());
		ASSERT_TRUE(getLengthPrefixedSlice(&input, &v));
		ASSERT_EQ("bar", v.toString());
		ASSERT_TRUE(getLengthPrefixedSlice(&input, &v));
		ASSERT_EQ(std::string(200, 'x'), v.toString());
		ASSERT_EQ("", input.toString());
	}
}

int main(int argc, char** argv) {
	return leveldb::test::runAllTests();
}