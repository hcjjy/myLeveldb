#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_
#include<vector>

namespace leveldb {
	class Arena
	{
	public:
		Arena();
		~Arena();
		char* allocate(size_t bytes);
		char* allocateAligned(size_t bytes);
		size_t memoryUsage() const { return blocks_memory_ + blocks_.capacity() * sizeof(char*); }
	private:
		char* allocateFallback(size_t bytes);
		char* allocateNewBlock(size_t block_bytes);
		char* alloc_ptr_;//当前指针
		size_t alloc_bytes_remaining_;//当前block剩余的内存

		std::vector<char*> blocks_;//保存所以内存块的指针
		size_t blocks_memory_;//到目前为止已经分配的内存

							  //No copying allowed
		Arena(const Arena&);
		void operator=(const Arena&);
	};
}
#endif // ! STORAGE_LEVELDB_UTIL_ARENA_H_

