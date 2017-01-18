#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include<assert.h>
//#include<stddef.h>
#include<string.h>
#include<string>
namespace leveldb
{
	class Slice
	{
	public:
		Slice() :data_(""), size_(0) {}
		Slice(const std::string& str):data_(str.data()),size_(str.length()){}
		Slice(const char* data) :data_(data), size_(strlen(data)) {}
		Slice(const char* data, size_t n) : data_(data), size_(n) {}
		//Slice(const Slice& slice) :data_(slice.data_), size_(slice.size_) {std::cout <<data_<< ":拷贝构造函数"<<std::endl; }
		//Slice& operator=(const Slice& slice);
		bool operator==(const Slice& slice);
		bool operator!=(const Slice& slice);
		char operator[](size_t n) const { assert(n < size_); return data_[n]; }
		const char* data() const { return data_; }
		size_t size() const { return size_; }
		void clear() { data_ = ""; size_ = 0; }
		bool empty()const { return size_ == 0; }
		void remove_prefix(size_t n) { assert(n <= size_); data_ += n; size_ -= n; }
		std::string toString()const { return std::string(data_, size_);}
		int compare(const Slice& b)const;
		bool starts_with(const Slice& x) const { return (size_ >= x.size_) && memcmp(data_, x.data_, x.size_) == 0; }
	private:
		const char* data_;
		size_t size_;
	};

	/*inline Slice& Slice::operator=(const Slice& slice)
	{
		std::cout << "赋值操作" << std::endl;
		if (this != &slice)
		{
			data_ = slice.data_;
			size_ = slice.size_;
		}
		return *this;
	}*/
	inline bool Slice::operator==(const Slice& slice)
	{
		return size_ == slice.size_ &&
				memcmp(data_, slice.data_, size_) == 0;
	}
	inline bool Slice::operator!=(const Slice& slice)
	{
		return !(*this==slice);
	}
	inline int Slice::compare(const Slice& b)const
	{
		//windows本可以使用strcmp,但是考虑到不同平台strcmp实现不同，故自己实现
		const int min_len = size_ < b.size_ ? size_ : b.size_;
		int r = memcmp(data_, b.data_, min_len);
		if (r == 0)
		{
			if (size_ < b.size_)
				r = -1;
			else if (size_ > b.size_)
				r = +1;
		}
		return r;
	}
}

#endif // !STORAGE_LEVELDB_INCLUDE_SLICE_H_
