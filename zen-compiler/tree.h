// Author: Tenton Lien
// 10/7/2018
#include <iostream>
#include <string>
#include <map>
#include "zc.h"

using namespace std;

struct TreeNode {
	string data;
	TreeNode * firstChild, * rightSibling;
};


// Custom tree data structure
class Tree {
	public:
		Tree();
		~Tree() {release(root);}
        void add(string);
        void add(int n) {add(to_string(n));}
        void add(string, string);
        void add(Tree tree);
        void release(TreeNode*);
		void preOrder() {preOrder(root);}
        void preOrder(TreeNode*);
        string getValue();
        int size();
        Tree operator[] (string);
        Tree operator[] (int);
        TreeNode* root = new TreeNode;
        //const TreeNode* operator[] (size_t) const;
	private:
		//TreeNode* currentNode = new TreeNode;
		
		

};