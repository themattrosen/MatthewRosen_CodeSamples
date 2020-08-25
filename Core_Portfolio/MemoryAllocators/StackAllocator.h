//Matthew Rosen
#pragma once

#include <cstdlib>

#define ASSERT(condition) if((cond)) {} else { __debugbreak(); }


// simple stack allocator
// Alloc<T> allocates an object without constructing it
//		it can also allocate an array of objects without construction
// Construct<T, Args...> allocates an object (or array of objects) and constructs it (or them)
// Free<T> frees an object or array of objects without calling its destructor
// Destruct<T> frees an object after calling its destructor
//		Free and Destruct can also free an array of objects (Destruct will call dtor on each)
//		Must pass in number of elements if its an array
// Clear resets internal ptrs, doesn't destruct any elements
//
// Only basic safety checking is done, a lot of trust is placed in the user
// Usage: 
//	* when allocating and freeing, you must free objects in reverse order that 
//    they were allocated in
//	* you can allocate objects of any type, and those objects are guaranteed
//    to be in contiguous memory
//	* if your ptr becomes misaligned, freeing it will be catastrophic, and 
//    there's no way for the stack allocator to know until it crashes
class StackAllocator
{
public:
	StackAllocator(size_t sizeInBytes)
		: m_stackSize(sizeInBytes)
	{
		m_memStack = (char *)malloc(sizeInBytes);
		ASSERT(m_memStack);
		std::memset(m_memStack, 0, sizeInBytes);
		m_nextPtr = m_memStack;
	}

	~StackAllocator()
	{
		ASSERT(m_nextPtr >= m_memStack &&
			"Some pointer became misaligned before freeing. This is really bad");

		if (m_memStack)
		{
			free(m_memStack);
			m_memStack = nullptr;
		}

		m_nextPtr = nullptr;
		m_stackSize = 0;
	}

	template<typename T>
	T* Alloc(unsigned numElements = 1)
	{
		// ensure that stack can support this allocation
		ASSERT(m_nextPtr + sizeof(T) * numElements <= m_memStack + m_stackSize &&
			"Allocation too big, would go outside stack!")

		// reinterpret top of stack
		T* mem = reinterpret_cast<T*>(m_nextPtr);

		// set top of stack to be where this allocation ends
		m_nextPtr += sizeof(T) * numElements;

		return mem;
	}

	template<typename T, typename... Args>
	T* Construct(Args... args)
	{
		// allocate object on the stack
		T* obj = Alloc<T>();

		// in-place construct object
		if(obj)
			T* mem = new (obj) T(args...);
		return obj;
	}

	template<typename T>
	void Free(T* address, unsigned numElements = 1)
	{
		// assure address is within stack
		ASSERT(address && (char*)address == m_nextPtr - sizeof(T) * numElements);

		// move stack pointer back down the stack
		m_nextPtr -= sizeof(T) * numElements;

		// reset bytes to 0 in reclaimed memory
		std::memset(m_nextPtr, 0, sizeof(T) * numElements);
	}

	template<typename T>
	void Destruct(T* address, unsigned numElements = 1)
	{
		ASSERT(address);

		// call dtor on each element in allocated array (works for non-arrays as well)
		for(unsigned i = 0; i < numElements; ++i)
			address[i].~T();

		// use free function
		Free(address);
	}

	void Clear()
	{
		// reset stack pointer to the top
		m_nextPtr = m_memStack;
	}

private:
	size_t m_stackSize;	// size of allocated stack in bytes
	char* m_memStack;	// ptr to bottom of the stack
	char* m_nextPtr;	// ptr to current top of stack
};
