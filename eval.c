#include <stdio.h>
#include <stdlib.h>

#include "ul.h"
#include "internal.h"

static list *
eval_list(world *w, list *l)
{
	list *nl, *il;

	nl = xcalloc(sizeof(list), 1);
	il = nl;

	for (; l && l->head; l = l->rest)
		il = lappend(il, eval(w, l->head));

	return nl;
}

static obj *
eval_ast(world *w, obj *o)
{
	obj *t;

	if (o->type == UL_SYMBOL){
		t = envget(w->env, o);
		if (t == NULL)
			fprintf(stderr, "symbol '%s' not found\n", o->data.str.str);
	} else if (o->type == UL_LIST) {
		t = xmalloc(sizeof(obj));
		t->type = UL_LIST;
		t->data.list = eval_list(w, o->data.list);
	} else {
		t = o;
	}
	return t;
}

static int
list_to_argtype(list *l)
{
	if (l->head == NULL)
		return UL_FN_NOARG | UL_FN_VARARG;
	else if (l->rest == NULL)
		return UL_FN_ONEARG | UL_FN_VARARG;
	else
		return UL_FN_VARARG;
}

static obj *
run_builtin(world *w, function *f, list *args)
{
	obj * (*fn) (world *, void *);

	if ((list_to_argtype(args) & f->flags) == 0) {
		fprintf(stderr, "mismatch of argument count, got: \n\t");
		printlist(stderr, args);
		fprintf(stderr, "\n");
		exit(1);
	}

	fn = f->fn;
	if (f->flags & UL_FN_NOARG)
		return fn(w, NULL);
	else if (f->flags & UL_FN_ONEARG)
		return fn(w, args->head);
	else
		return fn(w, args);
}

static obj *
run_userdefined(world *w, function *f, list *args)
{
	return NULL;
}

obj *
eval(world *w, obj *o)
{
	list *l;
	obj *head;
	function *f;

	if (o->type != UL_LIST)
		return eval_ast(w, o);

	l = o->data.list;

	if (l->head == NULL)
		return o;

	head = eval_ast(w, l->head);

	if (head->type != UL_FUNCTION) {
		fprintf(stderr, "expected function\n");
		exit(1);
	}

	f = head->data.fn;

	if ((f->flags & UL_FN_MACRO) == 0)
		l = eval_list(w, l->rest); 
	else
		l = l->rest;

	if (f->flags & UL_FN_BUILTIN)
		return run_builtin(w, f, l);
	else
		return run_userdefined(w, f, l);
}
