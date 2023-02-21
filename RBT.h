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
			//�θ� �Ҿƹ����� �����̸� �������� ����
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
		// �θ��� �θ� ��� 
		Node<Key, Value>* grandparent = parent->parent;

		if (sibling == nilnode)
		{
			sibling = (*_node);
		}
		// ������ �θ� �θ��� �θ�� ����
		sibling->parent = grandparent;
		// ������ ���� ���
		Node<Key, Value>* left_backup = sibling->left;
		// ������ ���ʿ� �θ� �ֱ�
		sibling->left = parent;
		// �θ��� �����ʿ� ���� ��� �ֱ�.
		parent->right = left_backup;
		// ���� ����� �θ� �θ�� �ϱ�.
		left_backup->parent = parent;
		//�θ��� �θ� ������ ����
		parent->parent = sibling;

		// �θ��ڸ��� ���� �ֱ�
		*parent_space = sibling;
	}
	void RightRotation(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		Node<Key, Value>** parent_space;
		serch_node(&root, parent->key, parent_space);
		// �θ��� �θ� ��� 
		Node<Key, Value>* grandparent = parent->parent;

		if (sibling == nilnode)
		{
			sibling = (*_node);
		}
		// ������ �θ� �θ��� �θ�� ����
		sibling->parent = grandparent;
		// ������ ������ ���
		Node<Key, Value>* right_backup = sibling->right;
		// ������ �����ʿ� �θ� �ֱ�
		sibling->right = parent;
		// �θ��� ���ʿ� ������ ��� �ֱ�.
		parent->left = right_backup;
		// ������ ����� �θ� �θ�� �ϱ�.
		right_backup->parent = parent;
		//�θ��� �θ� ������ ����
		parent->parent = sibling;

		// �θ��ڸ��� ���� �ֱ�
		*parent_space = sibling;
	}
	void Children_Alter_Black(Node<Key, Value>* _parent, Node<Key, Value>* _child_1, Node<Key, Value>* _child_2)
	{
		//delete ���� ������ ���� �ڽ��� red�� ��� ���� �ڽ��� black���� ���� �� 
		//������ red�� ���� case 1 ����
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
	//case 1 : �θ�� ������ ������ �� 
	//  => �θ�� ������ black ���� �ٲٰ� root node�� red ���� �˻�, red �� black���� ����.
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
	//case 2 : ���ʿ� push ���� �� �θ��� ������ push �׸��� �θ� ���� , ������ �� 
	//  => �������� ȸ��. 
	void insert_case2(Node<Key, Value>** _node)
	{
		//Node<T>** parent = &(*_node)->parent;
		//���� = black
		//(*_node)->type = NodeType::Black;
		//�θ� ������ ���� �ֱ�
		//������ ���� ��� �ּҴ� �Ű���� �ϱ� ������ ��� ���α�
		//������ left = ���� �θ�
		//���� �θ� right = ��� ���� ��� �ּ�.
		//������ �������� ������ ��带 �������� �ٽ� �˻��ϱ�.
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
	//case 3 : ���ʿ� push ���� �� �θ��� ���� push �׸��� �θ� ���� , ������ ��
	void insert_case3(Node<Key, Value>** _node)
	{
		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		Node<Key, Value>** uncle = GetUncle(_node);


		//�θ� = black �Ҿƹ��� = red �� ����
		(*_node)->parent->type = NodeType::Black;
		(*grand_parent)->type = NodeType::Red;

		//�Ҿƹ����� ��ġ�� �θ� �ְ�, �θ��� �����ʿ� �Ҿƹ����� ���� ��.
		//���������� �۾� ����� �ϴµ� �ϴ� Ʋ�� ¥��.
		Node<Key, Value>* g_p_p = (*grand_parent)->parent;
		Node<Key, Value>* temp_gpp = (*_node)->parent->parent;
		temp_gpp->left = nilnode;
		Node<Key, Value>** serch = nullptr;
		serch_node(&root, temp_gpp->key, serch);
		//�θ� ���� �Ҿƹ����� ����� ����
		(*_node)->parent->parent = g_p_p;
		//�θ� ������ ���� �Ҿƹ��� ���� 
		Node<Key, Value>* right_backup = (*_node)->parent->right;
		temp_gpp->left = right_backup;
		right_backup->parent = temp_gpp;
		(*_node)->parent->right = temp_gpp;
		temp_gpp->parent = (*_node)->parent;

		(*serch)/*�Ҿƹ��� ����*/ = (*_node)->parent;
		RBT_Insert(&(*_node));
	}
	//case 4 : ������ push ���� �� �θ��� ���� push �׸��� �θ� ���� , ������ ��
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
	//case 5 : ������ push ���� �� �θ��� ������ push �׸��� �θ� ����, ������ �� 
	void insert_case5(Node<Key, Value>** _node)
	{
		Node<Key, Value>** grand_parent = GetGrandParent(_node);
		Node<Key, Value>** uncle = GetUncle(_node);


		//�θ� = black �Ҿƹ��� = red �� ����
		(*_node)->parent->type = NodeType::Black;
		(*grand_parent)->type = NodeType::Red;

		//�Ҿƹ����� ��ġ�� �θ� �ְ�, �θ��� �����ʿ� �Ҿƹ����� ���� ��.
		//���������� �۾� ����� �ϴµ� �ϴ� Ʋ�� ¥��.
		Node<Key, Value>* g_p_p = (*grand_parent)->parent;
		Node<Key, Value>* temp_gpp = (*_node)->parent->parent;
		temp_gpp->right = nilnode;
		Node<Key, Value>** serch = nullptr;
		serch_node(&root, temp_gpp->key, serch);
		//�θ� ���� �Ҿƹ����� ����� ����
		(*_node)->parent->parent = g_p_p;
		//�θ� ���� ���� �Ҿƹ��� ���� 
		Node<Key, Value>* left_backup = (*_node)->parent->left;
		temp_gpp->right = left_backup;
		left_backup->parent = temp_gpp;
		(*_node)->parent->left = temp_gpp;
		temp_gpp->parent = (*_node)->parent;
		(*serch)/*�Ҿƹ��� ����*/ = (*_node)->parent;
		RBT_Insert(&(*_node));
	}

	void RBT_Insert(Node<Key, Value>** _node)
	{

		if ((*_node) == nilnode)
		{
			return;

		}//root node �� ���
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
			//���� �θ� push
			if ((*_node)->parent == (*grand_parent)->left)
			{
				//�θ��� �����ʿ� push
				if ((*_node)->parent->right == (*_node))
					insert_case2(_node);
				//�θ��� ���ʿ� push
				else
					insert_case3(_node);
			}
			//������ �θ� push
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
	/* case 4 ����
	:  doubly black (_node) �� ���� ��尡 Black �� ��
	   �� ������ ������ �ڳడ Red �̴�.

	   �ش� (extra node �� ���� ����)
	:  ������ ���� = �θ� ��
	   ������ ������ �ڳ�(Red) = Black
	   �θ� = Black
	   ������ �� : ���� ȸ�� (�θ� ����)


		�ش� (extra node �� ������ ����)
	:  ���� ���� = �θ� ��
	   ������ ���� �ڳ�(Red) = Black
	   �θ� = Black
	   ��������  ��  : ������ ȸ��(�θ����)
	*/
	//case 4 - r
	void delete_case4_r(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = parent->type;
		//���� �θ��� ������ ����
		sibling->type = tempcolor;
		//���� �ڳ� Red -> Black
		sibling->left->type = NodeType::Black;
		//�θ� Black
		parent->type = NodeType::Black;
		//������ ȸ��
		RightRotation(_node, parent, sibling);
	}
	//case 4 - l
	void delete_case4_l(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = parent->type;
		//���� �θ��� ������ ����
		sibling->type = tempcolor;
		//������ �ڳ� Red -> Black
		sibling->right->type = NodeType::Black;
		//�θ� Black
		parent->type = NodeType::Black;
		//���� ȸ��
		LeftRotation(_node, parent, sibling);
	}
#pragma endregion
#pragma region case 3
	/* case 3 ����
	:  doubly black (_node) �� ���� ��尡 Black �� ��
	   �� ������ ���� �ڳడ Red �̴�.
	   �ش�
	:  ������ red�ڽ��� ���� �ٲ۴�.
	   ���� �������� ������ ���� ���鸸 ����/�� ������ ȸ��.
	   case 4 ����.
	*/
	//case 3 - r
	void delete_case3_r(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = sibling->type;
		//������ ������ ������(Red)�� ���� �ٲ۴�.
		sibling->type = sibling->right->type;
		sibling->right->type = tempcolor;
		//���� ���� ���� ȸ��.
		Node<Key, Value>** sibling_right_child = &((*_node)->parent->left->right);
		LeftRotation(sibling_right_child, sibling, sibling->left);

		//case 4
		delete_case4_r(_node, parent, sibling->parent);
	}
	//case 3 - l
	void delete_case3_l(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		NodeType tempcolor = sibling->type;
		//������ ������ ����(Red)�� ���� �ٲ۴�.
		sibling->type = sibling->left->type;
		sibling->left->type = tempcolor;
		//���� ���� ������ ȸ��.
		Node<Key, Value>** sibling_left_child = &((*_node)->parent->right->left);
		RightRotation(sibling_left_child, sibling, sibling->right);
		//case 4
		delete_case4_l(_node, parent, sibling->parent);
	}
#pragma endregion
#pragma region case 2
	/* case 2 ����
	:  doubly black (_node)�� ���� ��尡 Black �� ��
	   ������ �ڳ�� ��� Black �̴�.

	   �ش�
	:  doubly black �� ���� ����� black �� ��Ƽ� �θ𿡰� ����.
	   �θ�� extra node ���°� �ȴ�.
	   �̶� �θ� red �̸� red-and-black ����
			�θ� black�̸� doubly black ���� �̴�.

	   �θ� root �̸� black ó���ϰ� ������
	   red-and-black �̸� black
	   doubly black �̸� �ٽ� case������ ���� �Ѵ�. RBT_Delete() ȣ��.
	*/
	//case 2
	void delete_case2(Node<Key, Value>** _node, Node<Key, Value>* parent, Node<Key, Value>* sibling)
	{
		//extra node �� extra ���¸� ���� �Ѵ�.
		(*_node)->is_extra = false;
		//���� ���(����)�� red �� ����
		sibling->type = NodeType::Red;
		//�θ��� ���¿� extra ���� �߰�.
		parent->is_extra = true;

		if (parent->type == NodeType::Red || parent->parent == nullptr)
		{
			parent->type = NodeType::Black;
			parent->is_extra = false;
		}
		//�θ� doubly black �� �� 
		else
		{
			RBT_Delete(&(*_node)->parent);
		}
	}
#pragma endregion
#pragma region case 1
	/* case 1 ����
	:  doubly black �� ������ Red �� ��

	   �ش�
	:  ������ Black ���� ����� ȸ�� ��  RBT_Delete().
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

		// ���� ��尡 �θ���� �����̴�.
		if (parent->left == (*_node))
		{
			sibling = parent->right;
			if (sibling->type == NodeType::Black)
			{
				//�ڽ� �Ѵ� Red
				if (sibling->left->type == NodeType::Red && sibling->right->type == NodeType::Red)
				{
					Children_Alter_Black(parent, sibling->left, sibling->right);
					delete_case1_l(_node, parent, sibling);
				}
				//�����ʸ� Red
				else if (sibling->right->type == NodeType::Red)
				{
					delete_case4_l(_node, parent, sibling);
				}
				//���ʸ� Red
				else if (sibling->left->type == NodeType::Red)
				{
					delete_case3_l(_node, parent, sibling);
				}
				/*
				  ����: ����
				  ������ �ڽ�1 : ����
				  ������ �ڽ� 2 : ����
				*/
				else
				{
					delete_case2(_node, parent, sibling);
				}
			}
			else // ������ red �� �� case 1 
			{
				delete_case1_l(_node, parent, sibling);
			}
		}
		else
		{
			sibling = parent->left;
			if (sibling->type == NodeType::Black)
			{
				//�ڽ� �Ѵ� Red
				if (sibling->left->type == NodeType::Red && sibling->right->type == NodeType::Red)
				{
					Children_Alter_Black(parent, sibling->left, sibling->right);
					delete_case1_r(_node, parent, sibling);
				}
				//���ʸ� Red
				else if (sibling->left->type == NodeType::Red)
				{
					delete_case4_r(_node, parent, sibling);
				}
				//�����ʸ� Red
				else if (sibling->right->type == NodeType::Red)
				{
					delete_case3_r(_node, parent, sibling);
				}
				/*
				  ����: ����
				  ������ �ڽ�1 : ����
				  ������ �ڽ� 2 : ����
				*/
				else
				{
					delete_case2(_node, parent, sibling);
				}
			}
			else // ������ red �� �� case 1 
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
		// ��尡 nil �̸� �߰��� node �ֱ�.
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
		//�ڽ��� ���� ���
		if (ptr->right == nilnode && ptr->left == nilnode)
		{
			_delete_color = ptr->type;
			(*_root) = nilnode;
			(*_root)->is_extra = true; // ������ ���� ��带 ��ü�� ��忡 extra �ο� 
			extranode = (*_root);
		}//�ϳ��� �ִ� ���
		else if (ptr->right == nilnode)
		{
			_delete_color = ptr->type;
			(*_root) = ptr->left;
			(*_root)->is_extra = true; // ������ ���� ��带 ��ü�� ��忡 extra �ο� 
			extranode = (*_root);
		}
		else if (ptr->left == nilnode)
		{
			_delete_color = ptr->type;
			(*_root) = ptr->right;
			(*_root)->is_extra = true; // ������ ���� ��带 ��ü�� ��忡 extra �ο� 
			extranode = (*_root);
		}//�Ѵ� �ִ� ���
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
			//������ �� : �����Ǵ� ����� successor ��� ��.
			_delete_color = leftest->type;
			//����� �� : �����Ǵ� ����� ��
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
			//��ü�� successor ��忡 ������ ����� ������ ����.
			(*_root)->type = use_color;
		}
		//��ü�� successor ����� �θ� ����.
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
		//���� �Ӽ��� �ִ��� üũ
		if (delete_color == NodeType::Black)
		{
			//root ��尡 ������ ���� ��.
			if (extranode == nilnode && extranode->parent == nullptr)
			{
				delete ptr;
				return;
			}
			//���� ����(����)�� ��ġ�� root �� �� ��ü�� successor ����� ���� black���� ����.
			else if ((*_root)->parent == nullptr)
			{
				(*_root)->type = NodeType::Black;

				//������ ���� ��ġ�� ��ü�� ��尡 root �̸� extra ���¸� ����.
				if (extranode == (*_root))
				{
					(*_root)->is_extra = false;
					return;
				}
			}

			//extra�� ���� ����� ���� red �̸� black ���� ����
			if (extranode->type == NodeType::Red)
			{
				extranode->type = NodeType::Black;
				return;
			}

			//extra ��� �������� �Ӽ��� �����ϰ� ��� ���ġ.
			//������ ����� �ڽ��� 0-1 �̸� ������ ��ġ�� extra ������
			//������ ����� �ڽ��� 2 �̸� successor ��ġ�� extra �̴�.
			Node<Key, Value>** extra_space;
			serch_node(_root, extranode->key, extra_space);
			RBT_Delete(extra_space);
		}
		//������ ������ Red �̸� extra �Ӽ� X
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
			printf("���� key ������ map ����.");
			//����ó��! key���� ã�� ����!
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
