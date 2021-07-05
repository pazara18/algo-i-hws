/*
 * Name: Abdulkadir
 * Surname: Pazar
 * Student No: 150180028
 * Date: 09-02-2021
 */
#include<iostream>
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
};

class BTreeNode 
{ 
    val* vals;
    int degree;
    BTreeNode** child_array;//child pointer array
    int num_keys;
    bool is_leaf;
public: 
    BTreeNode(int degree, bool is_leaf);//ctor
    void insert_non_full(val value);//called when node is non-full 
    void split_child(int index, BTreeNode *splitted);//called to split child if found child is full
    void traverse();
  

    friend class BTree; 
}; 
  
class BTree 
{ 
    BTreeNode *root;
    int degree;
public: 
    BTree(int degree) //ctor
    {  root = NULL;  this->degree = degree;} 
  
    void traverse(){if (root) root->traverse();}//print tree in prefix order
  
    void insert(val value); //insert given value

    BTreeNode* get_root(){return root;} //getter for root
}; 
  
BTreeNode::BTreeNode(int degree, bool is_leaf) //ctor for node
{ 
    this->degree = degree; 
    this->is_leaf = is_leaf;
  
    vals = new val[2*degree-1];
    child_array = new BTreeNode*[2*degree];
    for(int i = 0; i < 2*degree;i++)
        child_array[i] = NULL;
  
    num_keys = 0; 
} 

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
    cin >> node_count >> degree >> key_type; 
    reader.key = key_type; 
    BTree t(degree);
    for (int i = 0; i < node_count; i++){
        cin >> reader.x >> reader.y >> reader.z;
        t.insert(reader);
    }
    t.get_root()->traverse();
    return 0; 
} 