#include"iterator.h"

namespace leveldb
{
	Iterator::Iterator()
	{
		cleanup_.function = NULL;
		cleanup_.next = NULL;
	}

	Iterator::~Iterator()
	{
		if (cleanup_.function != NULL)
		{
			(*cleanup_.function)(cleanup_.arg1, cleanup_.arg2);
			for (Cleanup* p = cleanup_.next; p != NULL; )
			{
				(*p->function)(p->arg1, p->arg2);
				Cleanup* next = p->next;
				delete p;
				p = next;
			}
		}
	}

	void Iterator::registerCleanup(CleanupFunction func, void* arg1, void* arg2)
	{
		assert(func != NULL);
		Cleanup* c;
		if (cleanup_.function == NULL)
		{
			c = &cleanup_;
		}
		else
		{
			c = new Cleanup;
			c->next = cleanup_.next;
			cleanup_.next = c;
		}
		c->function = func;
		c->arg1 = arg1;
		c->arg2 = arg2;
	}

	namespace
	{
		class EmptyIterator : public Iterator
		{
		public:
			EmptyIterator(const Status& s) : status_(s) { }
			virtual bool valid() const { return false; }
			virtual void seek(const Slice& target) { }
			virtual void seekToFirst() { }
			virtual void seekToLast() { }
			virtual void next() { assert(false); }
			virtual void prev() { assert(false); }
			Slice key() const { assert(false); return Slice(); }
			Slice value() const { assert(false); return Slice(); }
			virtual Status status() const { return status_; }
		private:
			Status status_;
		};
	}
	Iterator* newEmptyIterator()
	{
		return new EmptyIterator(Status::OK());
	}
	Iterator* newErrorIterator(const Status& status)
	{
		return new EmptyIterator(status);
	}
}