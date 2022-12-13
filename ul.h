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

enum ul_tco_type {
	UL_NO_TCO,
	UL_FULL_TCO,
	UL_PARTIAL_TCO
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

/* struct ul_function
	void *fn
		If function is builtin, its pointer to function
		obj * (*fn)(world *, TYPE), where type is:
			- obj * when f takes one argument
			- list * when f takes many arguments
			- void * set to NULL, when no argument is passed
		If function is defined by programmer, fn points to
		function body of type obj *
	ul_obj *arg_name
		Symbol with name of the argument passed to user function.
		NULL if function does not take any arguments.
	ul_env *env
		Copy of the environment at the moment of function creation.
*/

struct ul_function {
	void *fn;
	enum ul_fn_flags flags;
	ul_obj *arg_name;
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
	ul_function *self;

	void *tco;
	enum ul_tco_type tco_type;
};

extern ul_obj *ul_nil, *ul_true;

int ul_objcmp(ul_obj *, ul_obj *);
int ul_listcmp(ul_list *, ul_list *);

ul_obj* ul_eval(ul_world *, ul_obj *);
ul_obj* ul_parse(ul_parser *);

ul_list* ul_list_append(ul_list *, void *);
ul_list* ul_list_nth(ul_list *, int);
ul_list* ul_list_copy(ul_list *);
ul_list* ul_list_end(ul_list *);
ul_list* ul_list_cons(ul_list *, void *);
int ul_list_size(ul_list *);
int ul_list_nsize(ul_list *, int);

ul_obj* ul_envget(ul_env *, ul_obj *);
ul_obj* ul_envset(ul_env *, ul_obj *, ul_obj *);
ul_env* ul_envnew(ul_env *);
ul_env* ul_envcopy(ul_env *);
ul_env* ul_envcopyall(ul_env *);

ul_obj* ul_treeget(ul_tree *, ul_obj*);
ul_obj* ul_treeset(ul_tree *, ul_obj*, ul_obj*);
ul_tree* ul_treecopy(ul_tree *);

ul_obj* ul_core_head   (ul_world *, ul_obj  *);
ul_obj* ul_core_rest   (ul_world *, ul_obj  *);
ul_obj* ul_core_len    (ul_world *, ul_obj  *);
ul_obj* ul_core_cons   (ul_world *, ul_list *);
ul_obj* ul_core_append (ul_world *, ul_list *);
ul_obj* ul_core_nth    (ul_world *, ul_list *);
ul_obj* ul_core_list   (ul_world *, ul_list *);

ul_obj* ul_core_add    (ul_world *, ul_list *);
ul_obj* ul_core_sub    (ul_world *, ul_list *);
ul_obj* ul_core_lt     (ul_world *, ul_list *);
ul_obj* ul_core_gt     (ul_world *, ul_list *);
ul_obj* ul_core_leqt   (ul_world *, ul_list *);
ul_obj* ul_core_geqt   (ul_world *, ul_list *);

ul_obj* ul_core_eq     (ul_world *, ul_list *);
ul_obj* ul_core_cmp    (ul_world *, ul_list *);
ul_obj* ul_core_not    (ul_world *, ul_obj *);
ul_obj* ul_core_and    (ul_world *, ul_list *);
ul_obj* ul_core_or     (ul_world *, ul_list *);

ul_obj* ul_core_typeof (ul_world *, ul_obj *);
ul_obj* ul_core_quote  (ul_world *, ul_obj *);
ul_obj* ul_core_def    (ul_world *, ul_list *);
ul_obj* ul_core_if     (ul_world *, ul_list *);
ul_obj* ul_core_let    (ul_world *, ul_list *);
ul_obj* ul_core_do     (ul_world *, ul_list *);
ul_obj* ul_core_lambda (ul_world *, ul_list *);
ul_obj* ul_core_recur  (ul_world *, ul_list *);


/* ul_scanlist
	Scan fixed length list and extract objects into corresponding type pointers.
	fmt is string which contains characters:
	- l for list
	- s for symbol
	- i for integer
	- f for function
	- n for nil
	- t for true
	- a for any
	For example "li" means that list must have as first argument list, and as second
	argument int. List can not have more elements.

*/
int ul_scanlist(ul_list *, char *, ...);

#endif
