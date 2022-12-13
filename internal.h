
typedef ul_obj obj;
typedef ul_parser parser;
typedef ul_string string;
typedef ul_list list;
typedef ul_function function;
typedef ul_world world;
typedef ul_env env;
typedef ul_tree tree;

#define eval ul_eval
#define parse ul_parse
#define lappend ul_list_append
#define list_append ul_list_append
#define list_nth ul_list_nth
#define list_copy ul_list_copy
#define list_end ul_list_end
#define list_cons ul_list_cons

#define objcmp ul_objcmp
#define listcmp ul_listcmp
#define list_size ul_list_size
#define list_nsize ul_list_nsize

#define envget ul_envget
#define envset ul_envset
#define envnew ul_envnew
#define envcopy ul_envcopy
#define envcopyall ul_envcopyall

#define treeget ul_treeget
#define treeset ul_treeset
#define treecopy ul_treecopy

#define scanlist ul_scanlist

void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void *, size_t);

void printobj(FILE *, obj *);
void printlist(FILE *, list *);

int list_szlet(list *, int);
int list_szget(list *, int);

obj *list_to_obj(list *);

void check_arglist(function *f, list *);
