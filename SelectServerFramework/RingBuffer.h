// version 1.0.2
#pragma once

#include <cstring>

class RingBuffer
{
public:
	RingBuffer()
		: mCapacity(DEFAULT_SIZE)
	{
		mBuffer = new char[DEFAULT_SIZE];
	}

	RingBuffer(int bufferSize)
		: mCapacity(bufferSize)
	{
		mBuffer = new char[bufferSize];
	}

	~RingBuffer()
	{
		delete[] mBuffer;
	}

	inline int GetCapacity(void) const
	{
		return mCapacity;
	}

	// 현재 사용중인 용량 얻기 (GetSize)
	inline int GetUseSize(void) const
	{
		return mSize;
	}

	// 현재 버퍼에 남은 용량 얻기
	inline int GetFreeSize(void) const
	{
		return mCapacity - mSize;
	}

	// 버퍼 포인터로 외부에서 한방에 쓸 수 있는 길이
	inline int GetDirectEnqueueSize(void) const
	{
		int freeSize = GetFreeSize();
		return (mCapacity - mRear) < freeSize ? (mCapacity - mRear) : freeSize;
	}

	// 버퍼 포인터로 외부에서 한방에 읽을 수 있는 길이
	inline int GetDirectDequeueSize(void) const
	{
		int useSize = GetUseSize();
		return (mCapacity - mFront) < useSize ? (mCapacity - mFront) : useSize;
	}

	bool Enqueue(const char* data, int dataSize)
	{
		int freeSize = GetFreeSize();
		int directEnqueueSize = GetDirectEnqueueSize();
		if (freeSize < dataSize)
		{
			return false;
		}

		char* enqueuePosition = mBuffer + mRear;

		if (directEnqueueSize >= dataSize)
		{
			memcpy(enqueuePosition, data, dataSize);
		}
		else
		{
			int firstEnqueueSize = directEnqueueSize;
			int secondEnqueueSize = dataSize - directEnqueueSize;
			memcpy(enqueuePosition, data, firstEnqueueSize);
			memcpy(mBuffer, data + firstEnqueueSize, secondEnqueueSize);
		}

		mSize += dataSize;
		mRear = (mRear + dataSize) % mCapacity;

		return true;
	}

	bool Dequeue(char* data, int dataSize)
	{
		int useSize = GetUseSize();
		int directDequeueSize = GetDirectDequeueSize();
		if (useSize < dataSize)
		{
			return false;
		}

		char* dequeuePosition = mBuffer + mFront;

		if (directDequeueSize >= dataSize)
		{
			memcpy(data, dequeuePosition, dataSize);
		}
		else
		{
			int firstDequeueSize = directDequeueSize;
			int secondDequeueSize = dataSize - directDequeueSize;
			memcpy(data, dequeuePosition, firstDequeueSize);
			memcpy(data + firstDequeueSize, mBuffer, secondDequeueSize);
		}

		mSize -= dataSize;
		mFront = (mFront + dataSize) % mCapacity;

		return true;
	}

	bool Peek(char* data, int dataSize) const
	{
		int useSize = GetUseSize();
		int directDequeueSize = GetDirectDequeueSize();
		if (useSize < dataSize)
		{
			return false;
		}

		char* peekPosition = mBuffer + mFront;

		if (directDequeueSize >= dataSize)
		{
			memcpy(data, peekPosition, dataSize);
		}
		else
		{
			int firstPeekSize = directDequeueSize;
			int secondPeekSize = dataSize - directDequeueSize;
			memcpy(data, peekPosition, firstPeekSize);
			memcpy(data + firstPeekSize, mBuffer, secondPeekSize);
		}

		return true;
	}

	inline void ClearBuffer(void)
	{
		mSize = 0;
		mRear = mFront;
	}

	inline char* GetFrontBufferPtr(void) const
	{
		return mBuffer + mFront;
	}

	inline char* GetRearBufferPtr(void) const
	{
		return mBuffer + mRear;
	}

	// Front 강제 이동(삭제), 이동 성공 여부를 반환
	inline bool MoveFront(int Size)
	{
		if (GetUseSize() < Size)
		{
			return false;
		}

		mSize -= Size;
		mFront = (mFront + Size) % mCapacity;

		return true;
	}

	// Rear 강제 이동(무의미한 요소 삽입), 이동 성공 여부를 반환
	inline bool MoveRear(int Size)
	{
		if (GetFreeSize() < Size)
		{
			return false;
		}

		mSize += Size;
		mRear = (mRear + Size) % mCapacity;

		return true;
	}

	enum
	{
		DEFAULT_SIZE = 10000
	};

private:

	char* mBuffer;
	int mCapacity;
	int mSize = 0;
	int mFront = 0;
	int mRear = 0;
};