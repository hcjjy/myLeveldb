#ifndef STORAGE_LEVELDB_UTIL_RANDOM_H_
#define STORAGE_LEVELDB_UTIL_RANDOM_H_

#include<stdint.h>
namespace leveldb
{
	class Random
	{
	public:
		explicit Random(uint32_t s):seed_(s & 0x7fffffffu){}
		uint32_t next();
		uint32_t uniform(int n) { return next() % n; }
		bool oneIn(int n) { return (next() % n) == 0; }
		uint32_t Skewed(int max_log) { return uniform(1 << uniform(max_log + 1)); }
	private:
		uint32_t seed_;
	};
	
	//[1, M - 1]
	inline uint32_t Random::next()
	{
		static const uint32_t M = 2147483647L;
		static const uint32_t A = 16807;
		uint64_t product = seed_ * A;
		seed_ = static_cast<uint32_t>(product >> 31) + (product & M);
		if (seed_ > M) //如何证明seed_ 不会等于M
			seed_ -= M;
		return seed_;
	}
}

#endif
