// version 1.0.0
#pragma once

#include <cstring>

class Serializer
{
public:
	inline Serializer()
		: mCapacity(DEFAULT_SIZE)
		, mSize(0)
	{
		mBuffer = new char[DEFAULT_SIZE];
	}
	inline ~Serializer()
	{
		delete[] mBuffer;
	}

	inline void Clear()
	{
		mSize = 0;
	}

	inline int GetCapacity()
	{
		return mCapacity;
	}

	inline int GetUseSize()
	{
		return mSize;
	}

	inline int GetFreeSize()
	{
		return mCapacity - mSize;
	}

	inline char* GetBufferPointer()
	{
		return mBuffer;
	}

	inline bool InsertByte(const char* source, int insertSize)
	{
		if (GetFreeSize() < insertSize)
		{
			return false;
		}

		memcpy(mBuffer + mSize, source, insertSize);
		mSize += insertSize;

		return true;
	}

	inline bool GetByte(char* dest, int size)
	{
		if (GetUseSize() < size)
		{
			return false;
		}

		memcpy(dest, mBuffer + mSize - size, size);
		mSize -= size;

		return true;
	}

	// operator overloading - serializing
	inline Serializer& operator<<(unsigned char value)
	{
		if (GetFreeSize() < sizeof(unsigned char))
		{
			intentionalCrash();
		}

		*(unsigned char*)(mBuffer + mSize) = value;
		mSize += sizeof(unsigned char);

		return *this;
	}

	inline Serializer& operator<<(char value)
	{
		if (GetFreeSize() < sizeof(char))
		{
			intentionalCrash();
		}

		*(char*)(mBuffer + mSize) = value;
		mSize += sizeof(char);

		return *this;
	}

	inline Serializer& operator<<(unsigned short value)
	{
		if (GetFreeSize() < sizeof(unsigned short))
		{
			intentionalCrash();
		}

		*(unsigned short*)(mBuffer + mSize) = value;
		mSize += sizeof(unsigned short);

		return *this;
	}

	inline Serializer& operator<<(short value)
	{
		if (GetFreeSize() < sizeof(short))
		{
			intentionalCrash();
		}

		*(short*)(mBuffer + mSize) = value;
		mSize += sizeof(short);

		return *this;
	}

	inline Serializer& operator<<(unsigned int value)
	{
		if (GetFreeSize() < sizeof(unsigned int))
		{
			intentionalCrash();
		}

		*(unsigned int*)(mBuffer + mSize) = value;
		mSize += sizeof(unsigned int);

		return *this;
	}

	inline Serializer& operator<<(int value)
	{
		if (GetFreeSize() < sizeof(int))
		{
			intentionalCrash();
		}

		*(int*)(mBuffer + mSize) = value;
		mSize += sizeof(int);

		return *this;
	}

	inline Serializer& operator<<(unsigned long value)
	{
		if (GetFreeSize() < sizeof(unsigned long))
		{
			intentionalCrash();
		}

		*(unsigned long*)(mBuffer + mSize) = value;
		mSize += sizeof(unsigned long);

		return *this;
	}

	inline Serializer& operator<<(long value)
	{
		if (GetFreeSize() < sizeof(long))
		{
			intentionalCrash();
		}

		*(long*)(mBuffer + mSize) = value;
		mSize += sizeof(long);

		return *this;
	}

	inline Serializer& operator<<(unsigned long long value)
	{
		if (GetFreeSize() < sizeof(unsigned long long))
		{
			intentionalCrash();
		}

		*(unsigned long long*)(mBuffer + mSize) = value;
		mSize += sizeof(unsigned long long);

		return *this;
	}

	inline Serializer& operator<<(long long value)
	{
		if (GetFreeSize() < sizeof(long long))
		{
			intentionalCrash();
		}

		*(long long*)(mBuffer + mSize) = value;
		mSize += sizeof(long long);

		return *this;
	}

	inline Serializer& operator<<(float value)
	{
		if (GetFreeSize() < sizeof(float))
		{
			intentionalCrash();
		}

		*(float*)(mBuffer + mSize) = value;
		mSize += sizeof(float);

		return *this;
	}

	inline Serializer& operator<<(double value)
	{
		if (GetFreeSize() < sizeof(double))
		{
			intentionalCrash();
		}

		*(double*)(mBuffer + mSize) = value;
		mSize += sizeof(double);

		return *this;
	}

	// operator overloading - marshaling
	inline Serializer& operator>>(unsigned char& value)
	{
		if (GetUseSize() < sizeof(unsigned char))
		{
			intentionalCrash();
		}

		value = *(unsigned char*)(mBuffer + mSize - sizeof(unsigned char));
		mSize -= sizeof(unsigned char);

		return *this;
	}

	inline Serializer& operator>>(char& value)
	{
		if (GetUseSize() < sizeof(char))
		{
			intentionalCrash();
		}

		value = *(char*)(mBuffer + mSize - sizeof(char));
		mSize -= sizeof(char);

		return *this;
	}

	inline Serializer& operator>>(unsigned short& value)
	{
		if (GetUseSize() < sizeof(unsigned short))
		{
			intentionalCrash();
		}

		value = *(unsigned short*)(mBuffer + mSize - sizeof(unsigned short));
		mSize -= sizeof(unsigned short);

		return *this;
	}

	inline Serializer& operator>>(short& value)
	{
		if (GetUseSize() < sizeof(short))
		{
			intentionalCrash();
		}

		value = *(short*)(mBuffer + mSize - sizeof(short));
		mSize -= sizeof(short);

		return *this;
	}

	inline Serializer& operator>>(unsigned int& value)
	{
		if (GetUseSize() < sizeof(unsigned int))
		{
			intentionalCrash();
		}

		value = *(unsigned int*)(mBuffer + mSize - sizeof(unsigned int));
		mSize -= sizeof(unsigned int);

		return *this;
	}

	inline Serializer& operator>>(int& value)
	{
		if (GetUseSize() < sizeof(int))
		{
			intentionalCrash();
		}

		value = *(int*)(mBuffer + mSize - sizeof(int));
		mSize -= sizeof(int);

		return *this;
	}

	inline Serializer& operator>>(unsigned long& value)
	{
		if (GetUseSize() < sizeof(unsigned long))
		{
			intentionalCrash();
		}

		value = *(unsigned long*)(mBuffer + mSize - sizeof(unsigned long));
		mSize -= sizeof(unsigned long);

		return *this;
	}

	inline Serializer& operator>>(long& value)
	{
		if (GetUseSize() < sizeof(long))
		{
			intentionalCrash();
		}

		value = *(long*)(mBuffer + mSize - sizeof(long));
		mSize -= sizeof(long);

		return *this;
	}

	inline Serializer& operator>>(unsigned long long& value)
	{
		if (GetUseSize() < sizeof(unsigned long long))
		{
			intentionalCrash();
		}

		value = *(unsigned long long*)(mBuffer + mSize - sizeof(unsigned long long));
		mSize -= sizeof(unsigned long long);

		return *this;
	}

	inline Serializer& operator>>(long long& value)
	{
		if (GetUseSize() < sizeof(long long))
		{
			intentionalCrash();
		}

		value = *(long long*)(mBuffer + mSize - sizeof(long long));
		mSize -= sizeof(long long);

		return *this;
	}

	inline Serializer& operator>>(float& value)
	{
		if (GetUseSize() < sizeof(float))
		{
			intentionalCrash();
		}

		value = *(float*)(mBuffer + mSize - sizeof(float));
		mSize -= sizeof(float);

		return *this;
	}

	inline Serializer& operator>>(double& value)
	{
		if (GetUseSize() < sizeof(double))
		{
			intentionalCrash();
		}

		value = *(double*)(mBuffer + mSize - sizeof(double));
		mSize -= sizeof(double);

		return *this;
	}

	enum
	{
		DEFAULT_SIZE = 1500
	};
private:

#pragma warning(push)
#pragma warning(disable: 6011)
	void intentionalCrash()
	{
		int* nullPtr = 0;
		*nullPtr = 0xFFFFFFFF;
	}
#pragma warning(pop)

	int mCapacity;
	int mSize;
	char* mBuffer;
};