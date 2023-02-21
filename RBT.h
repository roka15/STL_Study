#pragma once
#include <iostream>
enum class NodeType
{
	None,
	Black,
	Red
};

template<typename Key, typename Value>
class Node
{
public:
	Node() :parent(nullptr), left(nullptr), right(nullptr), type(NodeType::None), data(-1), is_extra(false), key(-1)
	{

	}
	Node(NodeType _type) :parent(nullptr), left(nullptr), right(nullptr), type(_type), data(-1), is_extra(false), key(-1)
	{

	}
	Node(Key _key, Value _data, Node<Key, Value>* nilnode, NodeType _type) :parent(nullptr), left(nilnode), right(nilnode), data(_data), type(_type), is_extra(false), key(_key)
	{
	}
	~Node()
	{

	}
	Node* parent;
	Node* left;
	Node* right;
	Key key;
	Value data;
	NodeType type;
	bool is_extra;
};
template <typename Key, typename Value>
class Node<Key, Value*>
{
public:
	Node() :parent(nullptr), left(nullptr), right(nullptr), type(NodeType::None), data(nullptr), is_extra(false), key(-1)
	{

	}
	Node(NodeType _type) :parent(nullptr), type(_type), data(nullptr), is_extra(false), key(-1)
	{

	}
	Node(Key _key, Value* _data, Node<Key, Value*>* nilnode, NodeType _type) :parent(nullptr), left(nilnode), right(nilnode), data(_data), type(_type), is_extra(false), key(_key)
	{
	}
	~Node()
	{
		delete data;
	}
	Node* parent;
	Node* left;
	Node* right;
	Key key;
	Value* data;
	NodeType type;
	bool is_extra;
};
template <typename Key, typename Value>
class RBT
{
private:
	Node<Key, Value>* root;
	int count;
	static Node<Key, Value>* nilnode;
	//static Node<T>* extranode;
private:
#pragma region red_black_tree func
private:
	bool serch_node(Node<Key, Value>** _root, Key _key, Node<Key, Value>**& serchnode)
	{
		Node<Key, Value>* ptr = *_root;
		if (*_root == nullptr)
		{
			return false;
		}
		if (*_root == nilnode)
		{
			serchnode = _root;
			return false;
		}

		if (ptr->key == _key)
		{
			serchnode = _root;
			return true;
		}
		else if (ptr->key > _key)
		{
			return serch_node(&((*_root)->left), _key, serchnode);
		}
		else if (ptr->key < _key)
		{
			return serch_node(&((*_root)->right), _key, serchnode);
		}
		return false;
	}
	Node<Key, Value>** GetGrandParent(Node<Key, Value>** _node)
	{
		if ((*_node) != nilnode)
		{
			return &((*_node)->parent->parent);
		}
		else
		{
			return nullptr;
		}
	}
	Node<Key, Value>** GetUncle(Node<Key, Value>** _node)
	{
		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		if ((*grand_parent) == nullptr)
		{
			return nullptr;
		}
		if ((*_node) != nilnode)
		{
			//부모가 할아버지의 왼쪽이면 오른쪽이 삼촌
			if ((*_node)->parent == (*grand_parent)->left)
			{
				return &(*grand_parent)->right;
			}
			else
			{
				return &(*grand_parent)->left;
			}
		}
		return &nilnode;
	}
	void LeftRotation(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		Node<Key, Value>** parent_space;
		serch_node(&root, parent->key, parent_space);
		// 부모의 부모 백업 
		Node<Key, Value>* grandparent = parent->parent;

		if (sibling == nilnode)
		{
			sibling = (*_node);
		}
		// 형제의 부모를 부모의 부모로 설정
		sibling->parent = grandparent;
		// 형제의 왼쪽 백업
		Node<Key, Value>* left_backup = sibling->left;
		// 형제의 왼쪽에 부모 넣기
		sibling->left = parent;
		// 부모의 오른쪽에 왼쪽 백업 넣기.
		parent->right = left_backup;
		// 왼쪽 백업의 부모를 부모로 하기.
		left_backup->parent = parent;
		//부모의 부모를 형제로 설정
		parent->parent = sibling;

		// 부모자리에 형제 넣기
		*parent_space = sibling;
	}
	void RightRotation(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		Node<Key, Value>** parent_space;
		serch_node(&root, parent->key, parent_space);
		// 부모의 부모 백업 
		Node<Key, Value>* grandparent = parent->parent;

		if (sibling == nilnode)
		{
			sibling = (*_node);
		}
		// 형제의 부모를 부모의 부모로 설정
		sibling->parent = grandparent;
		// 형제의 오른쪽 백업
		Node<Key, Value>* right_backup = sibling->right;
		// 형제의 오른쪽에 부모 넣기
		sibling->right = parent;
		// 부모의 왼쪽에 오른쪽 백업 넣기.
		parent->left = right_backup;
		// 오른쪽 백업의 부모를 부모로 하기.
		right_backup->parent = parent;
		//부모의 부모를 형제로 설정
		parent->parent = sibling;

		// 부모자리에 형제 넣기
		*parent_space = sibling;
	}
	void Children_Alter_Black(Node<Key, Value>* _parent, Node<Key, Value>* _child_1, Node<Key, Value>* _child_2)
	{
		//delete 에서 형제의 양쪽 자식이 red인 경우 양쪽 자식을 black으로 만든 후 
		//형제를 red로 만들어서 case 1 수행
		if (_child_1->type != _child_2->type && _child_1->type != NodeType::Red)
		{
			return;
		}

		NodeType tempcolor = _parent->type;

		_parent->type = _child_1->type;

		_child_1->type = tempcolor;
		_child_2->type = tempcolor;
	}
#pragma region insert func
	//case 1 : 부모와 삼촌이 레드일 때 
	//  => 부모와 삼촌을 black 으로 바꾸고 root node가 red 인지 검사, red 면 black으로 변경.
	void insert_case1(Node<Key, Value>** _node)
	{
		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		Node<Key, Value>** uncle = GetUncle(_node);
		if ((*grand_parent) == nullptr)
		{
			return;
		}

		(*_node)->parent->type = NodeType::Black;
		(*uncle)->type = NodeType::Black;
		(*grand_parent)->type = NodeType::Red;

		RBT_Insert(&(*grand_parent));
	}
	//case 2 : 왼쪽에 push 됐을 때 부모의 오른쪽 push 그리고 부모가 레드 , 삼촌은 블랙 
	//  => 왼쪽으로 회전. 
	void insert_case2(Node<Key, Value>** _node)
	{
		//Node<T>** parent = &(*_node)->parent;
		//본인 = black
		//(*_node)->type = NodeType::Black;
		//부모 공간에 본인 넣기
		//본인의 왼쪽 노드 주소는 옮겨줘야 하기 때문에 잠시 빼두기
		//본인의 left = 이전 부모
		//이전 부모 right = 잠깐 빼둔 노드 주소.
		//본인의 왼쪽으로 설정된 노드를 기준으로 다시 검색하기.
		Node<Key, Value>* curnode = (*_node);
		Node<Key, Value>** serch = nullptr;
		Node<Key, Value>* left_backup = curnode->left;
		curnode->left = nullptr;
		left_backup->parent = nullptr;
		serch_node(&root, curnode->parent->key, serch);
		Node<Key, Value>* temp = *serch;

		(*serch) = curnode;

		curnode->left = temp;
		curnode->parent = temp->parent;

		curnode->left->right = left_backup;
		curnode->left->parent = curnode;
		left_backup->parent = temp;


		RBT_Insert(&(*serch)->left);
	}
	//case 3 : 왼쪽에 push 됐을 때 부모의 왼쪽 push 그리고 부모가 레드 , 삼촌은 블랙
	void insert_case3(Node<Key, Value>** _node)
	{
		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		Node<Key, Value>** uncle = GetUncle(_node);


		//부모 = black 할아버지 = red 로 변경
		(*_node)->parent->type = NodeType::Black;
		(*grand_parent)->type = NodeType::Red;

		//할아버지의 위치에 부모를 넣고, 부모의 오른쪽에 할아버지가 들어가게 함.
		//이중포인터 작업 해줘야 하는데 일단 틀만 짜기.
		Node<Key, Value>* g_p_p = (*grand_parent)->parent;
		Node<Key, Value>* temp_gpp = (*_node)->parent->parent;
		temp_gpp->left = nilnode;
		Node<Key, Value>** serch = nullptr;
		serch_node(&root, temp_gpp->key, serch);
		//부모에 기존 할아버지의 조상과 연결
		(*_node)->parent->parent = g_p_p;
		//부모에 오른쪽 기존 할아버지 연결 
		Node<Key, Value>* right_backup = (*_node)->parent->right;
		temp_gpp->left = right_backup;
		right_backup->parent = temp_gpp;
		(*_node)->parent->right = temp_gpp;
		temp_gpp->parent = (*_node)->parent;

		(*serch)/*할아버지 공간*/ = (*_node)->parent;
		RBT_Insert(&(*_node));
	}
	//case 4 : 오른쪽 push 됐을 때 부모의 왼쪽 push 그리고 부모가 레드 , 삼촌은 블랙
	void insert_case4(Node<Key, Value>** _node)
	{
		Node<Key, Value>* curnode = (*_node);
		Node<Key, Value>** serch = nullptr;
		Node<Key, Value>* right_backup = curnode->right;
		curnode->right = nullptr;
		right_backup->parent = nullptr;
		serch_node(&root, curnode->parent->key, serch);
		Node<Key, Value>* temp = *serch;

		(*serch) = curnode;

		curnode->right = temp;
		curnode->parent = temp->parent;

		curnode->right->left = right_backup;
		curnode->right->parent = curnode;
		right_backup->parent = temp;


		RBT_Insert(&(*serch)->left);
	}
	//case 5 : 오른쪽 push 됐을 때 부모의 오른쪽 push 그리고 부모가 레드, 삼촌은 블랙 
	void insert_case5(Node<Key, Value>** _node)
	{
		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		Node<Key, Value>** uncle = GetUncle(_node);


		//부모 = black 할아버지 = red 로 변경
		(*_node)->parent->type = NodeType::Black;
		(*grand_parent)->type = NodeType::Red;

		//할아버지의 위치에 부모를 넣고, 부모의 오른쪽에 할아버지가 들어가게 함.
		//이중포인터 작업 해줘야 하는데 일단 틀만 짜기.
		Node<Key, Value>* g_p_p = (*grand_parent)->parent;
		Node<Key, Value>* temp_gpp = (*_node)->parent->parent;
		temp_gpp->right = nilnode;
		Node<Key, Value>** serch = nullptr;
		serch_node(&root, temp_gpp->key, serch);
		//부모에 기존 할아버지의 조상과 연결
		(*_node)->parent->parent = g_p_p;
		//부모에 왼쪽 기존 할아버지 연결 
		Node<Key, Value>* left_backup = (*_node)->parent->left;
		temp_gpp->right = left_backup;
		left_backup->parent = temp_gpp;
		(*_node)->parent->left = temp_gpp;
		temp_gpp->parent = (*_node)->parent;
		(*serch)/*할아버지 공간*/ = (*_node)->parent;
		RBT_Insert(&(*_node));
	}

	void RBT_Insert(Node<Key, Value>** _node)
	{

		if ((*_node) == nilnode)
		{
			return;

		}//root node 일 경우
		else if ((*_node)->parent == nullptr)
		{
			(*_node)->type = NodeType::Black;
			return;
		}

		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		Node<Key, Value>** uncle = GetUncle(_node);

		if ((uncle != nullptr) && (*_node)->parent->type == NodeType::Red && (*uncle)->type == NodeType::Red)
		{
			insert_case1(_node);
		}
		else if ((uncle != nullptr) && (*_node)->parent->type == NodeType::Red && (*uncle)->type == NodeType::Black)
		{
			//왼쪽 부모에 push
			if ((*_node)->parent == (*grand_parent)->left)
			{
				//부모의 오른쪽에 push
				if ((*_node)->parent->right == (*_node))
					insert_case2(_node);
				//부모의 왼쪽에 push
				else
					insert_case3(_node);
			}
			//오른쪽 부모에 push
			else
			{
				if ((*_node)->parent->left == (*_node))
					insert_case4(_node);
				else
					insert_case5(_node);
			}
		}
	}
#pragma endregion
#pragma region delete func
#pragma region case 4
	/* case 4 설명
	:  doubly black (_node) 의 형제 노드가 Black 일 때
	   그 형제의 오른쪽 자녀가 Red 이다.

	   해답 (extra node 가 왼쪽 기준)
	:  오른쪽 형제 = 부모 색
	   형제의 오른쪽 자녀(Red) = Black
	   부모 = Black
	   왼쪽일 시 : 왼쪽 회전 (부모 기준)


		해답 (extra node 가 오른쪽 기준)
	:  왼쪽 형제 = 부모 색
	   형제의 왼쪽 자녀(Red) = Black
	   부모 = Black
	   오른쪽일  시  : 오른쪽 회전(부모기준)
	*/
	//case 4 - r
	void delete_case4_r(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = parent->type;
		//형제 부모의 색으로 변경
		sibling->type = tempcolor;
		//왼쪽 자녀 Red -> Black
		sibling->left->type = NodeType::Black;
		//부모 Black
		parent->type = NodeType::Black;
		//오른쪽 회전
		RightRotation(_node, parent, sibling);
	}
	//case 4 - l
	void delete_case4_l(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = parent->type;
		//형제 부모의 색으로 변경
		sibling->type = tempcolor;
		//오른쪽 자녀 Red -> Black
		sibling->right->type = NodeType::Black;
		//부모 Black
		parent->type = NodeType::Black;
		//왼쪽 회전
		LeftRotation(_node, parent, sibling);
	}
#pragma endregion
#pragma region case 3
	/* case 3 설명
	:  doubly black (_node) 의 형제 노드가 Black 일 때
	   그 형제의 왼쪽 자녀가 Red 이다.
	   해답
	:  형제와 red자식의 색을 바꾼다.
	   형제 기준으로 형제에 속한 노드들만 오른/왼 쪽으로 회전.
	   case 4 수행.
	*/
	//case 3 - r
	void delete_case3_r(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = sibling->type;
		//형제와 형제의 오른쪽(Red)의 색과 바꾼다.
		sibling->type = sibling->right->type;
		sibling->right->type = tempcolor;
		//형제 기준 왼쪽 회전.
		Node<Key, Value>** sibling_right_child = &((*_node)->parent->left->right);
		LeftRotation(sibling_right_child, sibling, sibling->left);

		//case 4
		delete_case4_r(_node, parent, sibling->parent);
	}
	//case 3 - l
	void delete_case3_l(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = sibling->type;
		//형제와 형제의 왼쪽(Red)의 색과 바꾼다.
		sibling->type = sibling->left->type;
		sibling->left->type = tempcolor;
		//형제 기준 오른쪽 회전.
		Node<Key, Value>** sibling_left_child = &((*_node)->parent->right->left);
		RightRotation(sibling_left_child, sibling, sibling->right);
		//case 4
		delete_case4_l(_node, parent, sibling->parent);
	}
#pragma endregion
#pragma region case 2
	/* case 2 설명
	:  doubly black (_node)의 형제 노드가 Black 일 때
	   형제의 자녀들 모두 Black 이다.

	   해답
	:  doubly black 과 형제 노드의 black 을 모아서 부모에게 전달.
	   부모는 extra node 상태가 된다.
	   이때 부모가 red 이면 red-and-black 상태
			부모가 black이면 doubly black 상태 이다.

	   부모가 root 이면 black 처리하고 나가고
	   red-and-black 이면 black
	   doubly black 이면 다시 case문들을 수행 한다. RBT_Delete() 호출.
	*/
	//case 2
	void delete_case2(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		//extra node 의 extra 상태를 해제 한다.
		(*_node)->is_extra = false;
		//형제 노드(검정)을 red 로 변경
		sibling->type = NodeType::Red;
		//부모의 상태에 extra 상태 추가.
		parent->is_extra = true;

		if (parent->type == NodeType::Red || parent->parent == nullptr)
		{
			parent->type = NodeType::Black;
			parent->is_extra = false;
		}
		//부모가 doubly black 일 때 
		else
		{
			RBT_Delete(&(*_node)->parent);
		}
	}
#pragma endregion
#pragma region case 1
	/* case 1 설명
	:  doubly black 의 형제가 Red 일 때

	   해답
	:  형제를 Black 으로 만들고 회전 후  RBT_Delete().
	*/
	//case 1 - r
	void delete_case1_r(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = parent->type;
		parent->type = sibling->type;

		Node<Key, Value>** parent_space = &(*_node)->parent;

		RightRotation(_node, parent, sibling);

		RBT_Delete(_node);
	}
	//case 1 - l
	void delete_case1_l(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = parent->type;
		parent->type = sibling->type;

		Node<Key, Value>** parent_space = &(*_node)->parent;

		LeftRotation(_node, parent, sibling);

		RBT_Delete(_node);
	}
#pragma endregion
	void RBT_Delete(Node<Key, Value>** _node)
	{
		Node<Key, Value>* parent = (*_node)->parent;
		Node<Key, Value>* sibling = nullptr;

		// 현재 노드가 부모기준 왼쪽이다.
		if (parent->left == (*_node))
		{
			sibling = parent->right;
			if (sibling->type == NodeType::Black)
			{
				//자식 둘다 Red
				if (sibling->left->type == NodeType::Red && sibling->right->type == NodeType::Red)
				{
					Children_Alter_Black(parent, sibling->left, sibling->right);
					delete_case1_l(_node, parent, sibling);
				}
				//오른쪽만 Red
				else if (sibling->right->type == NodeType::Red)
				{
					delete_case4_l(_node, parent, sibling);
				}
				//왼쪽만 Red
				else if (sibling->left->type == NodeType::Red)
				{
					delete_case3_l(_node, parent, sibling);
				}
				/*
				  형제: 검정
				  형제의 자식1 : 검정
				  형제의 자식 2 : 검정
				*/
				else
				{
					delete_case2(_node, parent, sibling);
				}
			}
			else // 형제가 red 일 때 case 1 
			{
				delete_case1_l(_node, parent, sibling);
			}
		}
		else
		{
			sibling = parent->left;
			if (sibling->type == NodeType::Black)
			{
				//자식 둘다 Red
				if (sibling->left->type == NodeType::Red && sibling->right->type == NodeType::Red)
				{
					Children_Alter_Black(parent, sibling->left, sibling->right);
					delete_case1_r(_node, parent, sibling);
				}
				//왼쪽만 Red
				else if (sibling->left->type == NodeType::Red)
				{
					delete_case4_r(_node, parent, sibling);
				}
				//오른쪽만 Red
				else if (sibling->right->type == NodeType::Red)
				{
					delete_case3_r(_node, parent, sibling);
				}
				/*
				  형제: 검정
				  형제의 자식1 : 검정
				  형제의 자식 2 : 검정
				*/
				else
				{
					delete_case2(_node, parent, sibling);
				}
			}
			else // 형제가 red 일 때 case 1 
			{
				delete_case1_r(_node, parent, sibling);
			}
		}
	}
#pragma endregion
#pragma endregion
#pragma region BST func
	void push_node(Node<Key, Value>** _root, Node<Key, Value>* _node)
	{
		Node<Key, Value>* ptr = (*_root);
		// 노드가 nil 이면 추가할 node 넣기.
		if (*_root == nilnode||*_root==nullptr)
		{
			*_root = _node;
		}
		else if (ptr->key > _node->key)
		{
			_node->parent = (*_root);
			push_node(&((*_root)->left), _node);
		}
		else if (ptr->key < _node->key)
		{
			_node->parent = (*_root);
			push_node(&((*_root)->right), _node);
		}
	}
	Node<Key, Value>* child_up(Node<Key, Value>** _root, NodeType& _delete_color)
	{
		Node<Key, Value>* ptr = (*_root);
		Node<Key, Value>* parent_ptr = (*_root)->parent;
		Node<Key, Value>* extranode = nullptr;
		if (*_root == nilnode)
		{
			return nilnode;
		}
		//자식이 없는 경우
		if (ptr->right == nilnode && ptr->left == nilnode)
		{
			_delete_color = ptr->type;
			(*_root) = nilnode;
			(*_root)->is_extra = true; // 삭제된 색의 노드를 대체한 노드에 extra 부여 
			extranode = (*_root);
		}//하나만 있는 경우
		else if (ptr->right == nilnode)
		{
			_delete_color = ptr->type;
			(*_root) = ptr->left;
			(*_root)->is_extra = true; // 삭제된 색의 노드를 대체한 노드에 extra 부여 
			extranode = (*_root);
		}
		else if (ptr->left == nilnode)
		{
			_delete_color = ptr->type;
			(*_root) = ptr->right;
			(*_root)->is_extra = true; // 삭제된 색의 노드를 대체한 노드에 extra 부여 
			extranode = (*_root);
		}//둘다 있는 경우
		else
		{
			NodeType use_color = NodeType::None;
			Node<Key, Value>* root_left;
			Node<Key, Value>* root_right;
			Node<Key, Value>* temp = (*_root);
			Node<Key, Value>* leftest = ptr->right;
			while (leftest->left != nilnode)
			{
				temp = leftest;
				leftest = leftest->left;
			}
			//삭제할 색 : 삭제되는 노드의 successor 노드 색.
			_delete_color = leftest->type;
			//사용할 색 : 삭제되는 노드의 색
			use_color = (*_root)->type;

			if (temp->left == leftest)
			{
				root_left = ptr->left;
				root_right = ptr->right;
				(*_root) = leftest;

				NodeType dummy_color = NodeType::None;
				extranode = child_up(&(leftest), dummy_color);

				temp->left = leftest;
				leftest->parent = temp;

				root_left->parent = (*_root);
				root_right->parent = (*_root);
				(*_root)->left = root_left;
				(*_root)->right = root_right;
			}
			else
			{
				Node<Key, Value>** tempnode = _root;
				root_left = temp->left;
				(*tempnode) = leftest;
				(*tempnode)->left = root_left;
				(*tempnode)->right = leftest->right;
				(*tempnode)->right->is_extra = true;

				extranode = (*tempnode)->right;

				root_left->parent = (*tempnode);
			}
			//교체된 successor 노드에 색깔을 사용할 색으로 변경.
			(*_root)->type = use_color;
		}
		//교체된 successor 노드의 부모 설정.
		(*_root)->parent = parent_ptr;

		return extranode;
	}
	void pop_node(Node<Key, Value>** _root)
	{
		if (_root == nullptr)
			return;
		Node<Key, Value>* ptr = *_root;
		NodeType delete_color = NodeType::None;
		Node<Key, Value>* extranode = nullptr;
		extranode = child_up(_root, delete_color);
		//위반 속성이 있는지 체크
		if (delete_color == NodeType::Black)
		{
			//root 노드가 삭제된 것일 때.
			if (extranode == nilnode && extranode->parent == nullptr)
			{
				delete ptr;
				return;
			}
			//값이 변경(삭제)된 위치가 root 일 때 교체된 successor 노드의 색을 black으로 변경.
			else if ((*_root)->parent == nullptr)
			{
				(*_root)->type = NodeType::Black;

				//삭제된 색의 위치를 대체한 노드가 root 이면 extra 상태를 끈다.
				if (extranode == (*_root))
				{
					(*_root)->is_extra = false;
					return;
				}
			}

			//extra가 붙은 노드의 색이 red 이면 black 으로 변경
			if (extranode->type == NodeType::Red)
			{
				extranode->type = NodeType::Black;
				return;
			}

			//extra 노드 기준으로 속성에 부합하게 노드 재배치.
			//삭제한 노드의 자식이 0-1 이면 본인의 위치가 extra 이지만
			//삭제한 노드의 자식이 2 이면 successor 위치가 extra 이다.
			Node<Key, Value>** extra_space;
			serch_node(_root, extranode->key, extra_space);
			RBT_Delete(extra_space);
		}
		//삭제될 색상이 Red 이면 extra 속성 X
		else
		{
			extranode->is_extra = false;
		}
		delete ptr;

		nilnode->parent = nullptr;
		nilnode->is_extra = false;
	}

#pragma endregion
public:
	RBT() :root(nilnode),count(0)
	{

	}
	void Push(Key _key, Value  _value)
	{
		Node<Key, Value>* node = new Node<Key, Value>(_key, _value, nilnode, NodeType::Red);
		push_node(&root, node);
		RBT_Insert(&node);
		count++;
	}
	bool Pop(Key _key)
	{
		Node<Key, Value>** serchnode;
		if (serch_node(&root, _key, serchnode))
		{
			pop_node(serchnode);
			count--;
			return true;
		}
		return false;
	}
	bool Find(Key _key, Value& _value)
	{
		Node<Key, Value> dummy;
		Node<Key, Value>** serchnode;
		if (serch_node(&root, _key, serchnode))
		{
			_value = (*serchnode)->data;
			return true;
		}
		return false;
	}
	int Size()
	{
		return count;
	}
	Value& operator[](Key _key)
	{
		Value value;
		bool flag = Find(_key, value);
		if (flag)
		{
			return value;
		}
		else
		{
			printf("없는 key 값으로 map 접근.");
			//에러처리! key값을 찾지 못함!
			int* test = nullptr;

			test[100] = 100;

		}
	}
	void Clear()
	{
		delete root;
	}
};
template<typename Key, typename Value>
Node<Key, Value>* RBT<Key, Value>::nilnode = new Node<Key, Value>(NodeType::Black);
//template<typename T>
//Node<T>* RBT<T>::extranode = new Node<T>(NodeType::Black);
