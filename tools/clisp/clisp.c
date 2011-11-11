#include <stdlib.h>

#include <clisp_common.h>
/*#include <sy_file.h>*/

int main() {
  Scope* program;
  Atom* atom;

  program = scope_new(NULL);
  init_internal_functions(program);

  atom = atom_parse_string("(setq a \"hello\")");
  atom_eval(atom, program);
/*  atom_destroy(atom);*/

  atom = atom_parse_string("(print a)");
  atom_eval(atom, program);

  exit(0);
}
