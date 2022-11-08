#include <iostream>
#include <vector>

template<typename T, int SIZE = 10>
class mydeque
{
private:
	class Block
	{
	private:
		std::vector<T> data_array;
		bool front_flag;
		int front_index;
		int back_index;
	public:
		Block()
		{

		}
		Block(int _capacity, bool _front_flag = true) :front_flag(_front_flag)
		{
			if (front_flag)
			{
				front_index = _capacity - 1;
				data_array.resize(_capacity);
			}
			else
			{
				front_index = -1;
				data_array.reserve(_capacity);
			}
		}
		~Block()
		{

		}
		bool push_back_check()
		{
			if (data_array.size() == data_array.capacity())
				return false;
			else return true;
		}
		bool push_front_check()
		{
			if (front_index >= 0)
				return true;
			else return false;
		}
		void push_back(T _data)
		{
			data_array.push_back(_data);
		}
		void push_front(T _data)
		{
			data_array[front_index] = _data;
			front_index--;
		}
	};
	Block* block_array[SIZE];
	int capacity;
	int begin_index;
	int end_index;
public:
	mydeque() :begin_index(SIZE / 2), end_index(SIZE / 2), capacity(5)
	{
		block_array[begin_index] = new Block(capacity, false);
	}
	mydeque(int _capacity) :begin_index(SIZE / 2), end_index(SIZE / 2), capacity(_capacity)
	{
		block_array[begin_index] = new Block(capacity, false);
	}
	void push_back(const T& _data)
	{
		if (end_index < SIZE && !block_array[end_index]->push_back_check())
		{
			end_index++;
			if (end_index >= SIZE)
				return;
			block_array[end_index] = new Block(capacity, false);
		}
		block_array[end_index]->push_back(_data);
	}
	void push_front(const T& _data)
	{
		if (begin_index >= 0 && !block_array[begin_index]->push_front_check())
		{
			--begin_index;
			if (begin_index < 0)
				return;
				block_array[begin_index] = new Block(capacity);
		}
		block_array[begin_index]->push_front(_data);
	}

};
int main()
{
	mydeque<int, 4> d;
	d.push_back(1);
	d.push_back(2);
	d.push_back(3);
	d.push_back(4);
	d.push_back(5);
	d.push_back(1);
	d.push_back(1);
	d.push_back(1);
	d.push_back(1);
	d.push_back(1);
	d.push_back(1);
}