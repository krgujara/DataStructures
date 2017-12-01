// CSE674 HW7
// Due : Tuesday 12 / 5 at 11 : 59

#include <iostream>
#include <vector>
	using namespace std;


class node {
	//for simplicity, we again assume all numbers are distinct
public:
	int num_values;
	vector<int> value;  //it can contain 1 ... 3 values
	node * parent;
	vector<node *> child;  //child[i] ( i = 0 ... 3) is a pointer to child node i
	int child_state; //a node can have up to 4 child nodes: child 0, 1, 2, 3.  child_state i (0 ... 3) means this node is child i of its parent
	int is_leaf; //1 if this is a leaf node; otherwise 0
	node() { num_values = 0; is_leaf = 1; child.assign(4, nullptr); parent = nullptr; value.assign(3, -999); }
	void add_value(int k);  //add value to a node of less than 3 value
	void remove_value(int k); //remove value from a node of more than 1 value

	int find_item(int k);
    bool is_full();
	int remove_item();  // removes the largest item, called by the break_3_value_node
	node* disconnect_child(int child_num); // called by the break_3_value_node
	void connect_child(int child_num, node* child); // connect child to appropriate parent, called by break_3_value
	int insert_item(int item); // insert the item in the parent, called by break_3_value_node
							// exactly same as add_value function, except that insert_item returns the index at which the item is inserted

};


class two34_tree {
public:
	node * root;
	two34_tree() { root = nullptr; }
	void add(int k);
	node * find(int k); //find a node to add value k; invoked by add
	void break_3_value_node(node * p); //to be invoked by find
	void remove(int k);
	node * find_1(int k); //find a node to replace k once k is removed; invokde by remove
	void expand_1_value_node(node *p); //to be invoked by find_1
	void fusion(node *p); //to be invoked by exapnd_1_value_node
	void rotation(node *p); ////to be invoked by exapnd_1_value_node
	void in_order_traversal(node* p);
	node* get_next_child(node* curr_node, int val);
	
};

void two34_tree::in_order_traversal(node * p) {
	cout << endl;
	node * p1 = p;
	if (p1 == nullptr) return;
	int i;
	for (i = 0; i < p1->num_values; i++) {
		in_order_traversal(p1->child[i]);
		cout << "  " << p1->value[i] << "  " << "child_state = " << p1->child_state;
	}
	in_order_traversal(p1->child[i]);
}

void node::add_value(int k)
{
	// the node to which the value is being added currently
	// is definitely a leaf node, which is not full definitely.
	// this care has been taken in the add function of 234tree class
	num_values++;
	// start from right, examine items
	for (int i = num_values - 1; i >= 0; i--)
	{
		if (value[i] == -999)
		{
			continue;
		}
		else
		{
			// if new item is less, shift existing to right
			if (k < value[i])
			{
				value[i++] = value[i]; // shift it to right
			}
			else
			{
				// insert new item 
				value[i + 1] = k;
				return;
			}
		}
	}
	// shifted all items, but still new item not inserted,
	// it means, that the new item has to be inserted to first place
	value[0] = k;
}
void node::remove_value(int k)
{

}
int node::find_item(int key)
{
	// -1 if the item is not found
	for (int i = 0; i < num_values; i++)
	{
		if (value[i] == -999)
			break;
		else if (value[i] == key)
			return i;
	}
	return -1;
}

bool node::is_full()
{
	return (num_values == 3);
}

// insert the data item
void two34_tree::add(int k)
{
	// perform the splits
	// top down fassion (root to leaf)
	node* cur_node = root;
	while (true)
	{
		// if node is full
		if (cur_node->is_full())
		{
			// split it back
			break_3_value_node(cur_node);
			cur_node = cur_node->parent;
			// search once
			cur_node = get_next_child(cur_node, k);
		}
		// end if node is full
		else if (cur_node->is_leaf)
		{
			break;
		}
		else
		{
			// node is not full, not a leaf, so go to the lower level
			cur_node = get_next_child(cur_node, k);
		}
	}
	cur_node->add_value(k);

}
node* two34_tree::find(int k)
{
	node* cur_node = root;
	int child_number;
	while (true)
	{
		child_number = cur_node->find_item(k);
		if (child_number != -1)
		{
			// found it
			return cur_node;
		}
		else if (cur_node->is_leaf)
		{
			return cur_node;
		}
		else
		{
			cur_node = get_next_child(cur_node, k);
		}
	}
	return nullptr;
}

// get the appropriate child of the node during the search process
node* two34_tree::get_next_child(node* curr_node, int val)
{
	int i;
	// assumes node is not empty, not full. not a leaf
	int num_items = curr_node->num_values;
	for (i = 0; i < num_items; i++)
	{
		if (val < curr_node->value[i])
		{
			return curr_node->child[i];
		}
	}
	return curr_node->child[i];
}
void two34_tree::break_3_value_node(node*p)
{
	// assumes that the node is full
	node* parent, *child2, *child3;
	int item_index;
	int itemc, itemb;

	itemc = p->remove_item();
	itemb = p->remove_item();
	child2 = p->disconnect_child(2);
	child3 = p->disconnect_child(3);

	node*new_right = new node();
	if (p == root)  // if this node is the root node
	{
		root = new node();
		parent = root;
		root->connect_child(0, p);
	}
	else
	{
		// this node is not the root, get its parent
		parent = p->parent;
	}

		// deal with parent
		item_index = parent->insert_item(itemb);
		int n = parent->num_values;
		// move the parent's connections one child to right
		for (int j = n - 1; j > item_index; j--)
		{
			node*temp = parent->disconnect_child(j);
			parent->connect_child(j+1, temp);
		}
		// connect new right to the parent
		parent->connect_child(item_index + 1, new_right);

		// deal with the new right
		new_right->insert_item(itemc);
		new_right->connect_child(0,child2);
		new_right->connect_child(1, child3); 

}
// removes the largest item
int node::remove_item()
{
	// assumes node is not empty
	int val = value[num_values - 1];
	value[num_values - 1] = -999;
	num_values--;
	return val;
}

// disconnect child from this node, return it 
// called by break_3_value_node
node* node::disconnect_child(int child_num)
{
	node * temp_node = child[child_num];
	child[child_num] = nullptr;
	return temp_node;
}

// connect child
void node::connect_child(int child_num, node*ch)
{
	child[child_num] = ch;
	if (ch != nullptr)
	{
		ch->parent = this;
	}
}

// insert the data in the given node,
// and return the index at which it is inserted
// called by split_node
int node::insert_item(int k)
{
	// the node to which the value is being added currently
	// is definitely a leaf node, which is not full definitely.
	// this care has been taken in the add function of 234tree class
	num_values++;
	// start from right, examine items
	for (int i = num_values - 1; i >= 0; i--)
	{
		if (value[i] == -999)
		{
			continue;
		}
		else
		{
			// if new item is less, shift existing to right
			if (k < value[i])
			{
				value[i++] = value[i]; // shift it to right
			}
			else
			{
				// insert new item 
				value[i + 1] = k;
				return i+1;
			}
		}
	}
	// shifted all items, but still new item not inserted,
	// it means, that the new item has to be inserted to first place
	value[0] = k;
	return 0;
}


void two34_tree::remove(int k)
{

}
node* two34_tree::find_1(int k)
{
	return nullptr;
}
void two34_tree::expand_1_value_node(node*p)
{

}
void two34_tree::fusion(node*p)
{

}
void two34_tree::rotation(node* p)
{

}

int main() {
	//The following is just an example.
	//Different test casess can be used during grading.

	two34_tree t1;
	t1.add(40);
	t1.add(30);
	t1.add(20);
	t1.in_order_traversal(t1.root);
	t1.add(100);
	t1.add(120);
	t1.in_order_traversal(t1.root);
	t1.add(200);
	t1.in_order_traversal(t1.root);
	t1.add(400);
	t1.in_order_traversal(t1.root);
	t1.add(600);
	t1.in_order_traversal(t1.root);
	t1.remove(20);
	t1.in_order_traversal(t1.root);
	t1.remove(200);
	t1.in_order_traversal(t1.root);
	t1.remove(100);
	t1.in_order_traversal(t1.root);
	getchar();
	getchar();
	return 0;

}


