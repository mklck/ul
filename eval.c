#include <stdio.h>
#include <stdlib.h>

#include "ul.h"
#include "internal.h"

static void
err_expfn(obj *o)
{
	fprintf(stderr, "expected function, got: ");
	printobj(stderr, o);
	fprintf(stderr, "\n");
	exit(1);
}

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
		if (t == NULL) {
			fprintf(stderr, "symbol '%s' not found\n", o->data.str.str);
			t = ul_nil;
		}
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

void
check_arglist(function *f, list *args)
{
	if ((list_to_argtype(args) & f->flags) == 0) {
		fprintf(stderr, "mismatch of argument count, got: \n\t");
		printlist(stderr, args);
		fprintf(stderr, "\n");
		exit(1);
	}
}

static void *
extract_arg(enum ul_fn_flags f, list *args)
{
	if (f & UL_FN_NOARG)
		return NULL;
	else if (f & UL_FN_ONEARG)
		return args->head;
	else
		return args;
}

static obj *
run_builtin(world *w, function *f, list *args)
{
	obj * (*fn) (world *, void *);
	void *arg;

	fn = f->fn;
	arg = extract_arg(f->flags, args);

	return fn(w, arg);
}

static void
userfn_setarg(function *f, list *args)
{
	obj *arg;

	if (f->flags & UL_FN_ONEARG)
		arg = args->head;
	else if (f->flags & UL_FN_VARARG)
		arg = list_to_obj(args);

	if (f->arg_name)
		envset(f->env, f->arg_name, arg);
}

static world
new_world(world *w, function *f)
{
	world n;

	n = *w;
	n.env = f->env;
	n.self = f;

	return n;
}

static list *
eval_rest(world *w, function *f, list *l)
{
	return (f->flags & UL_FN_MACRO) ? l : eval_list(w, l);
}

static obj *
list_from_expr(world *w, obj *o, list **nl)
{
	list *l;

	if (o->type != UL_LIST)
		return eval_ast(w, o);

	l = o->data.list;

	if (l->head == NULL)
		return o;
	
	*nl = l;
	return NULL;
}

obj *
eval(world *w, obj *o)
{
	list *l;
	obj *head, *evaluated;
	function *f;
	world new_w;

	full_tco:
	w->tco_type = UL_NO_TCO;

	if ((o = list_from_expr(w, o, &l)))
		return o;

	head = eval(w, l->head);

	if (head->type != UL_FUNCTION)
		err_expfn(head);

	f = head->data.fn;
	l = eval_rest(w, f, l->rest);

	partial_tco:

	check_arglist(f, l);

	if ((f->flags & UL_FN_BUILTIN) == 0) {
		userfn_setarg(f, l);
		new_w = new_world(w, f);
		if (w->tco_type == UL_PARTIAL_TCO) {
			w = &new_w;
			o = f->fn;
			goto full_tco;
		} else {
			return eval(&new_w, f->fn);
		}
	}

	evaluated = run_builtin(w, f, l);

	if (w->tco_type == UL_NO_TCO) {
		return evaluated;
	} else if (w->tco_type == UL_FULL_TCO) {
		o = w->tco;
		goto full_tco;
	} else {
		f = w->self;
		l = w->tco;
		goto partial_tco;
	}
}