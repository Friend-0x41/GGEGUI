#pragma once

#include "ObjectPool.h"

template<typename T>
class ObjectTree final
{
public:

	ObjectTree()
	{

	}

	void insert(const T& obj)
	{

	}

	void erase(const T& obj)
	{

	}

	void destroy() noexcept
	{
		_destroy(m_p_root);
	}

	int size() const noexcept
	{
		return m_size;
	}

protected:
	// ...

private:

	struct Node
	{
		Node* m_p_child_node{ nullptr };
		Node* m_p_next_sibling{ nullptr };
		T data;
	};

	void _destroy(Node* p_node)
	{
		if (p_node && p_node->m_p_child_node)
		{
			_destroy(p_node->m_p_child_node);
		}
		while (p_node)
		{
			Node* p_next_node = p_node->m_p_next_sibling;
			m_alloc.dealloc(p_node);
			p_node = p_next_node;
		}
	}

	Node* m_p_root{ nullptr };
	ObjectPool<T> m_alloc;
	int m_size{ 0 };
};