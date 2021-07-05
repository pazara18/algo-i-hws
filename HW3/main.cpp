/*
 *  Name-Surname: Abdulkadir Pazar
 *	Student Number: 150180028
 *  Date: 02.01.2020
 */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Node
{
    string name;
    string team;
    int rebound;
    int point;
    int assist;
    char color;
    Node* parent;
    Node* left;
    Node* right;

    Node(string name, string team, int rebound, int assist, int point);
};

//constructor for a player node takes player, team, rebound, assist and points as arguments and fills the struct accordingly
Node::Node(string name, string team, int rebound, int assist, int point)
{
    this->name = name;
    this->team = team;
    this->rebound = rebound;
    this->assist = assist;
    this->point = point;
    this->color = 'r';
    this->parent = NULL;
    this->left = NULL;
    this->right = NULL;    
}

class RBTree
{
    Node *root;//root of RBTree
    Node *highest_point;//pointer to player with highest point
    Node *highest_rebound;//pointer to player with highest rebound
    Node *highest_assist;//pointer to player with highest assist

public:
    RBTree(){root = NULL;}
    void insert(Node*);//inserts node given as argument to tree
    void insertfix(Node*);//fixes violations created by insert function
    void rotate_left(Node*);//left-rotate operation
    void rotate_right(Node*);//right-rotate operation
    void update(Node*,string,int,int,int);//update operation used if player read from file already exists
    Node *search(string);//search player with name given as argument
    void print_tree(Node*, int);//print tree in requested format
    void print_season(string);//print season stats at the end of the season
    Node *get_root(){return root;}//returns root function
    void delete_tree(Node*);//delete the RBTree since nodes were dynamically allocated
    ~RBTree(){delete_tree(root);}//destructor for RBTree class
};

void RBTree::insert(Node *added)//iteratively insert node in correct position
{
	if (root == NULL){//insert first node
		root = added;
        highest_assist = added;
        highest_rebound = added;
        highest_point = added;	
	} else{
		Node* temp = root;
		bool is_inserted = false;
		while (!is_inserted){
			if (added->name < temp->name){//if name is alphabetically smaller than temp move left
				if (temp->left != NULL){//as long as end of the tree is not reached
					temp = temp->left;
				}
				else{//place node at the leaf and break loop
					temp->left = added;
					added->parent = temp;
					is_inserted = true;
				}
			} else{//if name is alphabetically bigger than temp move left
				if (temp->right != NULL){//as long as end of the tree is not reached
					temp = temp->right;
				}
				else{//place node at the leaf and break loop
					temp->right = added;
					added->parent = temp;
					is_inserted = true;
				}
			}
		}
	}
    if(highest_assist->assist < added->assist) highest_assist = added;//replace highest assist node if newly added node has greater assist number
    if(highest_rebound->rebound < added->rebound) highest_rebound = added;//replace highest rebound node if newly added node has greater assist number
    if(highest_point->point < added->point) highest_point = added;//replace highest point node if newly added node has greater assist number
    insertfix(added);//call insertfix to fix any violations
}

void RBTree::insertfix(Node *added)//mostly adapted from pseudocode at CLRS book 2nd edition p. 281
{
    Node *uncle;

    while (added->parent && added->parent->color == 'r'){//as long as violations exist
        if(added->parent == added->parent->parent->left){//if addeds parent is a left child
            uncle = added->parent->parent->right;//its uncle if exists is a right child
            if(uncle && uncle->color == 'r'){//if its uncle exists and its color is red
                added->parent->color = 'b';//addeds parents color is set to black
                uncle->color = 'b';//uncles color is set to black
                added->parent->parent->color = 'r';//grandparents color is set to red
                added = added->parent->parent;//move up the tree
            } else{
                if( added == added->parent->right){//if added became a right child
                    added = added->parent;//move up again
                    rotate_left(added);//call rotate left on addeds grandparent
                }
                added->parent->color = 'b';//set addeds parents color to black
                added->parent->parent->color = 'r';//set addeds grandparents color to red
                rotate_right(added->parent->parent);//call rotate right on added
            }
        } else{//its uncle if exists is a left child
            uncle = added->parent->parent->left;//its uncle if exists is a left child
            if(uncle && uncle->color == 'r'){//if its uncle exists and its color is red
                added->parent->color = 'b';//addeds parents color is set to black
                uncle->color = 'b';//uncles color is set to black
                added->parent->parent->color = 'r';//grandparents color is set to red
                added = added->parent->parent;//move up the tree
            } else{
                if(added == added->parent->left){//if added became a left child
                    added = added->parent;//move up again
                    rotate_right(added);//call rotate right on added
                }
                added->parent->color = 'b';//set addeds parents color to black
                added->parent->parent->color = 'r';//set addeds grandparents color to red
                rotate_left(added->parent->parent);//call rotate left on addeds grandparent
            }
        }
    }
    root->color = 'b';//root needs to be black
}

void RBTree::rotate_left(Node *rotated)
{
    Node *r_child;//right child of rotated node
    r_child = rotated->right;
    rotated->right = r_child->left;//left child of r_child becomes right child of rotated
    if (r_child->left != NULL) r_child->left->parent = rotated;//if left child of r_child existed its parent became rotated
    r_child->parent = rotated->parent;//r_childs parent became rotateds parent
    if (rotated->parent == NULL) root = r_child;//if rotated was root r_child became root
    else{
        if (rotated == rotated->parent->left) rotated->parent->left = r_child;//if rotated was left child of its parent r_child becomes parents left child
        else rotated->parent->right = r_child;//otherwise r_child becomes its right child
    }
    r_child->left = rotated;//rotated becomes r_childs left chlid
    rotated->parent = r_child;//r_child becomes rotateds parent
}

void RBTree::rotate_right(Node *rotated)
{
    Node *l_child;//left child of rotated node
    l_child = rotated->left;
    rotated->left = l_child->right;//right child of l_child becomes left child of rotated
    if (l_child->right != NULL) l_child->right->parent = rotated;//if right child of l_child existed its parent became rotated
    l_child->parent = rotated->parent;//l_childs parent became rotateds parent
    if (rotated->parent == NULL) root = l_child;//if rotated was root l_child became root
    else{
        if (rotated == rotated->parent->right) rotated->parent->right = l_child;//if rotated was right child of its parent l_child becomes parents right child
        else rotated->parent->left = l_child;//otherwise l_child becomes its left child
    }
    l_child->right = rotated;//rotated becomes l_childs right chlid
    rotated->parent = l_child;//l_child becomes rotateds parent
}

void RBTree::update(Node* updated,string team, int rebound, int assist, int point)
{
    updated->team = team;//change team of the player
    updated->rebound += rebound;//add new rebounds to total count
    updated->assist += assist;//add new assists to total count
    updated->point += point;//add new points to total count

    if(highest_assist->assist < updated->assist) highest_assist =updated;//change highest assist pointer if player has highest assist number after update
    if(highest_rebound->rebound < updated->rebound) highest_rebound = updated;//change highest rebound pointer if player has highest rebound number after update
    if(highest_point->point < updated->point) highest_point = updated;//change highest point pointer if player has highest point number after update
}

Node* RBTree::search(string name)//returns pointer to node of the player if exists, otherwise returns NULL
{

    if(root == NULL) return NULL;//if tree is empty return NULL 

    Node *temp = root;//start from root
    while(temp->name != name){//as long as player is not found
        if(temp != NULL){
            if(temp->name > name) temp = temp->left;//if temp points to a player that is alphabetically bigger than searched player move left
            else temp = temp->right;//if temp points to a player that is alphabetically smaller than searched player move right
         
            if(temp == NULL) return NULL;//if reached the end of the tree return NULL			
        }
    }
    return temp;//return temp as a result if found in tree
}

void RBTree::print_tree(Node *node, int level)
{
	if (node != NULL){//as long as leaves are not reached
		for (int i = 0; i < level; i++){//print dashes to symbolize levels
			cout << "-";
		}
        if(node->color == 'b') cout << "(BLACK) ";
        else cout << "(RED) ";
        cout << node->name << endl;
		level++;
		print_tree(node->left, level);//do recursive call to print left child
		print_tree(node->right, level);//do recursive call to print right child
	}
}

void RBTree::print_season(string season)//print statistics at the end of seasons
{
    cout << "End of the " << season << " Season" << endl;
    cout << "Max points: " << highest_point->point << " - Player Name: " << highest_point->name << endl;
    cout << "Max assists: " << highest_assist->assist << " - Player Name: " << highest_assist->name << endl;
    cout << "Max rebs: " << highest_rebound->rebound << " - Player Name: " << highest_rebound->name << endl;
}

void RBTree::delete_tree(Node* root)//delete tree recursively
{
    if(root){
        delete_tree(root->left);//delete left subtree
        delete_tree(root->right);//delete right subtree
        delete root;//delete root
    }
}

int main(int argc, char** argv)
{
    ifstream in(argv[1]);//start reading file given as argument

    bool first_season = true;//to print tree only at first season
    bool first_line = true;//used since prev season string is different than season always during first line

    if (!in){
        cerr << "CANNOT FIND FILE!" << endl;
        return 1;
    }

    string season = "";//season string
    string prev_season = "";//previous season to detect season changes to print season stats
    string name = "";//name string
    string team = "";//team string
    string s = "";//dummy string to read points, assists, rebounds
    int point = 0;
    int assist = 0;
    int rebound = 0;

    string header;//string to read header
    getline(in, header);

    RBTree tree;//create tree 

    while(!in.eof()){//start reading from file
        prev_season = season;//set previous season to season read in previous line
		getline(in, season, ',');//read season
        if(!first_line && prev_season != season){//if not in first line and season has changed
            tree.print_season(prev_season);//print season stats
            if(first_season){//if first season print tree
                tree.print_tree(tree.get_root(),0);
                first_season = false;
            }
            prev_season = season;
        }
        first_line = false;//set to false after first line 
		getline(in, name, ',');//read name to string
        getline(in, team, ',');//read team to string
        getline(in, s, ',');//read rebound to s
        rebound = stoi(s);//convert s to int
		getline(in, s, ',');//read assist to s
        assist = stoi(s);//convert s to int
        getline(in, s, '\n');//read point to s
        point = stoi(s);//convert s to int
        

        Node *temp = tree.search(name);//search if a node with name exists in tree
        if(temp) tree.update(temp, team, rebound, assist, point);//if it exists update player
        else{
            Node* newnode = new Node(name, team, rebound, assist, point);//create new node
            tree.insert(newnode);//insert new node to tree
        }
    }
    tree.print_season(season);//print last season stats
    return 0;
}