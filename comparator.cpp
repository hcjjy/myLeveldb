#include"comparator.h"
#include<algorithm>
#include"slice.h"

namespace leveldb
{
	Comparator::~Comparator() {}

	namespace
	{
		class BytewiseComparatorImpl : public Comparator
		{
		public:
			BytewiseComparatorImpl() {}
			virtual const char* name() const
			{
				return "leveldb.BytewiseComparator";
			}
			virtual int compare(const Slice& a, const Slice& b) const
			{
				return a.compare(b);
			}
			virtual void findShortestSeparator(std::string* start, const Slice& limit) const
			{
				size_t min_length = std::min(start->size(), limit.size());
				size_t diff_index = 0;
				while ((diff_index < min_length) && 
						((*start)[diff_index] == limit[diff_index]))
				{
					++diff_index;
				}
				if (diff_index < min_length)
				{
					uint8_t diff_byte = static_cast<uint8_t>((*start)[diff_index]);
					if (diff_byte < static_cast<uint8_t>(0xff) &&
						diff_byte + 1 < static_cast<uint8_t>(limit[diff_index]))
					{
						++(*start)[diff_index];
						start->resize(diff_byte + 1);
						assert(compare(*start, limit) < 0);
					}
				}
			}
			virtual void findShortSuccessor(std::string* key) const
			{
				size_t n = key->size();
				for (size_t i = 0; i < n; ++i)
				{
					const uint8_t byte = (*key)[i];
					if (byte != static_cast<uint8_t>(0xff))
					{
						(*key)[i] = byte + 1;
						key->resize(i + 1);
						return ;
					}
				}
			}
		};
	}
	static const BytewiseComparatorImpl bytewise;
	const Comparator* bytewiseComparator()
	{
		return &bytewise;
	}
}