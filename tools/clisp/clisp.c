#include <stdlib.h>

#include <clisp_common.h>
/*#include <sy_file.h>*/

int main() {
  Scope* program;
  Atom* atom;

  program = scope_new(NULL);
  init_internal_functions(program);

  atom = atom_parse_string("(defun x (e)"
                           " (cond ((= e 1) 1)"
                           "       (t (+ e (x (- e 1))))))");
  atom_eval(atom, program);
  atom_destroy(atom);

  atom = atom_parse_string("(print (x 100))");
  atom_eval(atom, program);
  atom_destroy(atom);

  exit(0);
}
