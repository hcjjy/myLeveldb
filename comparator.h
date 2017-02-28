#ifndef  STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
#define	 STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_

#include<string>

namespace leveldb 
{
	class Slice;
	class Comparator 
	{
	public:
		virtual ~Comparator();
		virtual int compare(const Slice& a, const Slice& b) const = 0;
		virtual const char* name() const = 0;
		virtual void findShortestSeparator(std::string* start, const Slice& limit) const = 0;
		virtual void findShortSuccessor(std::string* key) const = 0;
	};
	extern const Comparator* bytewiseComparator();
}
#endif // ! STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
