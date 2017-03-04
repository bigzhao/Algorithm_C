#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue> 

struct TreeNode {
	struct TreeNode *left = NULL;
	struct TreeNode *right = NULL;
	int val;
};

void input_tree(struct TreeNode ***nodes, int *nodes_num)
{
	
	printf("please input nodes number");
	scanf("%d", nodes_num);
	char inp[100];
	struct TreeNode *node = NULL;
	*nodes = (struct TreeNode **)malloc(sizeof(struct TreeNode*) * (*nodes_num));
	for (int i = 0; i < *nodes_num; i++) {
		scanf("%s", inp);
		if (strcmp(inp, "null") == 0) {
			(*nodes)[i] = NULL;
		}
		else {
			(*nodes)[i] = (struct TreeNode *)malloc(sizeof(struct TreeNode));
			(*nodes)[i]->val = atoi(inp);
		}
	}
}

void bulid_tree(struct TreeNode **nodes, int nodes_num)
{
	std::queue<struct TreeNode *>q;
	struct TreeNode *cur = nodes[0];
	int count = 0;
	for (int i = 1; i < nodes_num; i++) {
		if (count == 2) {
			count = 0;
			cur = q.front();
			q.pop();
		}
		if (NULL != nodes[i])
			q.push(nodes[i]);
		if (count == 0)
			cur->left = nodes[i];
		else if (1 == count)
			cur->right = nodes[i];
		count++;
	}
	while (!q.empty()) {
		cur = q.front();
		cur->left = NULL;
		cur->right = NULL;
		q.pop();
	}
}

void free_tree(struct TreeNode **nodes, int nodes_num)
{
	for (int i = 0; i < nodes_num; i++)
		free(nodes[i]);
	free(nodes);
}

void inorder(struct TreeNode *root)
{
	if (NULL == root)
		return;
	inorder(root->left);
	printf("%d ", root->val);
	inorder(root->right);
	return;
}

void morris_inorder(struct TreeNode *root)
{
	struct TreeNode *node = root, *pred = NULL; 
	while (NULL != node) {
		if (NULL == node->left) {
			printf("%d ", node->val);
			node = node->right;
			continue;
		}
		pred = node->left;
		while (NULL != pred->right && node != pred->right) {
			pred = pred->right;
		}
		if (NULL == pred->right) {
			pred->right = node;
			node = node->left;
		}
		else {
			printf("%d ", node->val);
			node = node->right;
			pred->right = NULL;
		}
	}
}

void preorder(struct TreeNode *root)
{
	if (NULL == root)
		return;
	printf("%d ", root->val);
	preorder(root->left);
	preorder(root->right);
}

void morris_preorder(struct TreeNode *root)
{
	struct TreeNode *node = root, *pred = NULL;
	while (NULL != node) {
		if (NULL == node->left) {
			printf("%d ", node->val);
			node = node->right;
			continue;
		}
		pred = node->left;
		while (NULL != pred->right && node != pred->right) {
			pred = pred->right;
		}
		if (NULL == pred->right) {
			printf("%d ", node->val);
			pred->right = node;
			node = node->left;
		}
		else {
			node = node->right;
			pred->right = NULL;
		}
	}
}

int main() {
	int nodes_num;
	struct TreeNode **nodes = NULL;
	input_tree(&nodes, &nodes_num);
	bulid_tree(nodes, nodes_num);
	printf("\ninorder:");
	inorder(nodes[0]);
	printf("\nmorris inorder:");
	morris_inorder(nodes[0]);
	printf("\npreorder:");
	preorder(nodes[0]);
	printf("\nmorrispreorder:");
	morris_preorder(nodes[0]);
	free_tree(nodes, nodes_num);
	return 0;
}