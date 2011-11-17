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
  SCOPE_ADD_FUNC(global_scope, "*", multiply);
  SCOPE_ADD_FUNC(global_scope, "/", divide);

  SCOPE_ADD_FUNC(global_scope, "setq", setq);
  SCOPE_ADD_FUNC(global_scope, "defun", defun);
  SCOPE_ADD_FUNC(global_scope, "print", print);
  SCOPE_ADD_FUNC(global_scope, "cond", cond);
   
  SCOPE_ADD_FUNC(global_scope, "=", equal); 
  SCOPE_ADD_FUNC(global_scope, "<", lessthan);
  SCOPE_ADD_FUNC(global_scope, ">", greatthan);
  SCOPE_ADD_FUNC(global_scope, "!=", notequal);

  SCOPE_ADD_FUNC(global_scope, "let", let);
  SCOPE_ADD_FUNC(global_scope, "progn", progn);

  SCOPE_ADD_FUNC(global_scope, "car", car);
  SCOPE_ADD_FUNC(global_scope, "cdr", cdr);
  SCOPE_ADD_FUNC(global_scope, "atom", atom);
  SCOPE_ADD_FUNC(global_scope, "quote", quote);
  
}


DECL(plus) {
  int i, sum = 0;
  list_eval_atoms(args, scope);
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
  list_eval_atoms(args, scope);
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
  list_eval_atoms(args, scope);
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
  list_eval_atoms(args, scope);
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
  Scope* global_scope = scope;
  list_eval_atoms(args, scope);
  atom_get_token(args->items[0], &token);
  while (global_scope->parent != NULL) global_scope = global_scope->parent;

  scope_set_symbol(scope->parent, token, atom_duplicate(args->items[1]));
  return atom_new_boolean(1);
}

DECL(defun) {
  char* function_name;
  List* function_param;
  List* function_body;
  Function* function;
  Atom* func_obj;

  atom_get_token(args->items[0], &function_name);
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
}

DECL(print) {
  char* to_show;
  list_eval_atoms(args, scope);
  atom_show(args->items[0], &to_show);
  puts(to_show);
  free(to_show);
  return atom_new_boolean(1);
}

DECL(cond) {
  Atom** cond_iter = args->items;
  if (args->n_items == 0) return atom_new_boolean(0);

  while (cond_iter != args->items + args->n_items) {
    List* cond_pair;
    char result_in_boolean;

    if (atom_get_list(*cond_iter, &cond_pair) != 0) {
      /* invalid syntax */
    }

    atom_eval(cond_pair->items[0], scope);
    atom_get_boolean(cond_pair->items[0], &result_in_boolean);
    if (result_in_boolean) {
      atom_eval(cond_pair->items[cond_pair->n_items - 1], scope);
      return atom_duplicate(cond_pair->items[cond_pair->n_items - 1]);
    }

    ++cond_iter;
  }
  return atom_new_boolean(0);
}


DECL(equal) {
  if (args->n_items == 1) {
    return atom_new_boolean(1);
  } else if (args->n_items == 2) {
    Atom* first = args->items[0], *second = args->items[1];
    atom_eval(first, scope);
    atom_eval(second, scope);
    if (atom_compare(first, second) == 0) return atom_new_boolean(1);
    return atom_new_boolean(0);
  } else {
    /* invalid argument count */
  }
  return atom_new_boolean(0);
}
DECL(lessthan) {
  if (args->n_items == 1) {
    return atom_new_boolean(1);
  } else if (args->n_items == 2) {
    Atom* first = args->items[0], *second = args->items[1];
    atom_eval(first, scope);
    atom_eval(second, scope);
    if (atom_compare(first, second) == -1) return atom_new_boolean(1);
    return atom_new_boolean(0);
  } else {
    /* invalid argument count */
  }
  return atom_new_boolean(0);
}
DECL(greatthan) {
  if (args->n_items == 1) {
    return atom_new_boolean(1);
  } else if (args->n_items == 2) {
    Atom* first = args->items[0], *second = args->items[1];
    atom_eval(first, scope);
    atom_eval(second, scope);
    if (atom_compare(first, second) == 1) return atom_new_boolean(1);
    return atom_new_boolean(0);
  } else {
    /* invalid argument count */
  }
    return atom_new_boolean(0);
}

DECL(notequal) {
  if (args->n_items == 1) {
    return atom_new_boolean(1);
  } else if (args->n_items == 2) {
    Atom* first = args->items[0], *second = args->items[1];
    atom_eval(first, scope);
    atom_eval(second, scope);
    if (atom_compare(first, second) != 0) return atom_new_boolean(1); 
    return atom_new_boolean(0);
  } else {
    /* invalid argument count */
  }
  return atom_new_boolean(0);
}

DECL(let) {
  Scope* local_scope;
  List* var_define;
  Atom* body;

  do {
    if (args->n_items == 2) break;
    if (atom_get_list(args->items[0], &var_define) == 0) break;

    return atom_new_boolean(0);
  } while (0);

  body = args->items[1];
  var_define = list_duplicate(args->items[0]->list);

  local_scope = scope_new(scope);
  {
    Atom** var_iter = var_define->items;

    if ((*var_iter)->type == ATOM_TYPE_TOKEN) {
      Atom* pair_atm = atom_duplicate(*var_iter);
      var_define = list_new();
      list_append_item(var_define, pair_atm);
      var_iter = var_define->items;
    }

    if ((*var_iter)->type == ATOM_TYPE_LIST) {
      List* var_pair;
      while (var_iter != var_define->items + var_define->n_items) {
        char* token_name;
        Atom* value;
        do {
          if (atom_get_list(*var_iter, &var_pair) == 0) break;
          if (var_pair->n_items == 2) break;
          if (atom_get_token(var_pair->items[0], &token_name) == 0) break;

          list_destroy(var_define);
          scope_destroy(local_scope);
          return atom_new_boolean(0);
        } while (0);

        value = atom_duplicate(var_pair->items[1]);
        atom_eval(value, local_scope);
        scope_set_symbol(local_scope, token_name, value);

        ++var_iter;
      }
    }

    /* exec last */
    atom_eval(body, local_scope);
  }
  scope_destroy(local_scope);

  return atom_duplicate(body);
}

DECL(progn) {
  Atom** prog_iter = args->items;
  while (prog_iter != args->items + args->n_items) {
    atom_eval(*prog_iter, scope);
    ++prog_iter;
  }
  if (args->n_items == 0) return atom_new_unknown();

  return atom_duplicate(prog_iter[-1]);
}

DECL(car) {
  List* lst;
  if (atom_get_list(args->items[0], &lst) != 0) {
    /* error */
    return atom_new_boolean(0);
  }
  if (lst->n_items == 0) {
    return atom_new_unknown();
  }

  return atom_duplicate(lst->items[0]);
}


DECL(cdr) {
  Atom* atm;
  List* lst;
  if (atom_get_list(args->items[0], &lst) != 0) {
    /* error */
    return atom_new_boolean(0);
  }
  if (lst->n_items == 0) {
    lst = list_new();
    return atom_new_list(lst);
  }

  lst = list_slice(lst, 1, -1);
  atm = atom_new_list(lst);
  list_destroy(lst);

  return atm;
}

DECL(cons) {
  Atom* fst;
  List* sec;
  if (args->n_items != 2) {
    /* error */
    return atom_new_unknown();
  }

  if (atom_get_list(args->items[1], &sec) != 0) {
    /* error */
    return atom_new_unknown();
  }
  fst = args->items[0];

  list_prepend_item(sec, fst);

  return atom_new_list(sec);
}

DECL(atom) {
  Atom* atm;

  if (args->n_items != 1) {
    /* error */
    return atom_new_unknown();
  }
  atm = args->items[0];
  atom_eval(atm, scope);

  return atom_new_boolean(atm->type != ATOM_TYPE_LIST);
}

DECL(quote) {
  Atom* atm;
  List* lst;

  if (args->n_items != 1) {
    /* error */
    return atom_new_unknown();
  }

  atm = args->items[0];
  if (atom_get_list(atm, &lst) != 0) {
    /* error */
    return atom_new_unknown();
  }

  lst->is_quoted = 1;
  return atom_new_list(lst);
}


#undef DECL
#undef SCOPE_ADD_FUNC
