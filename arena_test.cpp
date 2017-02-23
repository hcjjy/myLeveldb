#include"arena.h"
#include"random.h"
#include"testharness.h"

namespace leveldb 
{
	class ArenaTest { };
	TEST(ArenaTest, Empty)
	{
		Arena arena;
	}
	TEST(ArenaTest, Simple)
	{
		std::vector<std::pair<size_t, char*> > allocated;
		Arena arena;
		const int N = 100000;
		size_t bytes = 0;
		Random rnd(301);
		for (int i = 0; i < N; ++i)
		{
			size_t s;
			if (i % (N / 10) == 0)
			{
				s = i;
			}
			else
			{
				s = rnd.oneIn(4000) ? rnd.uniform(6000) :
					(rnd.oneIn(10) ? rnd.uniform(100) : rnd.uniform(20));
			}
			if (s == 0)
			{
				//我们的内存不允许分配大小为0的空间
				s = 1;
			}
			char* r;
			if (rnd.oneIn(10))
			{
				r = arena.allocateAligned(s);
			}
			else
			{
				r = arena.allocate(s);
			}
			for (int b = 0; b < s; ++b)
			{
				r[b] = i % 256;
			}
			bytes += s;
			allocated.push_back(std::make_pair(s, r));
			ASSERT_GE(arena.memoryUsage(), bytes);
			if (i > N / 10)
			{
				ASSERT_LE(arena.memoryUsage(), bytes * 1.10);
			}
		}
		for (int i = 0; i < allocated.size(); ++i)
		{
			size_t num_bytes = allocated[i].first;
			const char* p = allocated[i].second;
			for (int b = 0; b < num_bytes; ++b)
			{
				//检验分配的内存数据是否出错
				ASSERT_EQ(int(p[b]) & 0xff, i % 256);
			}
		}
	}
}
int main(int argc, char** argv)
{
	const char* matcher = getenv("LEVELDB_TESTS");
	return leveldb::test::runAllTests();
}