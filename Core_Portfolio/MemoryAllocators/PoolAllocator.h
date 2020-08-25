//Matthew Rosen
#pragma once

#include <cstdlib>
#define ASSERT(condition) if((cond)) {} else { __debugbreak(); }

// internal pool allocator used by PoolAllocator<T>
// DON'T USE THIS UNLESS YOU REALLY NEED IT
// pool allocator, no paging, one block at a time
template<unsigned B>
class PoolAllocatorImpl
{
	struct PoolObject
	{
		PoolObject* next;
	};
public:
	PoolAllocatorImpl(unsigned maxObjects, bool allowExtraAllocations = true)
	{
		static_assert(B >= sizeof(void*), "PoolAllocatorImpl is created with size B too small!");
		m_sizePerObject = B;
		m_maxObjects = maxObjects;
		m_poolSize = m_maxObjects * m_sizePerObject;
		m_numObjects = 0; 
		m_allowExtraAllocations = allowExtraAllocations;
		m_numExtraAllocations = 0; 
		m_freeList = nullptr;

		CreatePool();
	}

	~PoolAllocatorImpl()
	{
		// ensure that there aren't un-free extra allocations (memory leaks)
		ASSERT(m_numExtraAllocations == 0);
		if (m_pool)
		{
			free(m_pool);
			m_pool = nullptr;
		}

		m_freeList = nullptr;
	}

	// raw allocate one block of bytes
	void* Alloc()
	{
		ASSERT(m_pool);

		// case no more available objects
		if (m_freeList == nullptr)
		{
			// assert no more blocks available
			ASSERT(m_allowExtraAllocations);
			++m_numExtraAllocations;
			++m_numObjects;

			// allocate on the heap
			void* object = malloc(m_sizePerObject);
			return object;
		}
		// case objects available
		else
		{
			// pop front of the free linked list
			PoolObject* object = m_freeList;
			m_freeList = m_freeList->next;
			++m_numObjects;
			return object;
		}
	}

	void Free(void* object)
	{
		ASSERT(object && m_pool);
		char* obj = (char*)object;
		size_t objDistance, poolDistance;
		objDistance = obj - m_pool;
		poolDistance = (m_pool + m_poolSize) - m_pool;

		// case object is not within the pool
		if (poolDistance < objDistance && objDistance >= 0)
		{
			ASSERT(m_allowExtraAllocations);
			--m_numExtraAllocations;

			// free from heap
			free(object);
		}
		// case object is within the pool
		else
		{
			// push front to singly linked list
			PoolObject* memObj = (PoolObject*)(object);
			memObj->next = m_freeList;
			m_freeList = memObj;
		}

		--m_numObjects;
	}

	void Clear()
	{
		// free entire pool
		ASSERT(m_numExtraAllocations == 0);
		free(m_pool);
		m_freeList = nullptr;

		// recreate entire pool
		CreatePool();
	}

private:

	void CreatePool()
	{
		m_pool = (char*)malloc(m_poolSize);
		ASSERT(m_pool && "allocating space for memory pool failed!");

		// init the free list
		for (unsigned i = 0; i < m_maxObjects; ++i)
		{
			PoolObject* obj = (PoolObject*)(m_pool + i * m_sizePerObject);
			obj->next = m_freeList;
			m_freeList = obj;
		}
	}

	unsigned m_sizePerObject;			// bytes per object (same as B)
	unsigned m_maxObjects;				// max number of objects in the pool (pool isn't dynamic resizing)
	unsigned m_numObjects;				// current number of objects in the pool
	unsigned m_poolSize;				// size of the pool in bytes
	unsigned m_numExtraAllocations;		// count of extra allocations outside of the pool
	char* m_pool;						// pool of bytes
	PoolObject* m_freeList;				// singly linked list of free objects
	bool m_allowExtraAllocations;		// whether to allow allocations outside of pool in case max objects isn't enough
};

// templated pool allocator
// it can handle extra allocations beyond the max objects, but won't clean up
// that memory for you.
// sizeof(T) must be >= sizeof(void*) or won't compile
// Alloc doesn't call T's constructor,
// Construct does
// Free doesn't call T's destructor
// Destruct does
// Clear doesn't call destructors
template<typename T>
class PoolAllocator
{
public:
	PoolAllocator(u32 maxObjects, bool allowExtraAllocations = true)
		: m_allocator(maxObjects, allowExtraAllocations) {}

	T* Alloc()
	{
		return reinterpret_cast<T *>(m_allocator.Alloc());
	}

	template<typename... Args>
	T* Construct(Args... args)
	{
		T* obj = Alloc();

		// pass args to placement new
		T* mem = new (obj) T(args...);
		return obj;
	}

	void Free(T* address)
	{
		m_allocator.Free(address);
	}

	void Destruct(T* address)
	{
		// destruct then free
		address->~T();
		Free(address);
	}

	void Clear()
	{
		m_allocator.Clear();
	}

private:
	PoolAllocatorImpl<sizeof(T)> m_allocator;	// internal version, deals with the bytes itself
};
