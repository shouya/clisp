#include <scope.h>

#include <atom.h>
#include <hashtab.h>

/*
struct _Scope {
  Scope* parent;
  Htab* symtab;
}
*/

Scope* scope_new(Scope* parent) {
  Scope* s = calloc(sizeof(Scope));
  s->parent = parent;
  s->symtab = htab_new(SCOPE_SYMBOL_HASH_SIZE, &atom_destroy);
  return s;
}

void scope_destroy(Scope* scope) {
  htab_destroy(s->symtab);
  free(scope);
}

/* name will be duplicate but atom will not */
void scope_set_symbol(Scope* s, const char* name, Atom* atom) {
  htab_set(s->symtab, name, atom);
}

/*
  return 0 if delete successfully, 1 if symbol don't exists.
 */
int scope_del_symbol(Scope* s, const char* name) {
  if (htab_del(s->symtab, name) == 0)
    return 0;
  return 1;
}

/* don't free atom_ptr which passed in, it will point to item in hashtable
   if pass 0 in atom_ptr, only will check symbol existence.
   returns -1 if symbol don't exists, else return 0.
 */

int scope_find_symbol(Scope* s, const char* name, Atom** atom_ptr) {
  Atom* atom;
  if (htab_find(s->symtab, name, atom) == 0) {
    if (atom_ptr != NULL)
      *atom_ptr = atom;
    return 0;
  }

  if (s->parent == NULL)
    return -1;

  return scope_find_symbol(s->parent, name, atom_ptr);
}
