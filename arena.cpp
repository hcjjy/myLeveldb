#include"arena.h"
#include<assert.h>

namespace leveldb {
	static const int kMaxBlockSize = 4096;

	Arena::Arena()
		:blocks_memory_(0),
		alloc_ptr_(NULL),
		alloc_bytes_remaining_(0)
	{

	}
	Arena::~Arena()
	{
		for (int i = 0; i < blocks_.size(); ++i)
			delete[] blocks_[i];
	}

	char* Arena::allocate(size_t bytes)
	{
		assert(bytes > 0);
		if (bytes <= alloc_bytes_remaining_)
		{
			char * ret = alloc_ptr_;
			alloc_bytes_remaining_ -= bytes;
			alloc_ptr_ += bytes;
			return ret;
		}
		return allocateFallback(bytes);
	}

	char* Arena::allocateAligned(size_t bytes)
	{
		const int align = sizeof(void*); //对齐标准为指针的大小
		assert((align & (align - 1)) == 0); //保证是2的倍数
		size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);
		size_t slop = (current_mod == 0)? 0 : align - current_mod;
		size_t needed = bytes + slop;
		char* result = NULL;
		if (needed <= alloc_bytes_remaining_)
		{
			result = alloc_ptr_ + slop;
			alloc_ptr_ += needed;
			alloc_bytes_remaining_ -= needed;
		}
		else
		{
			result = allocateFallback(bytes);
		}
		//can I ignore?
		assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
		return result;
	}

	char* Arena::allocateFallback(size_t bytes)
	{
		if (bytes > kMaxBlockSize / 4)
		{
			return allocateNewBlock(bytes);
		}
		char* ret = allocateNewBlock(kMaxBlockSize);
		alloc_bytes_remaining_ = kMaxBlockSize;
		alloc_ptr_ = ret;
		alloc_ptr_ += bytes;
		alloc_bytes_remaining_ -= bytes;
		return ret;
	}

	char* Arena::allocateNewBlock(size_t block_bytes)
	{
		char * allocateArr = new char[block_bytes];
		blocks_memory_ += block_bytes;
		blocks_.push_back(allocateArr);
		return allocateArr;
	}
}