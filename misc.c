#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ul.h"
#include "internal.h"

#define SCANLIST_CHECKTYPE(c, o, t, p) \
	case (c): \
	if ((o)->type != (t)) \
		return -1; \
	*(p) = (o); \
	break;

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

static int
scanlist_sz(char *s)
{
	int l;

	for (l = 0; *s; s++) {
		l++;
		if (strchr("lsifnta", *s) == NULL)
			return -1;
	}
	return l;
}

int
scanlist(list *l, char *fmt, ...)
{
	va_list ptrs;
	int sz, i;
	obj *t, **o;

	sz = scanlist_sz(fmt);

	if (sz != list_size(l))
		return -1;

	va_start(ptrs, fmt);

	for (i = 0; l && l->head; l = l->rest) {
		t = l->head;
		o = va_arg(ptrs, obj**);
		switch (fmt[i]) {
			SCANLIST_CHECKTYPE('l', t, UL_LIST, o)
			SCANLIST_CHECKTYPE('s', t, UL_SYMBOL, o)
			SCANLIST_CHECKTYPE('i', t, UL_INT, o)
			SCANLIST_CHECKTYPE('f', t, UL_FUNCTION, o)
			SCANLIST_CHECKTYPE('n', t, UL_NIL, o)
			SCANLIST_CHECKTYPE('t', t, UL_TRUE, o)
			case 'a':
				*o = t;
				break;
		}
		i++;
	}

	va_end(ptrs);
	return 0;
}
