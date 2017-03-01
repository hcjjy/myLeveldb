#ifndef STORAGE_LEVELDB_INCLUDE_ITERATOR_H_
#define STORAGE_LEVELDB_INCLUDE_ITERATOR_H_

#include"slice.h"
#include"status.h"

namespace leveldb
{
	class Iterator
	{
	public:
		Iterator();
		virtual ~Iterator();
		virtual bool valid() const = 0;
		virtual void seek(const Slice& target) = 0;
		virtual void seekToFirst() = 0;
		virtual void seekToLast() = 0;
		virtual void next() = 0;
		virtual void prev() = 0;
		virtual Slice key() const = 0;
		virtual Status status() const = 0;
		typedef void(*CleanupFunction)(void* arg1, void* arg2);
		void registerCleanup(CleanupFunction function, void* arg1, void* arg2);
	private:
		struct Cleanup
		{
			CleanupFunction function;
			void* arg1;
			void* arg2;
			Cleanup* next;
		};
		Cleanup cleanup_;
		
		Iterator(const Iterator&);
		void operator=(const Iterator&);
	};
	extern Iterator* newEmptyIterator();
	extern Iterator* newErrorIterator(const Status& status);
}
#endif // STORAGE_LEVELDB_INCLUDE_ITERATOR_H_
