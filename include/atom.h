/* header guard generate by genheader.pl */
#ifndef _atom_h_
#define _atom_h_

#include <defines.h>

#define ATOM_TYPE_UNKOWN    0
#define ATOM_TYPE_INT       1
#define ATOM_TYPE_UINT      2
#define ATOM_TYPE_STRING    3
#define ATOM_TYPE_BOOLEAN   4
#define ATOM_TYPE_LIST      5
#define ATOM_TYPE_FUNCTION  6

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
  };
};

Atom* atom_new(void);
void atom_destroy(Atom* a);
Atom* atom_duplicate(const Atom* a);
void atom_reset(Atom* a);

Atom* atom_reference(Atom* ref);

Atom* atom_new_unknow(void);
Atom* atom_new_int(long value);
Atom* atom_new_uint(unsigned long value);
/* str will be duplicate  */
Atom* atom_new_string(const char* str);
Atom* atom_new_boolean(char boolean);
/* list will be duplicated */
Atom* atom_new_list(const List* list);
/* func will be duplicated */
Atom* atom_new_function(const Function* func);

void atom_destroy_string(char* str);
void atom_destroy_function(Function* func);
void atom_destroy_list(List* list);

void atom_set_unkown(Atom* atom);
void atom_set_int(Atom* atom, long value);
void atom_set_uint(Atom* atom, unsigned long value);
void atom_set_string(Atom* atom, const char* str);
void atom_set_boolean(Atom* atom, char boolean);
void atom_set_list(Atom* atom, const List* list);
void atom_set_function(Atom* atom, const Function* func);

/* will duplicate new_val */
void atom_set_atom(Atom* atom, const Atom* new_val);

Atom* atom_parse_string(const Scope* scope, const char* str);

#endif /* _atom_h_ */
