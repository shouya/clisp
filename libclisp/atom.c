#include <atom.h>

#include <scope.h>
#include <list.h>
#include <function.h>

#define MAX_STRING_LENGTH 4096
#define MAX_TOKEN_LENGTH 128
#define MAX_LIST_STRING_LENGTH 8192
#define MAX_SHOW_BUFFER_SIZE 4096

Atom* atom_new(void) {
  return calloc(1, sizeof(Atom));
}

void atom_destroy(Atom* a) {
  atom_reset(a);
  free(a);
}

void atom_reset(Atom* a) {
  if (a->is_ref) {
    a->type = ATOM_TYPE_UNKOWN;
    return;
  }

  switch (a->type) {
  case ATOM_TYPE_UNKOWN:
    break;

    /* value types */
  case ATOM_TYPE_INT:
    free(a->i32);
    break;
  case ATOM_TYPE_UINT:
    free(a->ui32);
    break;
  case ATOM_TYPE_BOOLEAN:
    free(a->boolean);
    break;

    /* reference types */
  case ATOM_TYPE_STRING:
    atom_destroy_string(a->string);
    break;
  case ATOM_TYPE_LIST:
    atom_destroy_list(a->list);
    break;
  case ATOM_TYPE_FUNCTION:
    atom_destroy_function(a->function);
    break;
  case ATOM_TYPE_TOKEN:
    atom_destroy_token(a->token);
    break;

  default: /* unkown */
    break;
  }
}

Atom* atom_duplicate(Atom* a) {
  /** TODO **/
}

int atom_token_to_refrence(Atom* a, Scope* scope) {
  Atom* sym;

  if (a->type != ATOM_TYPE_TOKEN) return -2;
  if (scope_find_symbol(scope, a->token, &sym) == 0) {
    atom_set_reference(a, sym);
    return 0;
  }
  return -1;
}

Atom* atom_reference(Atom* ref) {
  Atom* a = atom_new();
  atom_set_reference(ref);
  return a;
}

Atom* atom_new_unknow(void) {
  return atom_new();
}

Atom* atom_new_int(long value) {
  Atom* a = atom_new();
  atom_set_int(a, value);
  return a;
}
Atom* atom_new_uint(unsigned long value) {
  Atom* a = atom_new();
  atom_set_uint(value);
  return a;
}
/* str will be duplicate  */
Atom* atom_new_string(const char* str) {
  Atom* a = atom_new();
  atom_set_uint(str);
  return a;
}
Atom* atom_new_boolean(char boolean) {
  Atom* a = atom_new();
  atom_set_boolean(boolean);
  return a;
}
/* list will be duplicated */
Atom* atom_new_list(const List* list) {
  Atom* a = atom_new();
  atom_set_list(list);
  return a;
}
/* func will be duplicated */
Atom* atom_new_function(const Function* func) {
  Atom* a = atom_new();
  atom_set_function(func);
  return a;
}
/* token name will be duplicated */
Atom* atom_new_token(const char* token_name) {
  Atom* a = atom_new();
  atom_set_token(token_name);
  return a;
}

void atom_destroy_string(char* str) {
  free(str);
}
void atom_destroy_token(char* token) {
  free(token);
}

void atom_destroy_function(Function* func) {
  function_destroy(func);
}

void atom_set_unkown(Atom* atom) {
  atom_reset(atom);
}

void atom_set_int(Atom* atom, long value) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_INT;
  atom->i32 = malloc(sizeof(long));
  *atom->i32 = value;
}

void atom_set_uint(Atom* atom, unsigned long value) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_UINT;
  atom->ui32 = malloc(sizeof(unsigned long));
  *atom->ui32 = value;
}

void atom_set_string(Atom* atom, const char* str) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_STRING;
  atom->string = malloc(strlen(str) + 1);
  strcpy(atom->string, str);
}

void atom_set_boolean(Atom* atom, char boolean) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_BOOLEAN;
  atom->boolean = malloc(sizeof(char));
  *atom->boolean = boolean;
}

void atom_set_list(Atom* atom, const List* list) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_LIST;
  atom->list = list_duplicate(list);
}

void atom_set_function(Atom* atom, const Function* func) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_FUNCTION;
  atom->function = function_duplicate(func);
}

void atom_set_token(Atom* atom, const char* token_name) {
  atom_reset(atom);
  atom->type = ATOM_TYPE_TOKEN;
  atom->token = malloc(strlen(str) + 1);
  strcpy(atom->token, token_name);
}


void atom_set_atom(Atom* atom, const Atom* new_val) {
  atom_reset(atom);
  atom->type = new_val->type;
  /* TODO */
}

void atom_set_reference(Atom* a, const Atom* ref) {
  atom_reset(atom);
  a->type = ref->type;
  switch (a->type) {
  case ATOM_TYPE_UNKOWN: break;
  case ATOM_TYPE_INT: a->i32 = ref->i32; break;
  case ATOM_TYPE_UINT: a->ui32 = ref->ui32; break;
  case ATOM_TYPE_BOOLEAN: a->boolean = ref->boolean; break;
  case ATOM_TYPE_STRING: a->string = ref->string; break;
  case ATOM_TYPE_FUNCTION: a->function = ref->function; break;
  case ATOM_TYPE_LIST: a->list = ref->list; break;
  case ATOM_TYPE_TOKEN: a->token = ref->token; break;
  default:
    ;
  }
}


int atom_get_int(Atom* atom, long* i32) {
  switch (atom->type) {
  case ATOM_TYPE_INT:
    *i32 = atom->i32;
    break;
  case ATOM_TYPE_UINT:
    *i32 = atom->ui32;
    break;
  case ATOM_TYPE_BOOLEAN:
    *i32 = atom->boolean;
    break;
  default:
    return -1;
  }
  return 0;
}

int atom_get_uint(Atom* atom, unsigned* ui32) {
  switch (atom->type) {
  case ATOM_TYPE_INT:
    *ui32 = atom->i32;
    break;
  case ATOM_TYPE_UINT:
    *ui32 = atom->ui32;
    break;
  case ATOM_TYPE_BOOLEAN:
    *ui32 = atom->boolean;
    break;
  default:
    return -1;
  }
  return 0;
}

int atom_get_string(Atom* atom, char** string) {
  switch (atom->type) {
  case ATOM_TYPE_STRING:
    *string = atom->string;
    break;
  default:
    return -1;
  }
  return 0;
}

int atom_get_boolean(Atom* atom, char* boolean) {
  switch (atom->type) {
  case ATOM_TYPE_INT:
    *boolean = atom->i32 != 0;
    break;
  case ATOM_TYPE_UINT:
    *boolean = atom->ui32 != 0;
    break;
  case ATOM_TYPE_BOOLEAN:
    *boolean = atom->boolean != 0;
    break;
  case ATOM_TYPE_STRING:
    *boolean = atom->string != 0 && strlen(atom->string) != 0;
    break;
  case ATOM_TYPE_LIST:
    *boolean = atom->list->n_items != 0;
    break;
  case ATOM_TYPE_FUNCTION:
    *boolean = 1;
    break;

  case ATOM_TYPE_TOKEN: /* token cannot be use directly, anyway */
  default:
    return -1;
  }
  return 0;
}

int atom_get_list(Atom* atom, List** list) {
  switch (atom->type) {
  case ATOM_TYPE_LIST:
    *list = atom->list;
    break;
  default:
    return -1;
  }
  return 0;
}

int atom_get_function(Atom* atom, Function** function) {
  switch (atom->type) {
  case ATOM_TYPE_FUNCTION:
    *function = atom->function;
    break;
  default:
    return -1;
  }
  return 0;
}
int atom_get_token(Atom* atom, char** token_name) {
  switch (atom->type) {
  case ATOM_TYPE_TOKEN:
    *token_name = atom->token;
    break;
  default:
    return -1;
  }
  return 0;
}


int atom_eval(Atom* atom, Scope* scope) {
  switch (atom->type) {
  case ATOM_TYPE_UNKOWN:
  case ATOM_TYPE_INT:
  case ATOM_TYPE_UINT:
  case ATOM_TYPE_BOOLEAN:
  case ATOM_TYPE_STRING:
  case ATOM_TYPE_FUNCTION:
    break;

  case ATOM_TYPE_LIST: {
    Atom* result = list_eval(atom->list, scope);
    atom_set_atom(atom, result);
  } break;
  case ATOM_TYPE_TOKEN:
    atom_token_to_refrence(atom, scope);
    break;
  }

  return 0;
}



Atom* atom_parse_string(const char* str) {
  char* tokenval;

  /* parse string */
  do {
    char* strval = malloc(sizeof(char) * MAX_STRING_LENGTH);
    /* i don't know how to implement a dynamic width check in efficient way */
    if (sscanf(str, "\"%4095s\"", strval)) {
      Atom* a = atom_new_string(strval);
      free(strval);
      return a;
    }
    free(strval);
  } while (0);

  /* parse integer */
  do {
    long ival;
    unsigned long uival;
    if (sscanf(str, "%i", &ival)) {
      return atom_new_int(ival);;
    }
    if (sscanf(str, "%u", &uival)) {
      return atom_new_uint(uival);
    }
  } while (0);

  /* parse boolean */
  do {
    const char* p = str;
    char booltrue, boolfalse;
    while (isspace(*p) && ++p) {}
    booltrue =
      strcmp(p, "t") || strcmp(p, "T") ||
      strcmp(p, "true") || strcmp(p, "True") || strcmp(p, "TRUE") ||
      strcmp(p, "yes") || strcmp(p, "Yes") || strcmp(p, "YES");

    boolfalse =
      strcmp(p, "nil") || strcmp(p, "Nil") || strcmp(p, "NIL") ||
      strcmp(p, "false") || strcmp(p, "False") || strcmp(p, "FALSE") ||
      strcmp(p, "no") || strcmp(p, "No") || strcmp(p, "NO") ||
      strcmp(p, "()");

    if (booltrue || boolfalse) {
      return atom_new_boolean(booltrue);
    }
  } while (0);

  /* parse list & function */
  do {
    char *strlst = malloc(sizeof(char) * MAX_LIST_STRING_LENGTH);
    /* so here */
    if (sscanf(str, "'(%8191s)", strlst) ||
        sscanf(str, "(quote%*[ ](%8192s)%*[ ])", strlst)) {
      List* l = list_parse_string(strlst);
      Atom* a;
      l->is_quoted = 1;
      a = atom_new_list(l);
      list_destroy(l);
      free(str);
      return a;
    } else if (sscanf(str, "(%8191s)", strlst)) {
      List* l = list_parse_string(strlst);
      Atom* a;
      l->is_quoted = 0;
      a = atom_new_list(l);
      list_destroy(l);
      free(str);
      return a;
    }
  } while (0);

  /* parse references */ /*
  do {
    char* p;
    Atom* sym;

    if (scope == NULL) {
      break;
    }
    p = str;

    while (isspace(*p) && ++p) {}
    if (scope_find_symbol(scope, p, &sym) == 0) {
      return atom_reference(sym);
    }
    } while (0);*/


  /* parse token */
  do {
    while (isspace(*str) && ++str) {}

    return atom_new_token(str);

  } while (0);

  /* will not runs to here */
  return NULL;
}



void atom_show(Atom* atom, char** str) {
  char* buf = calloc(MAX_SHOW_BUFFER_SIZE, sizeof(char));
  switch (atom->type) {
  case ATOM_TYPE_UNKOWN:
    break;
  case ATOM_TYPE_INT: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "%+d", atom->i32);
  } break;
  case ATOM_TYPE_UINT: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "%u", atom->ui32);
  } break;
  case ATOM_TYPE_BOOLEAN: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "%s",   \
             atom->boolean ? "t" : "nil");
  } break;
  case ATOM_TYPE_STRING: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "\"%s\"", atom->string);
  } break;
  case ATOM_TYPE_FUNCTION: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "FUNCTION(%p)", atom);
  } break;
  case ATOM_TYPE_LIST: {
    char* lst_shw;
    list_show(atom->list, &lst_shw);
    strcpy(buf, lst_shw);
    free(lst_shw);
  } break;
  case ATOM_TYPE_TOKEN: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "TOKEN(%s)", atom->token);
  } break;

  default:
    ;
  }

  
  *str = malloc(strlen(buf) + 1);
  strcpy(*str, buf);

  free(buf);
  return;
}



void atom_show_debug(Atom* atom, char** str) {
  /* TODO */
}


#undef MAX_STRING_LENGTH
#undef MAX_TOKEN_LENGTH
#undef MAX_LIST_STRING_LENGTH
#undef MAX_SHOW_BUFFER_SIZE


