#include"memtable.h"

namespace leveldb
{
	static Slice getLengthPrefixedSlice(const char* data)
	{
		uint32_t len;
		const char* p = data;
		p = getVarint32Ptr(p, p + 5, &len);
		return Slice(p, len);
	}

	MemTable::MemTable(const InternalKeyComparator& cmp)
		:comparator_(cmp),
		refs_(0),
		arena_(),
		table_(comparator_, &arena_)
	{

	}

	MemTable::~MemTable()
	{
		assert(refs_ == 0);
	}

	size_t MemTable::approximateMemoryUsage() { return arena_.memoryUsage(); }

	int MemTable::KeyComparator::operator()(const char* aptr, const char* bptr) const
	{
		Slice a = getLengthPrefixedSlice(aptr);
		Slice b = getLengthPrefixedSlice(bptr);
		return comparator.compare(a,b);
	}

	static const char* encodeKey(std::string* scratch, const Slice& target)
	{
		scratch->clear();
		putVarint32(scratch, target.size());
		scratch->append(target.data(), target.size());
		return scratch->data();
	}

	class MemTableIterator: public Iterator
	{
	public:
		explicit MemTableIterator(MemTable::Table* table) :iter_(table) {}

		virtual bool valid() const { return iter_.valid(); }
		virtual void seek(const Slice& k) { iter_.seek(encodeKey(&tmp_, k)); }
		virtual void seekToFirst() { iter_.seekToFirst(); }
		virtual void seekToLast() { iter_.seekToLast(); }
		virtual void next() { iter_.next(); }
		virtual void prev() { iter_.prev(); }
		virtual Slice key() const { return getLengthPrefixedSlice(iter_.key()); }
		virtual Slice value() const
		{
			Slice key_slice = getLengthPrefixedSlice(iter_.key());
			return getLengthPrefixedSlice(key_slice.data() + key_slice.size());
		}
		virtual Status status() const { return Status::OK(); }
	private:
		MemTable::Table::Iterator iter_;
		std::string tmp_;

		MemTableIterator(const MemTableIterator& );
		void operator=(const MemTableIterator& );
	};
	Iterator* MemTable::newIterator()
	{
		return new MemTableIterator(&table_);
	}
	void MemTable::add(SequenceNumber s, ValueType type, const Slice& key, const Slice& value)
	{
		size_t key_size = key.size();
		size_t val_size = value.size();
		size_t internal_key_size = val_size + 8;
		const size_t encoded_len = varintLength(internal_key_size) + internal_key_size +
								   varintLength(val_size) + val_size;
		char* buf = arena_.allocate(encoded_len);
		char* p = encodeVarint32(buf, encoded_len);
		memcmp(p, key.data(), key_size);
		p += key_size;
		encodeFixed64(p, (s << 8) | type);
		p += 8;
		p = encodeVarint32(p, val_size);
		memcmp(p, value.data(), val_size);
		p += val_size;
		assert(p - buf == encoded_len);
		table_.insert(buf);
	}

	bool MemTable::get(const LookupKey& key, std::string* value, Status* s)
	{
		Slice memkey = key.memtable_key();
		Table::Iterator iter(&table_);
		iter.seek(memkey.data());
		if (iter.valid())
		{
			const char* entry = iter.key();
			uint32_t key_length;
			const char* key_ptr = getVarint32Ptr(entry, entry + 5, &key_length);
			if (comparator_.comparator.user_comparator()->compare(
				Slice(key_ptr, key_length), key.user_key()) == 0)
			{
				const uint64_t tag = decodeFixed64(key_ptr + key_length - 8);
				switch (static_cast<ValueType>(tag & 0xff))
				{
				case kTypeValue:
				{
					Slice v = getLengthPrefixedSlice(key_ptr + key_length);
					value->assign(v.data(), v.size());
					return true;
				}
				case kTypeDeletion:
					*s = Status::notFound(Slice());
					return true;
				}
			}
		}
		return false;
	}
}
