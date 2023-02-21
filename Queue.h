#pragma once
template <typename T>
struct Node
{
	Node() :next(nullptr), prev(nullptr) {}
	Node(T _data) :data(_data), next(nullptr), prev(nullptr) {}
	T data;
	Node* next;
	Node* prev;
};
template <typename T>
class Queue
{
private:
	Node<T>* head;
	Node<T>* tail;
	size_t _size;

public:
	Queue() :head(nullptr), tail(nullptr), _size(0) {}
	void Enqueue(T _data)
	{
		if (head == nullptr)
		{
			head = new Node<T>(_data);
			tail = head;
		}
		else
		{
			tail->next = new Node<T>(_data);
			tail->next->prev = tail;
			tail = tail->next;
		}
		_size++;
	}
	void Dequeue()
	{
		Node<T>* temp = head->next;
		delete head;
		head = temp;
		if (head != nullptr)
			head->prev = nullptr;
		_size--;
	}
	T front()
	{
		return head->data;
	}
	size_t size()
	{
		return _size;
	}
	bool is_empty()
	{
		if (head == nullptr)
			return true;
		return false;
	}
	~Queue()
	{
		Node<T>* ptr = head;
		while (ptr != nullptr)
		{
			Node<T>* temp = ptr->next;
			delete ptr;
			ptr = temp;
		}
	}
};

