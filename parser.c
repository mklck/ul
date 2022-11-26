#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "ul.h"
#include "internal.h"

/* Assumes that p is parser *p */
#define SINGLE_CHAR_TKN(c, t) if (p->buf == (c)) { next(p); \
	 return (token) {.type = (t)}; }

enum token_type {
	TKN_INT,
	TKN_LIST_START,
	TKN_LIST_END,
	TKN_SYMBOL,
	TKN_NIL,
	TKN_TRUE,
	TKN_EOF
};

typedef struct {
	char *str;
	int sz;
	enum token_type type;
} token;

obj* parse_obj(parser *, token);

static char
next(parser *p)
{
	p->buf = p->next(p->priv);
	return p->buf;
}

static void
skip_space(parser *p)
{
	while(isspace(p->buf))
		next(p);
}

static void
tkn_cappend(token *t, char c)
{
	if (t->str == NULL){
		t->sz = 1;
		t->str = xmalloc(2);
	} else {
		t->sz++;
		t->str = xrealloc(t->str, t->sz+1);
	}
	t->str[t->sz - 1] = c;
	t->str[t->sz] = 0;
}

static void
lex_int(parser *p, token *t)
{

	t->type = TKN_INT;

	for (; isdigit(p->buf); next(p))
		tkn_cappend(t, p->buf);
}

static int
issymbol(int c)
{
	char *s = "=+-/*<>";

	if (isalpha(c))
		return !0;

	for (; *s; s++)
		if (*s == c)
			return !0;
	return 0;
}

static void
lex_symbol(parser *p, token *t)
{

	t->type = TKN_SYMBOL;

	for (;issymbol(p->buf); next(p))
		tkn_cappend(t, p->buf);

	if ((t->sz == 3) && (strcmp(t->str, "nil") == 0)) {
		free(t->str);
		t->type = TKN_NIL;
	} else if ((t->sz == 4) && (strcmp(t->str, "true") == 0)) {
		free(t->str);
		t->type = TKN_TRUE;
	}
}

static token
lex(parser *p)
{
	token t = {0};
	skip_space(p);

	SINGLE_CHAR_TKN('(', TKN_LIST_START)
	SINGLE_CHAR_TKN(')', TKN_LIST_END)

	if (p->buf == '-') {
		tkn_cappend(&t, '-');
		next(p);
	}

	if (isdigit(p->buf))
		lex_int(p, &t);
	else
		lex_symbol(p, &t);

	return t;
}

list *
parse_list(parser *p)
{
	token t;
	list *l, *il;

	l = xcalloc(sizeof(list), 1);
	il = l;

	for (t = lex(p); t.type != TKN_LIST_END; t = lex(p))
		il = lappend(il, parse_obj(p, t));

	return l;
}

obj *
parse_obj(parser *p, token t)
{
	obj *o;

	o = xmalloc(sizeof(obj));

	if (t.type == TKN_INT) {
		o->type = UL_INT;
		sscanf(t.str, "%d", &o->data.i);
		free(t.str);
	} else if (t.type == TKN_SYMBOL) {
		o->type = UL_SYMBOL;
		o->data.str.str = t.str;
		o->data.str.sz = t.sz;
	} else if (t.type == TKN_LIST_START) {
		o->type = UL_LIST;
		o->data.list = parse_list(p);
	} else if (t.type == TKN_NIL) {
		return ul_nil;
	} else if (t.type == TKN_TRUE) {
		return ul_true;
	} else {
		/* todo some error */
	}

	return o;
}

obj *
parse(parser *p)
{
	token t;

	next(p);

	t = lex(p);

	return parse_obj(p, t);
}
