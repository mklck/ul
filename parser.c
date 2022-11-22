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

static token
lex_int(parser *p)
{
	token t = {.type = TKN_INT, .str = NULL};

	tkn_cappend(&t, p->buf);

	while (isdigit(next(p)))
		tkn_cappend(&t, p->buf);

	return t;
}

static token
lex_symbol(parser *p)
{
	token t = {.type = TKN_SYMBOL, .str = NULL};

	tkn_cappend(&t, p->buf);

	while (isalpha(next(p)))
		tkn_cappend(&t, p->buf);

	if ((t.sz == 3) && (strcmp(t.str, "nil") == 0)) {
		free(t.str);
		t.type = TKN_NIL;
	} else if ((t.sz == 4) && (strcmp(t.str, "true") == 0)) {
		free(t.str);
		t.type = TKN_TRUE;
	}

	return t;
}

static token
lex(parser *p)
{
	skip_space(p);

	SINGLE_CHAR_TKN('(', TKN_LIST_START)
	SINGLE_CHAR_TKN(')', TKN_LIST_END)

	if (isdigit(p->buf) || (p->buf == '-'))
		return lex_int(p);
	else
		return lex_symbol(p);

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
