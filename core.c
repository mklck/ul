#include <stdio.h>
#include <stdlib.h>

#include "ul.h"
#include "internal.h"

static void
err_invtype(enum ul_obj_type exp, enum ul_obj_type got)
{
	fprintf(stderr, "invalid data type");
	exit(1);
}

obj *
ul_core_add(world *w, list *l)
{
	int t = 0;
	obj *o;

	for (; l && l->head; l = l->rest){
		o = l->head;
		if (o->type != UL_INT)
			err_invtype(UL_INT, o->type);
		else
			t += o->data.i; 
	}
	o = xmalloc(sizeof(obj));
	o->type = UL_INT;
	o->data.i = t;

	return o;
}

obj *
ul_core_quote(world *w, obj *o)
{
	return o;
}

obj *
ul_core_let(world *w, list *l)
{
	obj *t = NULL;

	w->env = envnew(w->env);

	for (; l && l->head; l = l->rest)
		if (t) {
			envset(w->env, t, l->head);
			t = NULL;
		} else
			t = l->head;

	if (t == NULL) {
		fprintf(stderr,  "Expected expression\n");
		exit(1);
	}

	return eval(w, t);
}

obj *
ul_core_def(world *w, list *l)
{
	obj *k, *v;

	k = l->head;
	v = l->rest->head;

	if (l->rest->rest) {
		fprintf(stderr, "Too much arguments\n");
		exit(1);
	}

	v = eval(w, v);

	envset(w->env, k, v);
	return v;
}

obj *
ul_core_do(world *w, list *l)
{
	obj *t;

	for (; l && l->head; l= l->rest)
		t = eval(w, l->head);

	return t;
}

obj *
ul_core_if(world *w, list *l)
{
	obj *cond, *tf, *ff;
	list *t;

	t = list_nth(l, 1);
	if (t) {
		if (t->rest && t->rest->rest)
			goto error;
	} else {
		goto error;
	}

	cond = l->head;
	tf = t->head;
	ff = t->rest ? t->rest->head : NULL;

	cond = eval(w, cond);

	if (cond->type != UL_NIL)
		return eval(w, tf);
	else
		return ff ? eval(w, ff) : ul_nil;

	error:
		fprintf(stderr, "Argument count mismatch\n");
		exit(1);
}

static int
arg_to_fn(obj *o, function *f)
{

	if (o->type == UL_SYMBOL){
		f->arg_name = o;
		f->flags |= UL_FN_ONEARG;
		return 0;
	}
	if (o->type != UL_LIST && o->data.list->rest)
		return !0;

	o = o->data.list->head;

	if (o) {
		if (o->type == UL_SYMBOL) {
			f->flags |= UL_FN_VARARG;
			f->arg_name = o;
			return 0;
		}
		else {
			return !0;
		}
	} else { 
		f->flags |= UL_FN_NOARG;
		f->arg_name = NULL;
		return 0;
	}
}

obj *
ul_core_lambda(world *w, list *l)
{
	obj *arg, *body, *o;
	function *f;

	if ((l == NULL) || (l->rest == NULL) || (l->rest->rest)) {
		fprintf(stderr, "expected 2 arguments\n");
		exit(1);
	}

	arg = l->head;
	body = l->rest->head;

	f = xcalloc(sizeof(function), 1);
	arg_to_fn(arg, f);
	f->fn = body;
	f->env = envcopyall(w->env);

	o = xmalloc(sizeof(obj));
	o->type = UL_FUNCTION;
	o->data.fn = f;

	return o;
}
