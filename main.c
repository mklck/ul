#include <stdio.h>
#include <string.h>

#include "ul.h"
#include "internal.h"

typedef struct {
	char *name;
	void *ptr;
	enum ul_fn_flags flags;
} builtin_fn;

builtin_fn builtins[] = {
	{"+",      ul_core_add,    UL_FN_VARARG},
	{"-",      ul_core_sub,    UL_FN_VARARG},
	{"<",      ul_core_lt,     UL_FN_VARARG},
	{">",      ul_core_gt,     UL_FN_VARARG},
	{"<=",     ul_core_leqt,   UL_FN_VARARG},
	{">=",     ul_core_geqt,   UL_FN_VARARG},
	{"quote",  ul_core_quote,  UL_FN_ONEARG | UL_FN_MACRO},
	{"def",    ul_core_def,    UL_FN_VARARG | UL_FN_MACRO},
	{"if",     ul_core_if,     UL_FN_VARARG | UL_FN_MACRO},
	{"let",    ul_core_let,    UL_FN_VARARG | UL_FN_MACRO},
	{"do",     ul_core_do,     UL_FN_VARARG | UL_FN_MACRO},
	{"lambda", ul_core_lambda, UL_FN_VARARG | UL_FN_MACRO},
	{0}
};

void
init_world(world *w)
{
	env *e;
	obj *k, *v;
	builtin_fn *i;

	e = envnew(NULL);

	for (i = &builtins[0]; i->name; i++) {
		k = xmalloc(sizeof(obj));
		k->type = UL_SYMBOL;
		k->data.str.str = i->name;
		k->data.str.sz = strlen(i->name);

		v = xmalloc(sizeof(obj));
		v->data.fn = xmalloc(sizeof(function));
		v->type = UL_FUNCTION;
		v->data.fn->fn = i->ptr;
		v->data.fn->flags = i->flags | UL_FN_BUILTIN;

		envset(e, k, v);
	}

	w->env = e;
}

void
printtree(tree *t)
{
	printobj(stderr, t->key);
	fprintf(stderr, " ");
	printobj(stderr, t->val);
	fprintf(stderr, "\n");
	if (t->l)
		printtree(t->l);
	if (t->r)
		printtree(t->r);
}

int
main(int argc, char **argv)
{
	parser p;
	obj *o;
	world w;

	p.next = (int (*)(void *)) getc;
	p.priv = stdin;
	init_world(&w);

/*	printtree(w.env->head); */

	while (!feof(stdin)) {
		printf(">> ");
		o = parse(&p);
		o = eval(&w, o);
		printobj(stdout, o);
		puts("");
	}
	return 0;
}
