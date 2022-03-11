#ifndef _TREE_H_
#define _TREE_H_
struct _Node;

typedef struct _Tree {
	void* (*cpy)(void* data);
	int (*cmp)(void* x, void* y);
	void (*free)(void* data);
	struct _Node* root;
} Tree;

int tree_insert(Tree* t, void* data);
int tree_delete(Tree* t, void* data);
int tree_walk(Tree* t, int(*cb)(void*, int, int));
int tree_search(Tree* t, void* key, int (*cb)(void*));
void tree_free(Tree* t);
#endif /* _TREE_H_ */