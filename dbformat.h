#ifndef STORAGE_LEVELDB_DB_FORMAT_H_
#define STORAGE_LEVELDB_DB_FORMAT_H_

#include<stdio.h>
#include<stdint.h>
#include"slice.h"
#include"coding.h"
#include"logging.h"
#include"comparator.h"

namespace leveldb 
{
	namespace config 
	{
		static const int kNumLevels = 7;
		static const int kL_CompactionTrigger = 4;
		static const int kL0_SlowdownWritesTrigger = 8;
		static const int kL0_StopWritesTrigger = 12;
		static const int kMaxMemCompactLevel = 2;
	}
	class InternalKey;
	enum ValueType
	{
		kTypeDeletion = 0x0,
		kTypeValue = 0x1
	};
	static const ValueType kValueTypeForSeek = kTypeValue;
	typedef uint64_t SequenceNumber;
	static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);
	
	struct ParsedInternalKey
	{
		Slice user_key;
		SequenceNumber sequence;
		ValueType type;
		ParsedInternalKey() {}
		ParsedInternalKey(const Slice& u, const SequenceNumber& seq, ValueType t)
			:user_key(u), sequence(seq), type(t) {}
		std::string debugString() const;
	};

	inline size_t internalKeyEncodingLength(const ParsedInternalKey& key)
	{
		return key.user_key.size() + 8;
	}

	extern void appendInternalKey(std::string* result, const ParsedInternalKey& key);

	extern bool parseInternalKey(const Slice& internal_key, ParsedInternalKey* result);

	inline Slice extractUserKey(const Slice& internal_key)
	{
		assert(internal_key.size() >= 8);
		return Slice(internal_key.data(), internal_key.size() - 8);
	}

	inline ValueType extractValueType(const Slice& internal_key)
	{
		assert(internal_key.size() >= 8);
		const size_t n = internal_key.size();
		uint64_t num= decodeFixed64(internal_key.data() + n - 8);
		unsigned char c = num & 0xff;
		return static_cast<ValueType>(c);
	}

	class InternalKeyComparator : public Comparator
	{
	private:
		const Comparator* user_comparator_;
	public:
		explicit InternalKeyComparator(const Comparator* c) : user_comparator_(c) {}
		virtual const char* name() const;
		virtual int compare(const Slice& a, const Slice& b)const;
		virtual void findShortestSeparator(std::string* start, const Slice& limit)const;
		virtual void findShortSuccessor(std::string* key) const;
		const Comparator* user_comparator() const { return user_comparator_; }
		int compare(const InternalKey& a, const InternalKey& b)const;
	};

	class InternalKey
	{
	private:
		std::string rep_;
	public:
		InternalKey() {}
		InternalKey(const Slice& user_key, SequenceNumber s, ValueType t)
		{
			appendInternalKey(&rep_,ParsedInternalKey(user_key, s, t));
		}
		void decodeFrom(const Slice& s) { rep_.assign(s.data(), s.size()); }
		Slice encode() const
		{
			assert(!rep_.empty());
			return rep_;
		}
		Slice user_key() const { return extractUserKey(rep_); }
		void setFrom(const ParsedInternalKey& p)
		{
			rep_.clear();
			appendInternalKey(&rep_, p);
		}
		void clear() { rep_.clear(); }
		std::string debugString() const;
	};

	inline int InternalKeyComparator::compare(const InternalKey& a,
											  const InternalKey& b)const
	{
		return compare(a.encode(), b.encode());
	}

	inline bool parseInternalKey(const Slice& internal_key, ParsedInternalKey* result)
	{
		const size_t n = internal_key.size();
		if (n < 8)
			return false;
		uint64_t num = decodeFixed64(internal_key.data() + n - 8);
		unsigned char c = num & 0xff;
		result->user_key = Slice(internal_key.data(), n - 8);
		result->sequence = num >> 8;
		result->type = static_cast<ValueType>(c);
		return (c <= static_cast<unsigned char>(kTypeValue));
	}

	class LookupKey
	{
	public:
		LookupKey(const Slice& user_key, SequenceNumber sequence);
		~LookupKey();
		Slice memtable_key() const { return Slice(start_, end_ - start_); }
		Slice internal_key() const { return Slice(kstart_, end_ - kstart_); }
		Slice user_key() const { return Slice(kstart_, end_ - kstart_ - 8); }
	private:
		const char* start_;
		const char* kstart_;
		const char* end_;
		char space_[200];

		//No copying allowed
		LookupKey(const LookupKey&);
		void operator=(const LookupKey&);
	};

	inline LookupKey::~LookupKey()
	{
		if (start_ != space_)
			delete[] start_;
	}
}

#endif // !STORAGE_LEVELDB_DB_FORMAT_H_
