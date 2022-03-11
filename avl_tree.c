#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#define ABS(x) ((x)> 0 ? (x): -(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef struct _Node {
	struct _Node* l;
	struct _Node* r;
	void* data;
	int bal;
} Node;

Node* _Tr(Node* l, void* data, Node* r, int bal) {
	Node* res = NULL;
	if (bal == 2) {
		Node R = *r;
		if(R.bal < 0) {
			Node RL = *R.l;
			res = _Tr(_Tr(l, data, RL.l, MIN(-RL.bal, 0)), RL.data, _Tr(RL.r, R.data, R.r, MAX(-RL.bal, 0)), 0);
			free(R.l);
		} else
			res = _Tr(_Tr(l, data, R.l, 0), R.data, R.r, 0);
		free(r);
	} else if (bal == -2) {
		Node L = *l;
		if (L.bal > 0) {
			Node LR = *L.r;
			res = _Tr(_Tr(L.l, L.data, LR.l, MIN(-LR.bal, 0)), LR.data, _Tr(LR.r, data, r, MAX(-LR.bal, 0)), 0);
			free(l->r);
		} else
			res = _Tr(L.l, L.data, _Tr(L.r, data, r, 0), 0);
		free(l);
	} 
	if (res) return res;

	res = malloc(sizeof(Node));
	*res = (Node){l, r, data, bal};
	return res;
}

int _tree_insert(Tree *t, Node **n, void *data) {
	Node *x = *n;
	int dbal;
	if (!x) {
		*n = _Tr(NULL, t->cpy(data), NULL, 0);
		return 1;
	}
	if (t->cmp(data, x->data) <= 0)
		dbal = -_tree_insert(t, &x->l, data);
	else
		dbal = _tree_insert(t, &x->r, data);

	*n = _Tr(x->l, x->data, x->r, x->bal + dbal);
	dbal = ABS((*n)->bal) > ABS(x->bal) ? 1 : 0;
	free(x);
	return dbal;
}

int _tree_search(Tree* t, Node* n, void* key, int (*cb)(void*)) {
	int cmp;
	if(!n) return 0;
	cmp = t->cmp(key, n->data);
	if(cmp == 0)
		if(cb(n->data)) return 1;
	if (cmp <= 0)
		return _tree_search(t, n->l, key, cb);
	else
		return _tree_search(t, n->r, key, cb);
}

int tree_search(Tree* t, void* key, int(*cb)(void*)) {
	return _tree_search(t, t->root, key, cb);
}

int _tree_walk(Tree* t, Node* n, int level, int(*cb)(void*, int, int)) {
	if(!n) return 0;
	if(_tree_walk(t, n->l, level+1, cb)) return 1;
	if(cb(n->data, level, n->bal)) return 1;
	return _tree_walk(t, n->r, level+1, cb);
}

int tree_walk(Tree* t, int(*cb)(void*, int, int)) {
	return _tree_walk(t, t->root, 0, cb);
}

void _tree_free(Tree* t, Node* n) {
	Node* l, *r;
	if(!n) return;
	t->free(n->data);
	l = n->l;
	r = n->r;
	free(n);
	_tree_free(t, l);
	_tree_free(t, r);
}

void tree_free(Tree* t) {
	_tree_free(t, t->root);
}

int elect_l(Node** n, void** e) {
	Node* x = *n;
	int dbal = 1;
	if(!x->l) {
		*n = x->r;
		*e = x->data;
	} else {
		dbal = elect_l(&x->l, e);
		*n = _Tr(x->l, x->data, x->r, x->bal + dbal);
		dbal = ABS((*n)->bal) < ABS(x->bal) ? 1 : 0;
	}
	free(x);
	return dbal;
}


int elect_r(Node** n, void** e) {
	Node* x = *n;
	int dbal = 1;
	if(!x->r) {
		*n = x->l;
		*e = x->data;
	} else {
		dbal = elect_r(&x->r, e);
		*n = _Tr(x->l, x->data, x->r, x->bal - dbal);
		dbal = ABS((*n)->bal) < ABS(x->bal) ? 1 : 0;
	}
	free(x);
	return dbal;
}

int _tree_delete(Tree* t, Node** n, void* data) {
	Node* x = *n;
	int cmp, dbal;
	if(!x) return 0;
	cmp = t->cmp(data, x->data);

	printf("Comparing %d and %d => %d\n", (int)data, (int)x->data, cmp);
	if(!x->l && !x->r && !cmp) {
		printf("Deleted node found, no children\n");
		t->free(x->data);
		free(x);
		*n = NULL;
		return 1;
	}
	if(cmp < 0)
		dbal = _tree_delete(t, &x->l, data);
	else if(cmp > 0)
		dbal = -_tree_delete(t, &x->r, data);
	else {
		t->free(x->data);
		dbal = x->bal > 0 ? -elect_l(&x->r, &x->data) : elect_r(&x->l, &x->data);
	}
	*n = _Tr(x->l, x->data, x->r, x->bal + dbal);
	dbal = ABS((*n)->bal) < ABS(x->bal) ? 1 : 0;
	free(x);
	return dbal;
}

int tree_delete(Tree* t, void* data) {
	return _tree_delete(t, &t->root, data);
}

int tree_insert(Tree *t, void *data) {
	return _tree_insert(t, &t->root, data);
}