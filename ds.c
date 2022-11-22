#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ul.h"
#include "internal.h"

static obj _ul_nil = {.type = UL_NIL};
static obj _ul_true = {.type = UL_TRUE};

obj *ul_nil = &_ul_nil, *ul_true = &_ul_true;

list *
lappend(list *l, void *o)
{
	list *n;

	if (l->head) {
		n = xcalloc(sizeof(list), 1);
		l->rest = n;
		n->head = o;
		return n;
	} else {
		l->head = o;
		return l;
	}
}

static int
xstrcmp(string a, string b)
{
	int t;

	t = a.sz - b.sz;
	if (t)
		return t;

	for (t = 0; t < a.sz; t++)
		if (a.str[t] > b.str[t])
			return 1;
		else if (a.str[t] < b.str[t])
			return -1;
	
	return 0; 
}

static int
objcmp(obj *a, obj *b)
{

	if (a->type != b->type)
		return a->type - b->type;

	if (a->type == UL_SYMBOL)
		return xstrcmp(a->data.str, b->data.str);
	else
		return 0;
}

obj *
treeget(tree *t, obj *k)
{
	int c;

	tco:

	if (t == NULL)
		return NULL;

	c = objcmp(t->key, k);
	if (c == 0)
		return t->val;

	if (c < 0)
		t = t->l;
	else
		t = t->r;
	goto tco;
}

obj *
treeset(tree *t, obj *k, obj *v)
{

	int c;
	
	tco:

	if (t->key == NULL) {
		t->key = k;
		return t->val = v;
	}
	
	c = objcmp(t->key, k);
	if (c == 0)
		return t->val = v;

	if (c < 0) {
		if (t->l == NULL)
			t->l = xcalloc(sizeof(tree), 1);
		t = t->l;
	} else {
		if (t->r == NULL)
			t->r = xcalloc(sizeof(tree), 1);
		t = t->r;
	}
	goto tco;
}

env *
envnew(env *old)
{
	env *n;

	n = xcalloc(sizeof(env), 1);
	if (old)
		n->rest = old;
	n->head = xcalloc(sizeof(tree), 1);

	return n;
}

obj *
envget(env *e, obj *k)
{
	obj *t;

	for (; e && e->head; e = e->rest) {
		t = treeget(e->head, k);
		if (t)
			return t;
	}
	return NULL;
}

obj *
envset(env *e, obj *k, obj *v)
{
	return treeset(e->head, k, v);
}

list *
list_nth(list *l, int i)
{
	for (; l && l->head; l = l->rest)
		if (i-- == 0)
			return l;
	return NULL;
}