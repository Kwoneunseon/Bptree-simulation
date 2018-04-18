#include<iostream>
#include<vector>
#include<algorithm>
#include<math.h>
#include<queue>
#include <sstream>
#include <string>
#include <fstream>
#include <stdlib.h>

using namespace std;

#define MIN_VAL -1000000001;
int N=4; //size of each node (Max no. of keys)

struct node
{
    bool leaf;
    vector<int> records;
    vector<int> count;
    vector<node*> pointers;
    node() //default node is not a leaf
    {
	leaf = false;
    }
};

node* root;

void range(node* cur, int llimit,int rlimit)
{
    if(cur->leaf)
    {
	//perform search in node
	int total=0;

	while(cur && cur->records[0] <= rlimit)
	{
	    for(int i=0;i<cur->records.size();i++)
	    {
		if(llimit <= cur->records[i] && cur->records[i] <= rlimit)
		{
		    total += cur->count[i];
		}
	    }
	    if(cur->pointers.empty())
		break;
	    
	    cur = cur->pointers[0];
	}
	
	cout<<total<<endl;
    }
    else
    {
	int i=0;
	//find correct child node and recurse
	for(i=0;i<cur->records.size();i++)
	{
	    if(llimit < cur->records[i])
	    {
		break;
	    }
	}
	range(cur->pointers[i],llimit,rlimit);
    }
}

void count(node* cur, int key)
{
    if(cur->leaf)
    {
	//perform search in node
	for(int i=0;i<cur->records.size();i++)
	{
	    if(key == cur->records[i])
	    {
		cout<<cur->count[i]<<endl;
		return;
	    }
	}
	cout<<0<<endl;
    }
    else
    {
	int i=0;
	//find correct child node and recurse
	for(i=0;i<cur->records.size();i++)
	{
	    if(key < cur->records[i])
	    {
		break;
	    }
	}
	count(cur->pointers[i],key);
    }
}

void find(node* cur, int key)
{
    if(cur->leaf)
    {
	//perform search in node
	for(int i=0;i<cur->records.size();i++)
	{
	    if(key == cur->records[i])
	    {
		cout<<"YES"<<endl;
		return;
	    }
	}
	cout<<"NO"<<endl;
    }
    else
    {
	int i=0;
	//find correct child node and recurse
	for(i=0;i<cur->records.size();i++)
	{
	    if(key < cur->records[i])
	    {
		break;
	    }
	}
	find(cur->pointers[i],key);
    }
}

int insert_vec(node* tnode,int key)
{
    int i=0;
    for(i=0;i<tnode->records.size();i++)
    {
	if(key <= tnode->records[i])
	    break;
    }

    if(i<tnode->records.size() && key == tnode->records[i]) //Handle duplicate keys
    {
	tnode->count[i]++;	
    }
    else
    {
	tnode->records.insert(tnode->records.begin()+i,key);
	if(tnode->leaf)
	    tnode->count.insert(tnode->count.begin()+i,1);
    }
    
    return i;
}

void print_btree(queue<node*> printq) //debug purpose
{
    if(printq.empty())
	return;
    
    queue<node*> newq;
    while(!printq.empty())
    {
	for(int i=0;i<printq.front()->pointers.size();i++)
	{
	    if(printq.front()->leaf)
		break;
	    newq.push(printq.front()->pointers[i]);
	}

	if(!printq.front()->pointers.empty())
	    cout<<printq.front()<<" -- "<<printq.front()->pointers[0];
	
	for(int i=0;i<printq.front()->records.size();i++) //TODO: Try using templates to print vector
	{
	    if(printq.front()->leaf)
	    {
		cout<<" @ "<<printq.front()->records[i];
	    }
	    else
	    {
		cout<<" @ "<<printq.front()->records[i]<<" @ "<<printq.front()->pointers[i+1];
	    }
	}
	cout<<"  ##  ";
	printq.pop();
    }
    cout<<endl;
    cout<<endl;
    print_btree(newq);
}

node* key_insert(node* cur,node* par,int val)
{
    if(cur->leaf == true)
    {
	insert_vec(cur,val); //insert key
	
	if(cur->records.size() > N) //split situation for leaf node
	{
	    //cout<<"Leaf Node Overflow"<<endl;

	    node* rnode = new node(); //Create sibling on the right
	    rnode->leaf = true;

	    rnode->records.assign(cur->records.begin()+ceil((N+1)/2.0),cur->records.end()); //copy half elements from lnode to rnode
	    rnode->count.assign(cur->count.begin()+ceil((N+1)/2.0),cur->count.end()); //copy the counts for the same
	    
	    cur->records.resize(ceil((N+1)/2.0)); //remove elements of rnode from lnode
	    cur->count.resize(ceil((N+1)/2.0)); //remove counts of rnode from lnode

	    //Create connection across leaves
	    rnode->pointers.resize(1);
	    cur->pointers.resize(1);
	    rnode->pointers[0] = cur->pointers[0];
	    cur->pointers[0] = rnode; //using 0th pointer element to connect leaves

	    if(cur == par) //root node is being split
	    {
		node* rootnode = new node();
		rootnode->pointers.push_back(cur);
		rootnode->pointers.push_back(rnode);
		rootnode->records.push_back(rnode->records[0]);
		root = rootnode; //Updated to new root node
		return NULL;
	    }
	    
	    return rnode; //return entire rnode for parent to link
	}

	return NULL;
    }
    else 
    {
	//records for a node CANNOT be empty. Avoiding check
	int i=0;
	while(i < cur->records.size() && val >= cur->records[i]) //Find the appropriate pointer
	{
	    i++;
	}

	node* ret = key_insert(cur->pointers[i],cur,val); //Go down appropriate pointer to next child node

	//Check if key returned and then insert to current inner node.
	//ret->records[0] is the key that needs to be inserted in parent

	if(ret != NULL) //Split had occurred in the node below
	{
	    int pos = insert_vec(cur,ret->records[0]);

	    if(ret->leaf) //The node split below was a leaf node
	    {
		cur->pointers.insert(cur->pointers.begin()+pos+1,ret); //+1 represents pointer to node greater than current key value
	    }
	    else //The node split below was a inner node. 
	    {
		cur->pointers.insert(cur->pointers.begin()+pos+1,ret->pointers[0]); //Different rnode as compared to leaf node
	    }
	    
	    // Handle splitting of inner node.
	    if(cur->records.size() > N) //inner node is overflowing
	    {
		//cout<<"Inner Node Overflow"<<endl;
		node* rnode = new node();

		int tkey = cur->records[ceil(N/2.0)];
		rnode->records.assign(cur->records.begin()+ceil(N/2.0)+1,cur->records.end()); //copy half elements from lnode to rnode
		rnode->pointers.assign(cur->pointers.begin()+ceil((N+2)/2.0),cur->pointers.end()); //copy the pointers for the same

		cur->records.resize(ceil(N/2.0)); //remove elements of rnode from lnode
		cur->pointers.resize(ceil((N+2)/2.0)); //remove pointers of rnode from lnode

		if(cur == par) //root node is being split
		{
		    node* rootnode = new node();
		    rootnode->pointers.push_back(cur);
		    rootnode->pointers.push_back(rnode);
		    rootnode->records.push_back(tkey);
		    root = rootnode; //Updated to new root node
		    return NULL;
		}

		//TODO: return a node with middle key and pointer to rnode
		node* retnode = new node();
		retnode->records.push_back(tkey);
		retnode->pointers.push_back(rnode);
		return retnode;
	    }   
	}
    }
}

void print_assist()
{
    queue<node*> tnode;
    tnode.push(root);
    cout<<"Printing B+tree"<<endl;
    print_btree(tnode);
}

int main(int argc,char *argv[])
{
    cout<<"B+Tree Program"<<endl;
    
    root = new node();
    root->leaf = true;

    ifstream infile(argv[1]);
    string line,c;
    int a,b=-1;
    while (getline(infile, line))
    {
	istringstream iss(line);

	iss>>c>>a>>b;

	if(line[0] == 'I')
	{
	    key_insert(root,root,a);
	}
	else if(line[0] == 'F')
	{
	    find(root,a);
	}
	else if(line[0] == 'C')
	{
	    count(root,a);
	}
	else if(line[0] == 'R')
	{
	    range(root,a,b);
	}
	else if(line[0] == 'P')
	{
	    print_assist();
	}
	else
	{
	    cout<<"Input format error"<<endl;
	}
    }

    /*
    //Test Code
    key_insert(root,root,5);
    key_insert(root,root,3);
    key_insert(root,root,2);
    key_insert(root,root,4);
    print_assist();
    
    key_insert(root,root,6);
    key_insert(root,root,7);
    key_insert(root,root,1);
    key_insert(root,root,0);
    key_insert(root,root,5);
    print_assist();
    
    key_insert(root,root,8);
    key_insert(root,root,-1);
    key_insert(root,root,-2);
    key_insert(root,root,2);
    key_insert(root,root,2);
    print_assist();
    
    key_insert(root,root,-3);
    key_insert(root,root,-4);
    key_insert(root,root,-5);
    key_insert(root,root,-6);

    print_assist();
    key_insert(root,root,9);
    
    find(root,0);
    find(root,7);
    find(root,-14);
    
    count(root,2);
    count(root,-11);
    count(root,4);

    range(root,0,4);
    range(root,53,68);
    range(root,6,12);
    */

}
