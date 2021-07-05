/*
 * Name: Abdulkadir
 * Surname: Pazar
 * Student No: 150180028
 * Date: 09-02-2021
 */

#include <iostream>
using namespace std;

//struct for values
struct val
{
    int x;
    int y;
    char z;
    char key;

    //overloaded greater than for easier comparison
    friend bool operator>(const val &val1, const val &val2)
    {
        if (val1.key == 'x')
            return val1.x > val2.x;
        if (val1.key == 'y')
            return val1.y > val2.y;
        if (val1.key == 'z')
            return val1.z > val2.z;
        return true;
    }

    //overloaded equals for deletion operation
    friend bool operator==(const val &val1, const val &val2)
    {
        if (val1.key == 'x')
            return val1.x == val2.x;
        if (val1.key == 'y')
            return val1.y == val2.y;
        if (val1.key == 'z')
            return val1.z == val2.z;
        return true;
    }
};

class BTreeNode
{
    val *vals;
    int degree;
    BTreeNode **child_array; //child pointer array
    int num_keys;
    bool is_leaf;

public:
    BTreeNode(int degree, bool is_leaf); //ctor
    void insert_non_full(val value);                    //called when node is non-full
    void split_child(int index, BTreeNode *splitted);   //called to split child if found child is full
    void remove(val value);
    int find_key(val value);
    void remove_leaf(int index);
    void remove_internal(int index);
    val get_prev(int index);
    val get_next(int index);
    void fill(int index);
    void get_val_from_prev(int index);
    void get_val_from_next(int index);
    void merge(int index);
    void traverse();

    friend class BTree;
};

class BTree
{
    BTreeNode *root;
    int degree;

public:
    BTree(int degree) //ctor
    {
        root = NULL;
        this->degree = degree;
    }

    void traverse()
    {
        if (root != NULL) root->traverse();
    } //print tree in prefix order

    void insert(val value); //insert given value

    void remove(val value); //remove given key

    BTreeNode *get_root() { return root; } //getter for root
};

void BTreeNode::traverse() //print tree in prefix order
{
    for (int i = 0; i < num_keys; i++) 
    { 
        cout<<"("<<vals[i].x<<","<<vals[i].y<<","<<vals[i].z<<")"; 
        
    } 
    cout<<"\n";
    int j = 0;
    while(j<=num_keys && is_leaf == false){
        child_array[j]->traverse();
        j++;
    }
}

void BTree::insert(val value)
{
    if (root == NULL) //inserting first element
    {
        root = new BTreeNode(degree, true);
        root->vals[0] = value;
        root->num_keys = 1;
    }
    else
    {
        if (root->num_keys == 2 * degree - 1) //if root is full
        {
            BTreeNode *new_root = new BTreeNode(degree, false); //create new root

            new_root->child_array[0] = root; //old root becomes new roots child

            new_root->split_child(0, root); //split old root and move one value to new root

            int i = 0;
            if (value > new_root->vals[0]) //decide which children is going to have new value
                i++;
            new_root->child_array[i]->insert_non_full(value); //insert value to children

            root = new_root; //new root becomes root
        }
        else
            root->insert_non_full(value); //insert to root if not full
    }
}

void BTree::remove(val k)
{
    //call remove on root
    root->remove(k);
    if (root->num_keys == 0) //if root became empty after remove
    {
        BTreeNode *tmp = root;
        if (root->is_leaf)
            root = NULL; //if root has no child
        else
        {
            root = root->child_array[0]; //roots first child becomes root
        }
        delete tmp;
    }
}

BTreeNode::BTreeNode(int degree, bool is_leaf) //ctor for node
{
    this->degree = degree;
    this->is_leaf = is_leaf;

    vals = new val[2 * degree - 1];
    child_array = new BTreeNode *[2 * degree];
    for (int i = 0; i < 2 * degree; i++)
        child_array[i] = NULL;

    num_keys = 0;
}

int BTreeNode::find_key(val value)
{
    int i = 0;
    while (i < num_keys && value > vals[i])
        i++;
    return i;
}

void BTreeNode::remove(val value)
{
    int i = find_key(value);
    //i becomes index of the key if it is in this node
    //if key to remove is in this node
    if (i < num_keys && vals[i] == value)
    {
        //if node is a leaf node
        if (is_leaf)
            remove_leaf(i);
        else
            remove_internal(i);
    }
    else
    {
        bool in_subtree_last_child;
        if (i == num_keys)
            in_subtree_last_child = true;
        else
            in_subtree_last_child = false;
        //if child key is non-full fill it
        if (child_array[i]->num_keys < degree)
            fill(i);

        //if last child has been merged
        if (in_subtree_last_child && i > num_keys)
            child_array[i - 1]->remove(value);
        //if last child is not merged
        else
            child_array[i]->remove(value);
    }
}

void BTreeNode::remove_leaf(int index)
{
    for (int i=index + 1; i<num_keys; i++) 
        vals[i-1] = vals[i]; 
    // Reduce the count of keys 
    num_keys--; 
}

void BTreeNode::remove_internal(int index)
{
    val v = vals[index]; //key to remove
    //if child before key has more than t-1 keys
    if (child_array[index]->num_keys >= degree)
    {
        val prev = get_prev(index);       //get value before v
        vals[index] = prev;               //replace v vith prev
        child_array[index]->remove(prev); //recursive remove replacing val
    }
    // if child after key has more than t-1 keys
    else if (child_array[index + 1]->num_keys >= degree)
    {
        val next = get_next(index);           //get value after v
        vals[index] = next;                   //replace v with next
        child_array[index + 1]->remove(next); //rec remove next
    }
    //if both child has less keys merge
    else
    {
        merge(index);
        child_array[index]->remove(v); //remove v after merge
    }
}

val BTreeNode::get_prev(int index)
{
    BTreeNode *c = child_array[index];  //child before value at passed index
    while (!c->is_leaf)   //go down until leaf
        c = c->child_array[c->num_keys]; //current child becomes last child of child array
    return c->vals[c->num_keys - 1]; //last element of this node is the previous value before value at passed index
}

val BTreeNode::get_next(int index)
{
    BTreeNode *c = child_array[index + 1]; //child right after value at passed index
    while (!c->is_leaf)                    //go down until leaf
        c = c->child_array[0]; //current child becomes first child of child array
    return c->vals[0];                     //first element of this node is the next value before value at passed index
}

void BTreeNode::fill(int index)
{
    //if prev child has more than t-1 keys
    if (index != 0 && child_array[index - 1]->num_keys >= degree)
        get_val_from_prev(index);
    //if next child has more than t-1 keys
    else if (index != num_keys && child_array[index + 1]->num_keys >= degree)
        get_val_from_next(index);
    //otherwise merge
    else
    {
        if (index != num_keys)
            merge(index);
        else //if last child merge with prev sibling
            merge(index - 1);
    }
}

void BTreeNode::get_val_from_prev(int index)
{
    BTreeNode *child = child_array[index];
    BTreeNode *sibling = child_array[index - 1];
    //move all keys one step ahead before getting val
    for (int i = child->num_keys - 1; i >= 0; i--)
        child->vals[i + 1] = child->vals[i];

    //if child is internal move childs one ahead
    if (!(child->is_leaf))
    {
        for (int i = child->num_keys; i >= 0; i--)
            child->child_array[i + 1] = child->child_array[i];
    }
    //key from parent moves to child
    child->vals[0] = vals[index - 1];

    //siblings last child becomes childs first child
    if (!(child->is_leaf))
        child->child_array[0] = sibling->child_array[sibling->num_keys];

    //key from sibling moves to parent
    vals[index - 1] = sibling->vals[sibling->num_keys - 1];

    //update key counts
    child->num_keys++;
    sibling->num_keys--;
}

void BTreeNode::get_val_from_next(int index)
{

    BTreeNode *child = child_array[index];
    BTreeNode *sibling = child_array[index + 1];
    //key is inserted as last key of child
    child->vals[child->num_keys] = vals[index];
    //if child is internal siblings first child becomes childs last child
    if (!(child->is_leaf))
        child->child_array[child->num_keys + 1] = sibling->child_array[0];

    //first sibling key is moved to current nodes indexth value
    vals[index] = sibling->vals[0];

    //move sibling keys one to left
    for (int i = 1; i < sibling->num_keys; i++)
        sibling->vals[i - 1] = sibling->vals[i];

    //if sibling is internal move child elements one to left
    if (!(sibling->is_leaf))
    {
        for (int i = 1; i <= sibling->num_keys; i++)
            sibling->child_array[i - 1] = sibling->child_array[i];
    }
    //update key counts
    child->num_keys++;
    sibling->num_keys--;
}

void BTreeNode::merge(int index)
{
    BTreeNode *child = child_array[index];       //child to merge
    BTreeNode *sibling = child_array[index + 1]; //next sibling

    child->vals[degree - 1] = vals[index]; //insert key from current node to child[t-1]

    //copy keys from sibling to child
    for (int i = 0; i < sibling->num_keys; i++)
        child->vals[i + degree] = sibling->vals[i];

    //copy child array from sibling
    if (!(child->is_leaf))
    {
        for (int i = 0; i <= sibling->num_keys; i++){
            child->child_array[i + degree] = sibling->child_array[i];
        }
    }
    //moving all keys after index key one to left
    for (int i = index + 1; i < num_keys; i++)
        vals[i - 1] = vals[i];
    //moving all childs after index key one to left
    for (int i = index + 2; i <= num_keys; i++)
        child_array[i - 1] = child_array[i];

    //update key counts of child and current node
    child->num_keys += sibling->num_keys+1;
    num_keys--;
    delete(sibling);
}

void BTreeNode::insert_non_full(val value)
{
    int i = num_keys - 1;

    if (is_leaf)
    {
        //find location of new key to insert
        // move all bigger keys one ahead
        while (i >= 0 && vals[i] > value)
        {
            vals[i + 1] = vals[i];
            i--;
        }

        vals[i + 1] = value;
        num_keys++;
    }
    else
    {
        // Find the child which is going to have the new key
        while (i >= 0 && vals[i] > value)
            i--;

        // Check if the found child is full
        if (child_array[i + 1]->num_keys == 2 * degree - 1)
        {
            split_child(i + 1, child_array[i + 1]);
            if (value > vals[i + 1])
                i++;
        }
        child_array[i + 1]->insert_non_full(value);
    }
}

void BTreeNode::split_child(int index, BTreeNode *splitted)
{
    //new node to store degree - 1 values
    BTreeNode *new_node = new BTreeNode(splitted->degree, splitted->is_leaf);
    new_node->num_keys = degree - 1;

    //copy keys
    for (int j = 0; j < degree - 1; j++)
        new_node->vals[j] = splitted->vals[j + degree];

    //copy children
    if (!(splitted->is_leaf)) //if splitted node is not leaf
    {
        for (int j = 0; j < degree; j++){
            new_node->child_array[j] = splitted->child_array[j + degree];
        }
    }

    //reduce keys of splitted node
    splitted->num_keys = degree - 1;

    //create space for new child
    for (int j = num_keys; j >= index + 1; j--)
        child_array[j + 1] = child_array[j];

    //new child added to child array
    child_array[index + 1] = new_node;

    //move keys gt than index to one ahead
    for (int j = num_keys - 1; j >= index; j--)
        vals[j + 1] = vals[j];

    //copy middle key of splitted to this node
    vals[index] = splitted->vals[degree - 1];
    
    num_keys++;
}

int main()
{
    int node_count;
    int degree;
    char key_type;
    val reader;
    val to_remove; //dummy value struct to pass key to be deleted
    cin >> node_count >> degree >> key_type;
    reader.key = key_type;
    BTree t(degree);
    for (int i = 0; i < node_count; i++)
    {
        cin >> reader.x >> reader.y >> reader.z;
        t.insert(reader);
    }
    if (key_type == 'x')
    {
        to_remove.key = 'x';
        cin >> to_remove.x;
    }
    else if (key_type == 'y')
    {
        to_remove.key = 'y';
        cin >> to_remove.y;
    }
    else
    {
        to_remove.key = 'z';
        cin >> to_remove.z;
    }
    t.remove(to_remove);
    t.get_root()->traverse();
    return 0;
}