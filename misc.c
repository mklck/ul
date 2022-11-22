#include <stdlib.h>
#include <stdio.h>

#include "ul.h"
#include "internal.h"

void *
err_mem(void)
{
	fprintf(stderr, "Out of memory\n");
	exit(1);
	return NULL;
}

void *
xmalloc(size_t sz)
{
	void *ptr;

	ptr = malloc(sz);
	return ptr ? ptr : err_mem();
}

void *
xrealloc(void *p, size_t sz)
{
	p = realloc(p, sz);

	return p ? p : err_mem();
}

void *
xcalloc(size_t sz, size_t n)
{
	void *ptr;

	ptr = calloc(sz, n);

	return ptr ? ptr : err_mem();
}

void
printobj(FILE *f, obj *o)
{
	if (o->type == UL_INT)
		fprintf(f, "%d", o->data.i);
	else if (o->type == UL_SYMBOL)
		fprintf(f, "%s", o->data.str.str);
	else if (o->type == UL_LIST)
		printlist(f, o->data.list);
	else if (o->type == UL_NIL)
		fprintf(f, "nil");
	else if (o->type == UL_TRUE)
		fprintf(f, "true");
	else
		fprintf(f, "[unrecognized type: %d]", o->type);

	fflush(f);
}

void
printlist(FILE *f, list *l)
{
	fprintf(f, "(");

	for (; l && l->head; l = l->rest) {
		printobj(f, l->head);
		if (l->rest)
			fprintf(f, " ");
	}

	fprintf(f, ")");
	fflush(f);
}
