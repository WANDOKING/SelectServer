// doubly linked list

#pragma once

#include <cassert>

namespace mds
{
	template <typename T>
	class list
	{
	private:
		struct Node
		{
			T Data{};
			Node* Prev{};
			Node* Next{};
		};

	public:
		class iterator
		{
			friend iterator list<T>::erase(iterator iter);
		public:
			inline iterator(Node* node = nullptr)
				: mNode(node)
			{};

			inline iterator& operator++()
			{
				mNode = mNode->Next;
				return *this;
			}

			inline const iterator operator++(int)
			{
				const iterator ret(mNode);
				mNode = mNode->Next;
				return ret;
			}

			inline iterator& operator--()
			{
				mNode = mNode->Prev;
				return *this;
			}

			inline const iterator operator--(int)
			{
				const iterator ret(mNode);
				mNode = mNode->Prev;
				return ret;
			}

			inline T& operator*() const
			{
				return mNode->Data;
			}

			inline bool operator==(const iterator& other) const
			{
				return mNode == other.mNode;
			}

			inline bool operator!=(const iterator& other) const
			{
				return mNode != other.mNode;
			}
		private:
			Node* mNode;
		};

		inline list()
		{
			mHead.Prev = nullptr;
			mHead.Next = &mTail;
			mTail.Prev = &mHead;
			mTail.Next = nullptr;
		}

		inline ~list()
		{
			this->clear();
		}

		inline iterator begin() const
		{
			return iterator(mHead.Next);
		}

		inline iterator end()
		{
			return iterator(&mTail);
		}

		inline T front() const
		{
			assert(mSize > 0);
			return mHead.Next->Data;
		}

		inline T back() const
		{
			assert(mSize > 0);
			return mTail.Prev->Data;
		}

		void push_front(T data)
		{
			Node* newNode = new Node;
			newNode->Data = data;

			newNode->Prev = &mHead;
			newNode->Next = mHead.Next;
			mHead.Next->Prev = newNode;
			mHead.Next = newNode;

			++mSize;
		}

		void push_back(T data)
		{
			Node* newNode = new Node;
			newNode->Data = data;

			newNode->Prev = mTail.Prev;
			newNode->Next = &mTail;
			mTail.Prev->Next = newNode;
			mTail.Prev = newNode;

			++mSize;
		}

		void pop_front()
		{
			assert(mSize > 0);

			Node* deleteNode = mHead.Next;
			deleteNode->Next->Prev = &mHead;
			mHead.Next = deleteNode->Next;

			delete deleteNode;
			--mSize;
		}

		void pop_back()
		{
			assert(mSize > 0);

			Node* deleteNode = mTail.Prev;
			deleteNode->Prev->Next = &mTail;
			mTail.Prev = deleteNode->Prev;

			delete deleteNode;
			--mSize;
		}

		void clear()
		{
			Node* deleteNode = mHead.Next;
			Node* nextNode = deleteNode->Next;

			while (deleteNode != &mTail)
			{
				delete deleteNode;
				deleteNode = nextNode;
				nextNode = deleteNode->Next;
			}

			mHead.Next = &mTail;
			mTail.Prev = &mHead;

			mSize = 0;
		}

		iterator erase(iterator iter)
		{
			assert(iter.mNode != nullptr);
			assert(iter.mNode != &mHead);
			assert(iter.mNode != &mTail);

			iterator ret(iter.mNode);
			++ret;

			Node* deleteNode = iter.mNode;
			deleteNode->Prev->Next = deleteNode->Next;
			deleteNode->Next->Prev = deleteNode->Prev;

			delete deleteNode;
			--mSize;

			return ret;
		}

		void remove(T data)
		{
			Node* visit = mHead.Next;

			while (visit != &mTail)
			{
				if (visit->Data != data)
				{
					visit = visit->Next;
					continue;
				}

				visit->Prev->Next = visit->Next;
				visit->Next->Prev = visit->Prev;

				delete visit;
				--mSize;

				break;
			}
		}

		inline size_t size() const
		{
			return mSize;
		}

		inline bool empty() const
		{
			return mSize == 0;
		}

	private:
		size_t mSize = 0;
		Node mHead;
		Node mTail;
	};

}