#ifndef STORAGE_LEVELDB_DB_MEMTABLE_H_
#define STORAGE_LEVELDB_DB_MEMTABLE_H_

#include<assert.h>
#include"iterator.h"
#include"dbformat.h"
#include"skiplist.h"

namespace leveldb
{
	class InternalKeyComparator;
	class MemTableIterator;

	class MemTable
	{
	public:
		explicit MemTable(const InternalKeyComparator& comparator);
		void ref() { ++refs_; }
		void unref()
		{
			--refs_;
			assert(refs_ >= 0);
			if (refs_ <= 0)
				delete this;
		}
		size_t approximateMemoryUsage();
		Iterator* newIterator();
		void add(SequenceNumber seq, ValueType type, const Slice& key, const Slice& value);
		bool get(const LookupKey& key, std::string* value, Status* s);
	private:
		~MemTable();
		struct KeyComparator
		{
			const InternalKeyComparator comparator;
			explicit KeyComparator(const InternalKeyComparator& c) :comparator(c) {}
			int operator()(const char* a, const char* b) const;
		};
		friend class MemTableIterator;
		friend class MemTableBackwardIterator;

		typedef SkipList<const char*, KeyComparator> Table;

		KeyComparator comparator_;
		int refs_;
		Arena arena_;
		Table table_;

		MemTable(const MemTable&);
		void operator=(const MemTable&);
	};
}

#endif // !STORAGE_LEVELDB_DB_MEMTABLE_H_
