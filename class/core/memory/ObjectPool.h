#pragma once

#include "MemoryPool.h"

enum Error_state
{
	dealloc_nullptr
};

template<typename T>
class ObjectPool final
{
public:

	ObjectPool()
	{

	}

	int get_mem_used() const noexcept
	{
		return m_alloc.get_mem_used();
	}

	int get_obj_count() const noexcept
	{
		return m_size;
	}

	T* alloc() noexcept
	{
		void* p_obj = m_alloc.alloc();
		if (!p_obj)
		{
			return nullptr;
		}
		++m_size;
		return static_cast<T*>(p_obj);
	}

	Error_state dealloc(T* p_obj) noexcept
	{
		if (!p_obj)
		{
			return dealloc_nullptr;
		}
		m_alloc.dealloc(p_obj);
		--m_size;
	}

	void destroy_all() noexcept
	{
		m_alloc.free_all_chunks();
		m_size = 0;
	}

	void recycle_all() noexcept
	{
		m_alloc.recycle_all_chunks();
		m_size = 0;
	}

protected:
	// ...

private:

	ObjectPool(const ObjectPool&) = delete;
	ObjectPool(ObjectPool&&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;
	ObjectPool& operator=(ObjectPool&&) = delete;
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;
	void operator delete(void*) = delete;
	void operator delete[](void*) = delete;

	MemoryPool m_alloc{ sizeof(T) };
	int m_size{ 0 };
	const size_t m_obj_sz{ 8 };
};