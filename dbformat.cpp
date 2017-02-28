#include"dbformat.h"


namespace leveldb
{
	static uint64_t packSequenceAndType(uint64_t seq, ValueType t)
	{
		assert(seq <= kMaxSequenceNumber);
		assert(t <= kValueTypeForSeek);
		return (seq<<8) | t;
	}

	void appendInternalKey(std::string* result, const ParsedInternalKey& key)
	{
		result->append(key.user_key.data(), key.user_key.size());
		putFixed64(result, packSequenceAndType(key.sequence, key.type));
	}

	std::string ParsedInternalKey::debugString() const
	{
		char buf[50];
		snprintf(buf, sizeof(buf), "' @ %llu : %d", (unsigned long long) sequence, int(type));
		std::string result = "'";
		result += user_key.toString();
		result += buf;
		return result;
	}

	std::string InternalKey::debugString() const
	{
		std::string result;
		ParsedInternalKey parsed;
		if (parseInternalKey(rep_, &parsed))
		{
			result = parsed.debugString();
		}
		else
		{
			result = "(bad)";
			result.append(EscapeString(rep_));
		}
		return result;
	}

	const char* InternalKeyComparator::name()const
	{
		return "leveldb.InternalKeyComparator";
	}

	int InternalKeyComparator::compare(const Slice& akey, const Slice& bkey) const
	{
		int r = user_comparator_->compare(extractUserKey(akey), extractUserKey(bkey));
		if (r == 0)
		{
			const uint64_t aseq_t = decodeFixed64(akey.data() + akey.size() - 8);
			const uint64_t bseq_t = decodeFixed64(bkey.data() + bkey.size() - 8);
			if (aseq_t < bseq_t)
				r = -1;
			else if (aseq_t > bseq_t)
				r = +1;
		}
		return r;
	}

	void InternalKeyComparator::findShortestSeparator(std::string* start,
		const Slice& limit) const
	{
		Slice user_start = extractUserKey(*start);
		Slice user_limit = extractUserKey(limit);
		std::string tmp(user_start.data(), user_start.size());
		user_comparator_->findShortestSeparator(&tmp, user_limit);
		if (user_comparator_->compare(*start, tmp) < 0)
		{
			putFixed64(&tmp, packSequenceAndType(kMaxSequenceNumber, kValueTypeForSeek));
			assert(this->compare(*start, tmp) < 0);
			assert(this->compare(tmp, limit) < 0);
			start->swap(tmp);
		}
	}

	void InternalKeyComparator::findShortSuccessor(std::string* key)const
	{
		Slice user_key = extractUserKey(*key);
		std::string tmp(user_key.data(), user_key.size());
		user_comparator_->findShortSuccessor(&tmp);
		if (user_comparator_->compare(user_key, tmp) < 0)
		{
			putFixed64(&tmp, packSequenceAndType(kMaxSequenceNumber, kValueTypeForSeek));
			assert(this->compare(*key, tmp) < 0);
			key->swap(tmp);
		}
	}

	LookupKey::LookupKey(const Slice& user_key, SequenceNumber s)
	{
		size_t usize = user_key.size();
		size_t needed = usize + 13;//at most: 5 + usize + 8
		char *dst;
		if (needed <= sizeof(space_))
			dst = space_;
		else
			dst = new char[needed];
		start_ = dst;
		dst = encodeVarint32(dst, usize + 8);
		kstart_ = dst;
		memcpy(dst, user_key.data(), usize);
		dst += usize;
		encodeFixed64(dst, packSequenceAndType(s, kValueTypeForSeek));
		dst += 8;
		end_ = dst;
	}
}
