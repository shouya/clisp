/* header guard generated by genheader.pl */
#ifndef _scope_h_
#define _scope_h_

#include <defines.h>

#define SCOPE_SYMBOL_HASH_SIZE 4096

struct _Scope {
  Scope* parent;
  Htab* symtab; /* symbol table of Atom* 's */
};

Scope* scope_new(Scope* parent);
void scope_destroy(Scope* scope);

/* name will be duplicate but atom will not */
void scope_set_symbol(Scope* s, const char* name, const Atom* atom);

/*
  return 0 if delete successfully, 1 if symbol don't exists.
 */
int scope_del_symbol(Scope* s, const char* name);

/* don't free atom_ptr which passed in, it will point to item in hashtable
   if pass 0 in atom_ptr, only will check symbol existence.
   returns -1 if symbol don't exists, else return 0.
 */
int scope_find_symbol(const Scope* s, const char* name, Atom** atom_ptr);

#endif /* _scope_h_ */
