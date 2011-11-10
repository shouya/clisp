#include <internal_functions.h>

#include <atom.h>
#include <list.h>
#include <function.h>
#include <scope.h>

#define DECL(name)                                      \
  Atom* internal_function_##name(                       \
    List* args,                                         \
    Scope* scope,                                       \
    char** error_message,                               \
    jmp_buf trace_point)                                \

/*typedef Atom* (*FunctionCallback)(List* args, Scope scope);*/

DECL(plus) {
  int i, sum = 0;
  for (i = 0; i != args->n_items; ++i) {
    long item;
    if (atom_get_int(args->items[i], &item) == 0) {
      sum += item;
    } else {
      RaiseError("invalid operator number of plus");
    }
  }
  return atom_new_int(sum);
}

DECL(minus) {
  int i, diff = 0;
  for (i = 0; i != args->n_items; ++i) {
    long item;
    if (atom_get_int(args->items[i], &item) == 0) {
      if (i == 0)
        diff = item;
      else
        diff -= item;
    } else {
      RaiseError("invalid operator number of plus");
    }
  }
  return atom_new_int(diff);
}

DECL(multiply) {
  int i, product = 1;
  for (i = 0; i != args->n_items; ++i) {
    long item;
    if (atom_get_int(args->items[i], &item) == 0) {
      product *= item;
    } else {
      RaiseError("invalid operator number of plus");
    }
  }
  return atom_new_int(product);
}
DECL(divide) {
  int i, quotient;
  for (i = 0; i != args->n_items; ++i) {
    long item;
    if (atom_get_int(args->items[i], &item) == 0) {
      if (i == 0)
        quotient = item;
      else
        quotient /= item;
    } else {
      RaiseError("invalid operator number of plus");
    }
  }
  return atom_new_int(quotient);
}

DECL(setq) {
  char* token;
  atom_get_token(args->items[0], &token);
  scope_set_symbol(token, args->items[1]);
  return atom_new_boolean(1);
}

DECL(defun);

DECL(print) {
  char* to_show;
  atom_show(args->item[0], &to_show);
  printf("%s\n", to_show);
  free(to_show);
  return atom_new_boolean(1);
}


