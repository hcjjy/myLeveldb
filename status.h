#ifndef STORAGE_LEVELDB_INCLUDE_STATUS_H_
#define STORAGE_LEVELDB_INCLUDE_STATUS_H_

#include<stdint.h>
#include"slice.h"
namespace leveldb
{
	class Status
	{
	public:
		Status() : state_(NULL) {}
		~Status() { delete[] state_; }
		Status(const Status& s);
		void operator=(const Status& s);
		static Status OK() { return Status(); }
		static Status notFound(const Slice& msg, const Slice& msg2 = Slice())
		{
			return Status(kNotFound, msg, msg2);
		}
		static Status corruption(const Slice& msg, const Slice& msg2 = Slice())
		{
			return Status(kCorruption, msg, msg2);
		}
		static Status notSupported(const Slice& msg, const Slice& msg2 = Slice())
		{
			return Status(kNotSupported, msg, msg2);
		}
		static Status invalidArgument(const Slice& msg, const Slice& msg2 = Slice())
		{
			return Status(kInvalidArgument, msg, msg2);
		}
		static Status IOError(const Slice& msg, const Slice& msg2 = Slice())
		{
			return Status(kIOError, msg, msg2);
		}
		bool ok() const { return state_ == NULL; }
		bool isNotFound() const { return code() == kNotFound; }
		bool isCorruption() const { return code() == kCorruption; }
		bool isNotSupported() const { return code() == kNotSupported; }
		bool isInvalidArgument() const { return code() == kInvalidArgument; }
		bool isIOError() const { return code() == kIOError; }
		std::string toString() const;
	private:
		// OK status has a NULL state_.  Otherwise, state_ is a new[] array
		// of the following form:
		//    state_[0..3] == length of message
		//    state_[4]    == code
		//    state_[5..]  == message
		const char* state_;
		enum Code
		{
			kOk = 0,
			kNotFound = 1,
			kCorruption = 2,
			kNotSupported = 3,
			kInvalidArgument = 4,
			kIOError = 5
		};
		Code code() const{ return state_ == NULL ? kOk : static_cast<Code>(state_[4]); }
		static const char* copyState(const char* state);
		Status(Code code, const Slice& msg, const Slice& msg2);
	};
	inline Status::Status(const Status& s)
	{
		state_ = s.state_ == NULL ? NULL : copyState(s.state_);
	}
	inline void Status::operator=(const Status & s)
	{
		//or (state_ != s.state_)
		if (this != &s )
		{
			delete[] state_;
			state_ = s.state_ == NULL ? NULL : copyState(s.state_);
		}
	}
}

#endif 