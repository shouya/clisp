/* header guard generate by genheader.pl */
#ifndef _atom_h_
#define _atom_h_

#define ATOM_TYPE_UNKOWN    0
#define ATOM_TYPE_INT       1
#define ATOM_TYPE_UINT      2
#define ATOM_TYPE_STRING    3
#define ATOM_TYPE_BOOLEAN   4
#define ATOM_TYPE_FUNCTION  5
#define ATOM_TYPE_LIST      6
#define ATOM_TYPE_SPECIAL   7

#define ATOM_SPEC_TYPE_ARG  1 /* with argument name as content */
#define ATOM_SPEC_TYPE_VAR  2 /* with variable name as content */
#define ATOM_SPEC_TYPE_FUN  3 /* with function name as content */
#define ATOM_SPEC_TYPE_RET  4 /* with no content */

struct _Atom {
  char* name;
  int type;
  int is_ref;
  union {
    long i32;             /* 1 */
    unsigned long ui32;   /* 2 */
    char* string;         /* 3 */
    char boolean;         /* 4 */
    List* list;           /* 5 */
    Function* function;   /* 6 */
    struct _Special_t {   /* 7 */
      int type;
      char* content;
    } special;
  };
};

Atom* atom_new(void);
void atom_destroy(Atom* a);
Atom* atom_duplicate(Atom* a);

Atom* atom_new_unknow(void);
Atom* atom_new_int(long value);
Atom* atom_new_uint(unsigned long value);
/* str will be duplicate  */
Atom* atom_new_string(const char* str);
Atom* atom_new_boolean(char boolean);
/* func will be duplicated */
Atom* atom_new_function(const Function* func);
/* content wil be duplicate */
Atom* atom_new_special(int type, const char* content);

void atom_destroy_string(char* atom);
void atom_destroy_function(Function* func);
void atom_destroy_special(int type, const char* content);

void atom_set_unkown(Atom* atom);
void atom_set_int(Atom* atom, long value);
void atom_set_uint(Atom* atom, unsigned long value);
void atom_set_string(Atom* atom, const char* str);
void atom_set_boolean(Atom* atom, char boolean);
void atom_set_function(Atom* atom, const Function* func);
void atom_set_special(Atom* atom, int type, const char* content);
void atom_set_atom(Atom* atom, Atom* new_val);

Atom* atom_parse_string(Program* prog, const char* str);

#endif /* _atom_h_ */
