#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ul.h"
#include "internal.h"

#define LIST_NULL_OR_EMPTY(l) (((l) == NULL) || ((l)->head == NULL))

static obj _ul_nil = {.type = UL_NIL};
static obj _ul_true = {.type = UL_TRUE};

obj *ul_nil = &_ul_nil, *ul_true = &_ul_true;

list *
list_append(list *l, void *o)
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

list *
list_cons(list *l, void *o)
{
	list *n;

	if (l->head) {
		n = xmalloc(sizeof(list));
		n->head = o;
		n->rest = l;
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
		if (a.str[t] < b.str[t])
			return 1;
		else if (a.str[t] > b.str[t])
			return -1;
	
	return 0; 
}

int
list_cmp(list *a, list *b)
{
	int t;

	for (; a && a->head; a = a->rest) {
		if (b && b->head) {
			t = objcmp(a->head, b->head);
			if (t)
				return t;
		} else {
			break;
		}
		b = b->rest;
	}

	if (LIST_NULL_OR_EMPTY(a) && LIST_NULL_OR_EMPTY(b))
		return 0;
	else if (b)
		return -1;
	else 
		return 1;
}

list *
list_copy(list *l)
{
	list *n, *ni;

	n = xcalloc(sizeof(list), 1);
	ni = n;

	for (; l && l->head; l = l->rest)
			ni = list_append(ni, l->head);

	return n;
}

list *
list_end(list *l)
{
	for (; l && l->head; l = l->rest)
		if (l->rest == NULL)
			return l;
	return l;
}

int
objcmp(obj *a, obj *b)
{

	if (a->type != b->type)
		return a->type - b->type;

	if (a->type == UL_SYMBOL)
		return xstrcmp(a->data.str, b->data.str);
	else if (a->type == UL_INT)
		return b->data.i - a->data.i;
	else if (a->type == UL_LIST)
		return list_cmp(a->data.list, b->data.list);
	else
		return 0;
}

obj *
treeget(tree *t, obj *k)
{
	int c;

	tco:

	if ((t == NULL) || (t->key == NULL))
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

tree *
treecopy(tree *t)
{
	tree *n;

	if (t == NULL)
		return NULL;

	n = xmalloc(sizeof(tree));
	n->key = t->key;
	n->val = t->val;

	n->l = treecopy(t->l);
	n->r = treecopy(t->r);

	return n;
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

env *
envcopy(env *e)
{
	env *n;

	n = xmalloc(sizeof(env));
	n->head = treecopy(e->head);
	n->rest = NULL;

	return n;
}

env *
envcopyall(env *e)
{
	env *n, *i;

	for (n = NULL; e && e->head; e = e->rest)
		if (n) {
			i->rest = envcopy(e);
			i = i->rest;
		} else {
			n = envcopy(e);
			i = n;
		}

	return n;
}

list *
list_nth(list *l, int i)
{
	for (; l && l->head; l = l->rest)
		if (i-- == 0)
			return l;
	return NULL;
}

int
list_size(list *l)
{
	int t;

	for (t = 0; l && l->head; l = l->rest)
		t++;

	return t;
}

int
list_nsize(list *l, int n)
{
	int t;

	for (t = 0; l && l->head; l = l->rest)
		if (++t > n)
			return n;

	return t;
}

obj *
list_to_obj(list *l)
{
	obj *o;

	o = xmalloc(sizeof(obj));
	o->type = UL_LIST;
	o->data.list = l;

	return o;
}
