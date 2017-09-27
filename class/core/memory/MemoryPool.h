#pragma once

#include <new>

class MemoryPool final
{
public:

	MemoryPool(size_t ele_sz)
		: m_ele_sz{ele_sz > sizeof(uintptr_t) ? ele_sz : sizeof(uintptr_t)}
	{

	}

	~MemoryPool()
	{
		free_all_chunks();
	}

	void* alloc() noexcept
	{
		if (!m_p_free_mem)
		{
			if (!_grow())
			{
				return nullptr;
			}
		}
		Link* p_mem = m_p_free_mem;
		m_p_free_mem = m_p_free_mem->m_p_next_link;
		return static_cast<void*>(p_mem);
	}

	void dealloc(void* p_ele) noexcept
	{
		Link* ptr = static_cast<Link*>(p_ele);
		ptr->m_p_next_link = m_p_free_mem;
		m_p_free_mem = ptr;
	}

	int get_mem_used() const
	{
		return m_alloc_mem;
	}

	void free_all_chunks()
	{
		Chunk* p_curent{ m_p_chunk_head };
		while (p_curent)
		{
			Chunk* p_next_ck = p_curent->m_p_next_ck;
			delete p_curent;
			p_curent = p_next_ck;
		}

		m_p_chunk_head = nullptr;
		m_p_free_mem = nullptr;
		m_alloc_mem = 0;
	}

	void recycle_all_chunks()
	{
		Chunk* p_current{ m_p_chunk_head };
		const ptrdiff_t num_ele = Chunk::storage_sz / m_ele_sz;
		while (p_current)
		{
			Chunk* p_next_ck = p_current->m_p_next_ck;
			if (p_next_ck)
			{
				reinterpret_cast<Link*>(&p_current->mem[(num_ele - 1) * m_ele_sz])->m_p_next_link
					= reinterpret_cast<Link*>(p_next_ck->mem);
			}
			p_current = p_next_ck;
		}
		m_p_free_mem = reinterpret_cast<Link*>(m_p_chunk_head->mem);
	}

protected:
	// ...

private:

	struct Link
	{
		Link* m_p_next_link{ nullptr };
	};

	struct Chunk
	{
		enum
		{
			storage_sz = 8 * 1024 - 16
		};
		Chunk* m_p_next_ck{ nullptr };
		alignas(8) char mem[storage_sz];
	};

	bool _grow() noexcept
	{
		Chunk* p_new_ck = new(std::nothrow) Chunk;
		if (!p_new_ck)
		{
			return false;
		}
		_organize_chunk(p_new_ck);
		p_new_ck->m_p_next_ck = m_p_chunk_head;
		m_p_chunk_head = p_new_ck;
		m_p_free_mem = reinterpret_cast<Link*>(m_p_chunk_head->mem);
		m_alloc_mem += Chunk::storage_sz + 16;
	}

	void _organize_chunk(Chunk* p_ck)
	{
		const ptrdiff_t num_ele = Chunk::storage_sz / m_ele_sz;
		char* p_str = p_ck->mem;
		char* p_end = &p_ck->mem[(num_ele - 1) * m_ele_sz];
		for (char* p{ p_str }; p < p_end; p += m_ele_sz)
		{
			reinterpret_cast<Link*>(p)->m_p_next_link = reinterpret_cast<Link*>(p + m_ele_sz);
		}
		reinterpret_cast<Link*>(p_end)->m_p_next_link = nullptr;
	}

	MemoryPool(const MemoryPool&) = delete;
	MemoryPool(MemoryPool&&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;
	MemoryPool& operator=(MemoryPool&&) = delete;
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;
	void operator delete(void*) = delete;
	void operator delete[](void*) = delete;

	Chunk* m_p_chunk_head{ nullptr };
	Link* m_p_free_mem{ nullptr };
	int m_alloc_mem{ 0 };
	const size_t m_ele_sz{ 8 };
};