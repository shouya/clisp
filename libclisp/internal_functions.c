#include <internal_functions.h>

#include <atom.h>
#include <list.h>
#include <function.h>
#include <scope.h>

#define DECL(name)                                      \
  Atom* internal_function_##name(                       \
    const List* args,                                   \
    Scope* scope,                                       \
    char** error_message,                               \
    jmp_buf trace_point)                                \

#define SCOPE_ADD_FUNC(scope, name, function)                           \
  do {                                                                  \
    Function* func;                                                     \
    Atom* atom;                                                         \
    List* param = list_new();                                           \
    func = function_new_internal(param, &internal_function_##function); \
    list_destroy(param);                                                \
    atom = atom_new_function(func);                                     \
    function_destroy(func);                                             \
    scope_set_symbol(scope, name, atom);                                \
  } while (0)                    

/*typedef Atom* (*FunctionCallback)(List* args, Scope scope);*/

void init_internal_functions(Scope* global_scope) {
  SCOPE_ADD_FUNC(global_scope, "+", plus);
  SCOPE_ADD_FUNC(global_scope, "-", minus);
  SCOPE_ADD_FUNC(global_scope, "setq", setq);
  SCOPE_ADD_FUNC(global_scope, "defun", defun);
  SCOPE_ADD_FUNC(global_scope, "print", print);
}


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
  int i, quotient = 0;
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
  scope_set_symbol(scope, token, args->items[1]);
  return atom_new_boolean(1);
}

DECL(defun) {
  char* function_name;
  List* function_param;
  List* function_body;
  Function* function;
  Atom* func_obj;

  atom_get_string(args->items[0], &function_name);
  atom_get_list(args->items[1], &function_param);
  atom_get_list(args->items[2], &function_body);

  function = function_new_userdefined(
    function_param,
    function_body);
  func_obj = atom_new_function(function);

  if (scope->parent) {
    scope_set_symbol(scope->parent, function_name, func_obj);
  }

  function_destroy(function);

  return atom_new_boolean(1);
};

DECL(print) {
  char* to_show;
  atom_show(args->items[0], &to_show);
  puts(to_show);
  free(to_show);
  return atom_new_boolean(1);
}


