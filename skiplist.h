#ifndef STORAGE_LEVELDB_DB_SKIPLIST_H_
#define STORAGE_LEVELDB_DB_SKIPLIST_H_

#include<assert.h>
#include<stdlib.h>
#include"arena.h"
#include"random.h"
#include"status.h"
#include"port_win.h"

namespace leveldb
{
	template<typename Key, class Comparator>
	class SkipList
	{
	private:
		struct Node;
	public:
		explicit SkipList(Comparator* cmp, Arena* arena);
		void insert(const Key& key);
		bool contains(const Key& key) const;
		class Iterator
		{
		public:
			explicit Iterator(const SkipList* list);
			bool valid() const;
			const Key& key() const;
			void next();
			void prev();
			void seek(const Key& target);
			void seekToFirst();
			void seekToLast();
		private:
			const SkipList* list_;
			Node* node_; //保存指向当前结点的指针
		};
	private:
		Node* newNode(const Key& key, int hight);
		Node* findEqualOrGreater(const Key& key, Node** pre) const;
		Node* findLessThan(const Key& key) const;
		Node* findLast() const;
		int randomHeight();
		bool equal(const Key& a, const Key& b) const { return compare_(a, b) == 0; }
		enum{kMaxHeight = 12};
		Comparator* const compare_;
		Arena* const arena_; //用于分配节点内存
		Node* const head_; //头结点指针
		Random rnd_;
		port::AtomicPointer curMaxHeight_;
		inline int getCurMaxHeight() const
		{
			return reinterpret_cast<intptr_t>(curMaxHeight_.NoBarrier_Load());
		}
		//不允许拷贝
		SkipList(const SkipList&);
		void operator=(const SkipList&);
	};

	//实现细节
	template<typename Key, class Comparator>
	struct SkipList<Key, Comparator>::Node
	{
		Node(const Key& key) :key_(key) {}
		Key const key_;
		//原子操作获得
		Node* next(int n)
		{
			assert(n >= 0);
			return reinterpret_cast<Node*>(next_[n].Acquire_Load());
		}
		//原子操作存储
		void setNext(int n, Node* x)
		{
			assert(n >= 0);
			next_[n].Release_Store(x);
		}
		//正常操作获得
		Node* noBarrier_Next(int n)
		{
			assert(n >= 0);
			return reinterpret_cast<Node*>(next_[n].NoBarrier_Load());
		}
		//正常操作存储
		void noBarrier_SetNext(int n, Node* x)
		{
			assert(n >= 0);
			next_[n].NoBarrier_Store(x);
		}
	private:
		port::AtomicPointer next_[1];
	};

	template<typename Key, class Comparator>
	SkipList<Key, Comparator>::SkipList(Comparator* cmp, Arena* arena)
		:compare_(cmp),
		arena_(arena),
		head_(newNode(0, kMaxHeight)),
		rnd_(0xdeadbeef),
		curMaxHeight_(reinterpret_cast<void*>(1))
	{
		for (int i = 0; i < kMaxHeight; ++i)
			head_->setNext(i, NULL);
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::insert(const Key& key)
	{
		Node* pre[kMaxHeight];
		Node* node = findEqualOrGreater(key, pre);
		assert(node && !equal(node->key_, key));
		int h = randomHeight();
		if (h > getCurMaxHeight())
		{
			//he_ 可以不用了吧
			//for (int i = curMaxHeight_; i < h; ++i)
			//	prev[i] = head_;
			curMaxHeight_.NoBarrier_Store(reinterpret_cast<void*>(h));
		}
		node = newNode(key, h);
		for (int i = h - 1; i >= 0; --i)
		{
			node->noBarrier_SetNext(i,pre[i]->noBarrier_Next(i));
			pre[i]->setNext(i, node);
		}
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node*
	SkipList<Key, Comparator>::newNode(const Key& key, int height) 
	{
		char* allocate = arena_->allocateAligned(sizeof(Node) + sizeof(height)*(height - 1));
		Node* result = new (allocate)Node(key);
		return result;
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node*
	SkipList<Key, Comparator>::findEqualOrGreater(const Key& key, Node** pre) const
	{
		int level = kMaxHeight - 1;
		Node* x = head_;
		Node* next;
		while (level >= 0)
		{
			next = x->next(level);
			if (next == NULL || compare_(next->key_, key) >= 0)
			{
				if (pre[level])
					pre[level] = x;
				--level;
			}
			else
			{
				x = next;
			}
		}
		return next;
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node* 
	SkipList<Key, Comparator>::findLessThan(const Key& key) const
	{
		int level = kMaxHeight - 1;
		Node* x = head_;
		while (level >= 0)
		{
			//he_ 可以不用吧
			//assert(x == head_ || compare_(x->key_, key) < 0);
			Node* next = x->next(level);
			if (next == NULL || compare_(next->key_, key) >= 0)
				--level;
			else
				x = next;
		}
		return x;
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node*
	SkipList<Key, Comparator>::findLast() const
	{
		int level = kMaxHeight - 1;
		Node* x = head_;
		while (level >= 0)
		{
			Node* next = x->next(0);
			if (next == NULL)
				--level;
			else
				x = next;
		}
		return x;
		//没有利用跳表的优势，这样遍历太慢了
		//Node* x = head;
		//while (x->next_[0] != NULL)
		//	x = x->next_[0];
		//return x;
	}

	template<typename Key, class Comparator>
	int SkipList<Key, Comparator>::randomHeight()
	{
		//以概率为四分之一增加高度
		static const unsigned int kBranching = 4;
		int height = 1;
		while (height < kMaxHeight && (rnd_.next() % kBranching == 0))
		{
			++height;
		}
		assert(height > 0);
		assert(height <= kMaxHeight);
		return height;
	}

	template<typename Key, class Comparator>
	bool SkipList<Key, Comparator>::contains(const Key& key) const
	{
		Node* node = findEqualOrGreater(key, NULL);
		return (node != NULL && equal(node->key_, key));
	}

	//内置迭代器
	template<typename Key, class Comparator>
	SkipList<Key, Comparator>::Iterator::Iterator(const SkipList* list)
	{
		list_ = list;
		node_ = NULL;
	}

	template<typename Key, class Comparator>
	bool SkipList<Key, Comparator>::Iterator::valid() const
	{
		return node_ != NULL;
	}

	template<typename Key, class Comparator>
	const Key& SkipList<Key, Comparator>::Iterator::key() const
	{
		assert(valid());
		return node_->key;
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::Iterator::next()
	{
		assert(valid());
		return node_->next(0);
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::Iterator::prev()
	{
		assert(valid());
		node_ = list_->findLessThan(node->key_);
		if (node_ == list_head_)
			node_ = NULL;
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::Iterator::seek(const Key& target)
	{
		node_ = list_->findEqualOrGreater(target, NULL);
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::Iterator::seekToFirst()
	{
		node_ = list_->head_->next(0);
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::Iterator::seekToLast()
	{
		node_ = list_->findLast();
		if (node_ == list_->head_)
			node_ = NULL;
	}
}

#endif // !STORAGE_LEVELDB_DB_SKIPLIST_H_

