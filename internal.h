
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
#define lappend ul_lappend
#define list_nth ul_list_nth

#define envget ul_envget
#define envset ul_envset
#define envnew ul_envnew
#define envcopy ul_envcopy
#define envcopyall ul_envcopyall

#define treeget ul_treeget
#define treeset ul_treeset
#define treecopy ul_treecopy

void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void *, size_t);

void printobj(FILE *, obj *);
void printlist(FILE *, list *);

int list_szlet(list *, int);
int list_szget(list *, int);
