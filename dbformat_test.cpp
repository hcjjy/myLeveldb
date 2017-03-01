#ifdef HHH

#include"dbformat.h"
#include"logging.h"
#include"testharness.h"

namespace leveldb
{
	static std::string iKey(const std::string& user_key,
							uint64_t seq, ValueType vt)
	{
		std::string encoded;
		appendInternalKey(&encoded, ParsedInternalKey(user_key, seq, vt));
		return encoded;
	}

	static std::string shorten(const std::string& s, const std::string& l)
	{
		std::string result = s;
		InternalKeyComparator(bytewiseComparator()).findShortestSeparator(&result, l);
		return result;
	}

	static std::string shortSuccessor(const std::string& s)
	{
		std::string result = s;
		InternalKeyComparator(bytewiseComparator()).findShortSuccessor(&result);
		return result;
	}

	static void testKey(const std::string& key, uint64_t seq, ValueType vt)
	{
		std::string encoded = iKey(key, seq, vt);
		Slice in(encoded);
		ParsedInternalKey decoded("",0,kTypeValue);
		ASSERT_TRUE(parseInternalKey(in, &decoded));
		ASSERT_EQ(key, decoded.user_key.toString());
		ASSERT_EQ(seq, decoded.sequence);
		ASSERT_EQ(vt, decoded.type);

		ASSERT_TRUE(!parseInternalKey(Slice("bar"), &decoded));
	}

	class FormatTest{};
	TEST(FormatTest, InternalKey_EncodeDecode)
	{
		const char* keys[] = { "","k","hello","longgggggggggggggg" };
		const uint64_t seq[] =
		{
			1,2,3,
			(1ull << 8) - 1, 1ull << 8, (1ull << 8) + 1,
			(1ull << 8) - 1, 1ull << 16, (1ull << 16) + 1,
			(1ull << 32) - 1, 1ull << 32, (1ull << 32) + 1
		};
		for (int k = 0; k < sizeof(keys) / sizeof(keys[0]); ++k)
		{
			for (int s = 0; s < sizeof(seq) / sizeof(seq) / sizeof(seq[0]); ++s)
			{
				testKey(keys[k], seq[s], kTypeValue);
				testKey("hello", 1, kTypeDeletion);
			}
		}
	}

	TEST(FormatTest, InternalKeyShortSeparator)
	{
		//When user keys are same
		ASSERT_EQ(iKey("foo", 100, kTypeValue),
			shorten(iKey("foo", 100, kTypeValue),iKey("foo", 99, kTypeValue)));
		ASSERT_EQ(iKey("foo", 100, kTypeValue),
			shorten(iKey("foo", 100, kTypeValue), iKey("foo", 101, kTypeValue)));
		ASSERT_EQ(iKey("foo", 100, kTypeValue),
			shorten(iKey("foo", 100, kTypeValue), iKey("foo", 100, kTypeValue)));
		ASSERT_EQ(iKey("foo", 100, kTypeValue),
			shorten(iKey("foo", 100, kTypeValue), iKey("foo", 100, kTypeDeletion)));

		//When user keys are misordered
		ASSERT_EQ(iKey("foo", 100, kTypeValue),
			shorten(iKey("foo", 100, kTypeValue), iKey("bar", 99, kTypeValue)));

		// When user keys are different, but correctly ordered
		ASSERT_EQ(iKey("g", kMaxSequenceNumber, kValueTypeForSeek),
			shorten(iKey("foo", 100, kTypeValue), iKey("hello", 200, kTypeValue)));

		// When start user key is prefix of limit user key
		ASSERT_EQ(iKey("foo", 100, kTypeValue),
			shorten(iKey("foo", 100, kTypeValue), iKey("foobar", 200, kTypeValue)));

		// When limit user key is prefix of start user key
		ASSERT_EQ(iKey("foobar", 100, kTypeValue),
			shorten(iKey("foobar", 100, kTypeValue), iKey("foo", 200, kTypeValue)));
	}

	TEST(FormatTest, InternalKeyShortestSuccessor)
	{
		ASSERT_EQ(iKey("g", kMaxSequenceNumber, kValueTypeForSeek),
			shortSuccessor(iKey("foo", 100, kTypeValue)));
		ASSERT_EQ(iKey("\xff\xff", 100, kTypeValue),
			shortSuccessor(iKey("\xff\xff", 100, kTypeValue)));
	}
}

int main(int argc, char** argv)
{
	return leveldb::test::runAllTests();
}

#endif