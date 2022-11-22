#ifndef UL_HEADER
#define UL_HEADER

typedef struct ul_obj ul_obj;
typedef struct ul_parser ul_parser;
typedef struct ul_string ul_string;
typedef struct ul_list ul_list;
typedef struct ul_function ul_function;
typedef struct ul_world ul_world;
typedef struct ul_env ul_env;
typedef struct ul_tree ul_tree;

enum ul_obj_type {
	UL_LIST = 1 << 0,
	UL_INT = 1 << 1,
	UL_SYMBOL = 1 << 2,
	UL_FUNCTION = 1 << 3,
	UL_NIL = 1 << 4,
	UL_TRUE = 1 << 5
};

enum ul_fn_flags {
	UL_FN_NOARG  = 1 << 0,
	UL_FN_ONEARG = 1 << 1,
	UL_FN_VARARG = 1 << 2,

	UL_FN_BUILTIN = 1 << 3,
	UL_FN_MACRO   = 1 << 4
};


struct ul_string {
	unsigned sz;
	char *str;
};

union ul_obj_data {
	int i;
	ul_string str;
	ul_function *fn;
	ul_list *list;
};

struct ul_obj {
	enum ul_obj_type type;
	union ul_obj_data data;
};

struct ul_list {
	void *head;
	ul_list *rest;
};

struct ul_function {
	void *fn;
	enum ul_fn_flags flags;
	ul_env *env;
};

struct ul_env {
	ul_tree *head;
	ul_env *rest;
};

struct ul_tree {
	ul_obj *key, *val;
	ul_tree *l, *r;
};

struct ul_parser {
	int (*next) (void *);
	void *priv;
	char buf;
};

struct ul_world {
	ul_env *env;
};

extern ul_obj *ul_nil, *ul_true;

ul_obj* ul_eval(ul_world *, ul_obj *);
ul_obj* ul_parse(ul_parser *);

ul_list* ul_lappend(ul_list *, void *);
ul_list* ul_list_nth(ul_list *, int);

ul_obj* ul_envget(ul_env *, ul_obj *);
ul_obj* ul_envset(ul_env *, ul_obj *, ul_obj *);
ul_env* ul_envnew(ul_env *);

ul_obj* ul_treeget(ul_tree *, ul_obj*);
ul_obj* ul_treeset(ul_tree *, ul_obj*, ul_obj*);

ul_obj* ul_core_add   (ul_world *, ul_list *);
ul_obj* ul_core_quote (ul_world *, ul_obj *);
ul_obj* ul_core_def   (ul_world *, ul_list *);
ul_obj* ul_core_if    (ul_world *, ul_list *);
ul_obj* ul_core_let   (ul_world *, ul_list *);
ul_obj* ul_core_do    (ul_world *, ul_list *);

#endif
