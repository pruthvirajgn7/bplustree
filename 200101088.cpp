#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int t_global;
int d_global;
int num_index_nodes=0;
int num_leaf_nodes=0;

class Node {
     bool IS_LEAF;
     int *key, size;
     Node **ptr;
     friend class BPTree;

     public:
          Node();
};

int MAX;

Node::Node() {
  key = new int[MAX];
  ptr = new Node *[MAX + 1];
}

class BPTree {
    Node *root;
    void insertInternal(int, Node *, Node *);
    Node *findParent(Node *, Node *);

    public:
        BPTree();
        void search(int);
        void insert(int);
        void display(Node *);
        Node *getRoot();
};

BPTree::BPTree() {
  root = NULL;
}

void BPTree::search(int x){
     if (root == NULL) {
         cout << "Tree is empty\n";
     }
     else{
         Node *cursor = root;
         while (cursor->IS_LEAF == false){
             for (int i = 0; i < cursor->size; i++) {
                 if (x < cursor->key[i]) {
                     cursor = cursor->ptr[i];
                     break;
                 }
                 if (i == cursor->size - 1) {
                     cursor = cursor->ptr[i + 1];
                     break;
                 }
             }
         }
         for (int i = 0; i < cursor->size; i++) {
            if (cursor->key[i] == x) {
                 cout << "Found\n";
                 return;
            }
         }
         cout << "Not found\n";
     }
}

void BPTree::insert(int x){
    if (root == NULL){
        root = new Node;
        num_leaf_nodes++;
        root->key[0] = x;
        root->IS_LEAF = true;
        root->size = 1;
    }
    else{
        Node *cursor = root;
        Node *parent;
        while (cursor->IS_LEAF == false){
            parent = cursor;
            for (int i = 0; i < cursor->size; i++) {
                 if (x < cursor->key[i]) {
                    cursor = cursor->ptr[i];
                    break;
                 }
                 if (i == cursor->size - 1) {
                     cursor = cursor->ptr[i + 1];
                     break;
                 }
            }
        }
        if (cursor->size < 2*d_global){
            int i = 0;
            while (x > cursor->key[i] && i < cursor->size)
                i++;
            for (int j = cursor->size; j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
            }
            cursor->key[i] = x;
            cursor->size++;
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
            cursor->ptr[cursor->size - 1] = NULL;
        }
        else{
            Node *newLeaf = new Node;
            num_leaf_nodes++;
            vector <int> virtualNode;
            for (int i = 0; i < 2*d_global; i++) {
                virtualNode.push_back(cursor->key[i]);
            }
            int i = 0, j;
            while (x > virtualNode[i] && i < 2*d_global)
                i++;
            for (int j = 2*d_global; j > i; j--) {
                 virtualNode[j] = virtualNode[j - 1];
            }
            virtualNode[i] = x;
            newLeaf->IS_LEAF = true;
            cursor->size = d_global;
            newLeaf->size = d_global+1;
            //cursor->ptr[cursor->size] = newLeaf;
           //newLeaf->ptr[newLeaf->size] = cursor->ptr[2*d_global];
          // cursor->ptr[2*d_global] = NULL;
            for (i = 0; i < cursor->size; i++) {
                cursor->key[i] = virtualNode[i];
            }
            for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
                 newLeaf->key[i] = virtualNode[j];
            }
            if (cursor == root) {
                Node *newRoot = new Node;
                newRoot->key[0] = newLeaf->key[0];
                newRoot->ptr[0] = cursor;
                newRoot->ptr[1] = newLeaf;
                newRoot->IS_LEAF = false;
                num_index_nodes++;
                newRoot->size = 1;
                root = newRoot;
            }
            else {
                insertInternal(newLeaf->key[0], parent, newLeaf);
            }
        }
    }
}

void BPTree::insertInternal(int x, Node *cursor, Node *child){
    if (cursor->size < 2*t_global + 1){
         int i = 0;
         while (x > cursor->key[i] && i < cursor->size)
            i++;
         for (int j = cursor->size; j > i; j--) {
             cursor->key[j] = cursor->key[j - 1];
         }
         for (int j = cursor->size + 1; j > i + 1; j--) {
            cursor->ptr[j] = cursor->ptr[j - 1];
         }
         cursor->key[i] = x;
         cursor->size++;
         cursor->ptr[i + 1] = child;
    }
    else{
        Node *newInternal = new Node;
        num_index_nodes+=2;
        vector <int> virtualKey;
        vector <Node *> virtualPtr;
        for (int i = 0; i < 2*t_global + 1; i++) {
            virtualKey.push_back(cursor->key[i]);
        }
        for (int i = 0; i < 2*t_global + 2; i++) {
            virtualPtr.push_back(cursor->ptr[i]);
        }

        int i = 0, j;
        while (x > virtualKey[i] && i < 2*t_global+1)
            i++;
        for (int j = 2*t_global + 1; j > i; j--) {
            virtualKey[j] = virtualKey[j - 1];
        }
        virtualKey[i] = x;
        for (int j = 2*t_global + 2; j > i + 1; j--) {
            virtualPtr[j] = virtualPtr[j - 1];
        }
        virtualPtr[i + 1] = child;
        newInternal->IS_LEAF = false;
        cursor->size = t_global;
        newInternal->size = t_global+1;
        for (i = 0; i < cursor->size; i++) {
                cursor->key[i] = virtualKey[i];
        }
        for (i = 0, j = cursor->size+1; i < newInternal->size; i++, j++) {
            newInternal->key[i] = virtualKey[j];
        }
        for (i = 0, j = cursor->size+1 ; i < newInternal->size + 1; i++, j++) {
            newInternal->ptr[i] = virtualPtr[j];
        }
        if (cursor == root) {
            Node *newRoot = new Node;
            newRoot->key[0] = virtualKey[cursor->size];
            newRoot->ptr[0] = cursor;
            newRoot->ptr[1] = newInternal;
            newRoot->IS_LEAF = false;
            newRoot->size = 1;
            root = newRoot;
        }
        else{
            insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
        }
    }
}


Node *BPTree::findParent(Node *cursor, Node *child){
    Node *parent;
    if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
        return NULL;
    }
    for (int i = 0; i < cursor->size + 1; i++) {
         if (cursor->ptr[i] == child) {
            parent = cursor;
            return parent;
         } 
         else {
             parent = findParent(cursor->ptr[i], child);
             if (parent != NULL)
                return parent;
         }
    }
    return parent;
}

void BPTree::display(Node *cursor){
    cout<< num_index_nodes <<" "<<num_leaf_nodes<<" ";
    for(int i=0;i<root->size;i++){
        cout<< root->key[i] << " ";
    }
    cout<<"\n";
}

Node *BPTree::getRoot() {
  return root;
}

int main(){
    cin>>d_global;
    cin>>t_global;
    if(2*t_global + 1 > 2*d_global){
        MAX = 2*t_global + 1;
    }
    else{
         MAX = 2* d_global;
    }
    BPTree node;
    int temp;
    while(cin>>temp){
        if(temp==1){
            int temp2;
            cin>>temp2;
            node.insert(temp2);
        }
        else if(temp == 2){
            node.display(node.getRoot());
        }
        if(temp == 3){
            break;
        }
    }
}

