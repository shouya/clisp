#include <setjmp.h>

#include <function.h>

#include <list.h>
#include <atom.h>
#include <scope.h>

/*
struct Function_ {
  char* parameter;
  int n_insts;
  char* insts;
};
*/

static Function* function_new(void) {
  return calloc(1, sizeof(Function));
}


Function* function_new_internal(const List* parameter, FunctionCallback body) {
  Function* f = function_new();
  f->type = FUNC_TYPE_INTERNAL;
  f->parameter = list_duplicate(parameter);
  f->internal = body;
  return f;
}

Function* function_new_userdefined(const List* parameter, const List* body) {
  Function* f = function_new();
  f->type = FUNC_TYPE_USERDEFINED;
  f->parameter = list_duplicate(parameter); 
  if (function_check_parameter(f) != 0) {
    list_destroy(f->parameter);
    free(f);
    return NULL; /* this function may return a null value */
  }
  f->user_defined = list_duplicate(body);

  return f;
}

Function* function_duplicate(const Function* function) {
  if (function->type == FUNC_TYPE_USERDEFINED) {
    return function_new_userdefined(
      function->parameter, function->user_defined);
  } else {
    return function_new_internal(
      function->parameter, function->internal);
  }
}


void function_destroy(Function* f) {
  list_destroy(f->parameter);
  if (f->type == FUNC_TYPE_USERDEFINED) {
    list_destroy(f->user_defined);
  }
}

int function_check_parameter(const Function* function) {
  Atom* const* p;
  const List* param;
  int n_invalid = 0;

  if (function->type == FUNC_TYPE_INTERNAL) {
    return 0; /* internal, handle and check */
  }

  param = function->parameter;
  p = param->items;

  while (p - param->items < param->n_items) {
    if ((*p++)->type != ATOM_TYPE_TOKEN)
      ++n_invalid;
  }
  return n_invalid;
}

int function_check_arguments(const Function* function,
                             const List* arguments) {
  if (function->type == FUNC_TYPE_INTERNAL)
    return 0;

  return ((function->parameter->n_items == arguments->n_items) ?
          0 : (-1));
}


/*
  returns:
     0: successed and stored result into `result'.
    -1: argument count error.
    -2: parameter invalid.
    -3: functionc evaluate error.
    
 */

int function_call(const Function* function,
                  const List* args,
                  Scope* scope,
                  Atom** result) {
  if (function->type == FUNC_TYPE_INTERNAL) {
    return function_call_internal(
      function->internal,
      args, scope, result);
  } else {
    return function_call_userdefined(
      function, args, scope, result);
  }

  return 0;
}

/*
  returns:
      0:   successed and stored result into atom
     -1:   no such function in scope
     -2:   function cannot be evaluate
 */

int function_eval_list(const List* list, Scope* scope, Atom** result) {
  Function* func_obj;
  Atom* func_atom;
  List* args;
  Atom* func_evaled;

  if (list->n_items == 0) return -2;

  func_atom = atom_duplicate(list->items[0]);
  atom_dereference_token(func_atom, scope);
  if (atom_get_function(func_atom, &func_obj) != 0) {
    atom_destroy(func_atom);
    return -1;
  }
  args = list_slice(list, 1, -1);

  if (func_obj->type == FUNC_TYPE_USERDEFINED) {
    Atom** iter = &args->items[0];
    while (iter != args->items + args->n_items) {
      atom_eval(*iter++, scope);
    }
  } else {
/*    list_dereference_token(args, scope);*/
  }

  if (function_call(func_obj, args, scope, &func_evaled) == 0) {
    *result = func_evaled;
    list_destroy(args);
    atom_destroy(func_atom);
    return 0;
  }

  list_destroy(args);
  atom_destroy(func_atom);

  return -2;
}


int function_call_internal(const FunctionCallback callback,
                           const List* args,
                           Scope* scope,
                           Atom** result) {
  Scope* function_scope = scope_new(scope);
  jmp_buf trace_point;
  List* args_dup = NULL;
  char* errmsg;

  if (setjmp(trace_point) == 0) {
    Atom* eval_result;
    args_dup = list_duplicate(args);
    eval_result = (*callback)(args_dup, function_scope, &errmsg, trace_point);
    *result = eval_result;
  } else {
    /* handle error here */
    list_destroy(args_dup);
    scope_destroy(function_scope);
    return -3;
  }

  list_destroy(args_dup);
  scope_destroy(function_scope);
  return 0;
}


int function_call_userdefined(const Function* function,
                              const List* args,
                              Scope* scope,
                              Atom** result) {
  Atom** arg_ptr;
  Atom* const* param_ptr;
  List* args_dup;
  Scope* function_scope;
  Atom* eval_result;

  if (function_check_parameter(function) != 0) {
    return -2;
  }
  if (function_check_arguments(function, args) != 0) {
    return -1;
  }

  function_scope = scope_new(scope);
  args_dup = list_duplicate(args);
  arg_ptr = args_dup->items;
  param_ptr = function->parameter->items;

  while (arg_ptr != args_dup->items + args_dup->n_items) {
    char* sym_name;
    
    atom_eval(*arg_ptr, function_scope);
    atom_get_token(*param_ptr, &sym_name);

    scope_set_symbol(function_scope, sym_name, *arg_ptr);

    ++arg_ptr;
    ++param_ptr;
  }

  if (list_eval(function->user_defined, function_scope, &eval_result) == 0) {
    *result = eval_result;
  } else {
    list_destroy(args_dup);
    scope_destroy(function_scope);
    return -3;
  }

  list_destroy(args_dup);
  scope_destroy(function_scope);

  return 0;
}
