/* header guard generate by genheader.pl */
#ifndef _atom_h_
#define _atom_h_

#include <defines.h>

#define ATOM_TYPE_UNKNOWN   0
#define ATOM_TYPE_INT       1
#define ATOM_TYPE_UINT      2
#define ATOM_TYPE_STRING    3
#define ATOM_TYPE_BOOLEAN   4
#define ATOM_TYPE_LIST      5
#define ATOM_TYPE_FUNCTION  6
#define ATOM_TYPE_TOKEN     7

struct _Atom {
  int type;
  int is_ref;
  union {
    long* i32;            /* 1 */
    unsigned long* ui32;  /* 2 */
    char* string;         /* 3 */
    char* boolean;        /* 4 */
    List* list;           /* 5 */
    Function* function;   /* 6 */
    char* token;          /* 7 */
  };
};

Atom* atom_new(void);
void atom_destroy(Atom* a);

Atom* atom_duplicate(const Atom* a);

void atom_reset(Atom* a);
/*
  return value:
   -1:  symbol not found in scope
   -2:  atom is not token type
    0:  atom seted to reference of special token
 */
int atom_token_to_refrence(Atom* a, const Scope* scope);

Atom* atom_reference(const Atom* ref);

Atom* atom_new_unknown(void);
Atom* atom_new_int(long value);
Atom* atom_new_uint(unsigned long value);
/* str will be duplicate  */
Atom* atom_new_string(const char* str);
Atom* atom_new_boolean(char boolean);
/* list will be duplicated */
Atom* atom_new_list(const List* list);
/* func will be duplicated */
Atom* atom_new_function(const Function* func);
/* token name will be duplicated */
Atom* atom_new_token(const char* token_name);

void atom_destroy_string(char* str);
void atom_destroy_token(char* token);
void atom_destroy_function(Function* func);
void atom_destroy_list(List* list);

void atom_set_unknown(Atom* atom);
void atom_set_int(Atom* atom, long value);
void atom_set_uint(Atom* atom, unsigned long value);
void atom_set_string(Atom* atom, const char* str);
void atom_set_boolean(Atom* atom, char boolean);
void atom_set_list(Atom* atom, const List* list);
void atom_set_function(Atom* atom, const Function* func);
void atom_set_token(Atom* atom, const char* token_name);

/* will duplicate new_val */
void atom_set_atom(Atom* atom, const Atom* new_val);
void atom_set_reference(Atom* atom, const Atom* reference);


/* this group of functions DO NOT allocate new memory */
/*
  return  0 for successed,
         -1 for incompact type;
 */
int atom_get_int(Atom* atom, long* i32);
int atom_get_uint(Atom* atom, unsigned* ui32);
int atom_get_string(Atom* atom, char** string);
int atom_get_boolean(Atom* atom, char* boolean);
int atom_get_list(Atom* atom, List** list);
int atom_get_function(Atom* atom, Function** function);
int atom_get_token(Atom* atom, char** token_name);

int atom_eval(Atom* atom, Scope* scope);
void atom_dereference_token(Atom* atom, const  Scope* scope);

Atom* atom_parse_string(const char* str);

void atom_show(const Atom* atom, char** str);
void atom_show_debug(const Atom* atom, char** str);

/*
  return 0 for equal,
  1 for great,
  -1 for less,
  2 for non equal(cannot compare size)
  -2 for type error
*/
int atom_compare(const Atom* a1, const Atom* a2);


#endif /* _atom_h_ */
