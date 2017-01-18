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
		char* alloc_ptr_;//��ǰָ��
		size_t alloc_bytes_remaining_;//��ǰblockʣ����ڴ�

		std::vector<char*> blocks_;//���������ڴ���ָ��
		size_t blocks_memory_;//��ĿǰΪֹ�Ѿ�������ڴ�

							  //No copying allowed
		Arena(const Arena&);
		void operator=(const Arena&);
	};
}
#endif // ! STORAGE_LEVELDB_UTIL_ARENA_H_

