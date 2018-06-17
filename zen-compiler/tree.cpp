// Author: Tenton Lien
// 10/7/2018
#include <iostream>
#include <string>
#include <map>
#include "zc.h"
#include "tree.h"

using namespace std;

Tree::Tree() {
    root -> data = "root";
	root -> firstChild = NULL;
	root -> rightSibling = NULL;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
}


void Tree::release(TreeNode* node) {
    if (node == NULL) return;
    release(node -> firstChild);
    release(node -> rightSibling);
}


void Tree::add(Tree tree) {
    TreeNode* p= new TreeNode;
    p -> data = tree.root -> firstChild -> data;
    p -> firstChild = tree.root -> firstChild -> firstChild;
    p -> rightSibling = tree.root -> firstChild -> rightSibling;

    if (this -> root -> firstChild == NULL) {
        this -> root -> firstChild = p;
    }
}


void Tree::add(string name) {
    TreeNode* p= new TreeNode;
    p -> data = name;
    p -> firstChild = NULL;
    p -> rightSibling = NULL;
    
    if (this -> root -> firstChild == NULL) {
        this -> root -> firstChild = p;
    } else {
        TreeNode *temp = this -> root -> firstChild;
        while (temp -> rightSibling != NULL) {
            temp = temp -> rightSibling;
        }
        temp -> rightSibling = p;
    }
}


void Tree::add(string name, string data) {
    TreeNode* p= new TreeNode;
    TreeNode* q= new TreeNode;
    
    q -> data = data;
    q -> firstChild = NULL;
    q -> rightSibling = NULL;

    p -> data = name;
    p -> firstChild = q;
    p -> rightSibling = NULL;

    if (this -> root -> firstChild == NULL) {
        this -> root -> firstChild = p;
    } else {
        TreeNode *temp = this -> root -> firstChild;
        while (temp -> rightSibling != NULL) {
            temp = temp -> rightSibling;
        }
        temp -> rightSibling = p;
    }
}

Tree Tree::operator[] (string n) {
    TreeNode* tn = root -> firstChild;
    while (tn -> data != n) {
        if (tn -> rightSibling != NULL) {
            tn = tn -> rightSibling;
        } else {
            break;
        }
    }
    Tree tree;
    tree.root = tn;
    return tree;
}

Tree Tree::operator[] (int n) {
    string str = to_string(n);
    TreeNode* tn = root -> firstChild;
    while (tn -> data != str) {
        if (tn -> rightSibling != NULL) {
            tn = tn -> rightSibling;
        } else {
            break;
        }
    }
    Tree tree;
    tree.root = tn;
    return tree;
}


void Tree::preOrder(TreeNode* node) {
    if (node == NULL) return;
    cout << "\"" << node -> data << "\"";
    if (node -> firstChild == NULL) {
        //cout << " ";
    } else {
        cout << ": ";
        cout << "{";
        //if (node -> firstChild -> rightSibling != NULL || node -> firstChild -> firstChild != NULL) cout << "{";
        preOrder(node -> firstChild);
        //if (node -> firstChild -> rightSibling != NULL || node -> firstChild -> firstChild != NULL) cout << "}";
        cout << "}";
    }
    if (node -> rightSibling == NULL) {
        
    } else {
        cout << ", ";
        preOrder(node -> rightSibling);
           
    }
}

string Tree::getValue() {
    return this -> root -> firstChild -> data;
}

int Tree::size() {
    if (this -> root -> firstChild == NULL) return 0;
    int count = 1;
    TreeNode* tn = this -> root -> firstChild;
    while (tn -> rightSibling != NULL) {
        count ++;
        tn = tn -> rightSibling;
    }
    return count;
}
