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

Atom* atom_duplicate(const Atom* a) {
  if (a->is_ref) {
    return atom_reference(a);
  }

  switch (a->type) {
  case ATOM_TYPE_UNKOWN:
    return atom_new_unknow();
    break;

    /* value types */
  case ATOM_TYPE_INT:
    return atom_new_int(*a->i32);
  case ATOM_TYPE_UINT:
    return atom_new_uint(*a->ui32);
  case ATOM_TYPE_BOOLEAN:
    return atom_new_boolean(*a->boolean);

    /* reference types */
  case ATOM_TYPE_STRING:
    return atom_new_string(a->string);
  case ATOM_TYPE_LIST:
    return atom_new_list(a->list);
  case ATOM_TYPE_FUNCTION:
    return atom_new_function(a->function);
  case ATOM_TYPE_TOKEN:
    return atom_new_token(a->token);

  default: /* unkown */
    break;
  }

  return NULL;
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

Atom* atom_reference(const Atom* ref) {
  Atom* a = atom_new();
  atom_set_reference(a, ref);
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
  atom_set_uint(a, value);
  return a;
}
/* str will be duplicate  */
Atom* atom_new_string(const char* str) {
  Atom* a = atom_new();
  atom_set_string(a, str);
  return a;
}
Atom* atom_new_boolean(char boolean) {
  Atom* a = atom_new();
  atom_set_boolean(a, boolean);
  return a;
}
/* list will be duplicated */
Atom* atom_new_list(const List* list) {
  Atom* a = atom_new();
  atom_set_list(a, list);
  return a;
}
/* func will be duplicated */
Atom* atom_new_function(const Function* func) {
  Atom* a = atom_new();
  atom_set_function(a, func);
  return a;
}
/* token name will be duplicated */
Atom* atom_new_token(const char* token_name) {
  Atom* a = atom_new();
  atom_set_token(a, token_name);
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
void atom_destroy_list(List* list) {
  list_destroy(list);
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
  atom->token = malloc(strlen(token_name) + 1);
  strcpy(atom->token, token_name);
}


void atom_set_atom(Atom* atom, const Atom* new_val) {
  atom_reset(atom);
  atom->type = new_val->type;
  /* TODO */
}

void atom_set_reference(Atom* a, const Atom* ref) {
  atom_reset(a);
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
    *i32 = *atom->i32;
    break;
  case ATOM_TYPE_UINT:
    *i32 = *atom->ui32;
    break;
  case ATOM_TYPE_BOOLEAN:
    *i32 = *atom->boolean;
    break;
  default:
    return -1;
  }
  return 0;
}

int atom_get_uint(Atom* atom, unsigned* ui32) {
  switch (atom->type) {
  case ATOM_TYPE_INT:
    *ui32 = *atom->i32;
    break;
  case ATOM_TYPE_UINT:
    *ui32 = *atom->ui32;
    break;
  case ATOM_TYPE_BOOLEAN:
    *ui32 = *atom->boolean;
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
    Atom* result;
    if (list_eval(atom->list, scope, &result) != 0) {
      return 0; /* cannot evaluate list */
    }
    atom_set_atom(atom, result);
  } break;
  case ATOM_TYPE_TOKEN:
    atom_token_to_refrence(atom, scope);
    break;
  }

  return 0;
}



Atom* atom_parse_string(const char* str) {
  const char* const str_end = str + strlen(str) - 1;
  while (isspace(*str)) ++str;

  /* parse string */
  do {
    char is_str = 0;
    char* strval;
    Atom* ret;

    if (*str == '"' && *str_end == '"') {
      is_str = 1;
    }

    if (!is_str) break;

    strval = malloc(sizeof(char) * MAX_STRING_LENGTH);
    memcpy(strval, str + 1, str_end - str - 1);
    strval[str_end - str] = '\0';

    ret = atom_new_string(strval);
    free(strval);
    return ret;
    
  } while (0);

  /* parse integer */
  do {
    long ival;
    unsigned long uival;
    if (sscanf(str, "%li", &ival)) {
      return atom_new_int(ival);;
    }
    if (sscanf(str, "%lu", &uival)) {
      return atom_new_uint(uival);
    }
  } while (0);

  /* parse boolean */
  do {
    const char* p = str;
    char booltrue, boolfalse;

    booltrue =
      strcmp(p, "t")==0 || strcmp(p, "T")==0 ||
      strcmp(p, "true")==0 || strcmp(p, "True")==0 || strcmp(p, "TRUE")==0 ||
      strcmp(p, "yes")==0 || strcmp(p, "Yes")==0 || strcmp(p, "YES")==0;

    boolfalse =
      strcmp(p, "nil")==0 || strcmp(p, "Nil")==0 || strcmp(p, "NIL")==0 ||
      strcmp(p, "false")==0 || strcmp(p, "False")==0 || strcmp(p, "FALSE")==0 ||
      strcmp(p, "no")==0 || strcmp(p, "No")==0 || strcmp(p, "NO")==0 ||
      strcmp(p, "()")==0;

    if (booltrue || boolfalse) {
      return atom_new_boolean(booltrue);
    }
  } while (0);

  /* parse list & function */
  do {
    char *strlst;
    List* list;
    Atom* ret;
    char is_list = 0, in_str = 0, is_quoted = 0;
    int bracket_stack = 0;
    
    const char* p = NULL, *start = str;
 
    if (*str == '(') {
      is_list = 1;
      bracket_stack = 1;
      is_quoted = 0;
      start = p = str + 1;
    } else if (strncmp(str, "'(", 2) == 0) {
      is_list = 1;
      bracket_stack = 1;
      is_quoted = 1;
      start = p = str + 2;
    }

    if (!is_list) {
      break;
    }

    strlst = malloc(sizeof(char) * MAX_LIST_STRING_LENGTH);

    for (;;) {
      if (p > str_end) {
        free(strlst);
        strlst = NULL;
        break;
      }

      if (*p == '(') {
        !in_str ? ++bracket_stack : 0;
      } else if (*p == ')') {
        !in_str ? --bracket_stack : 0;
      } else if (*p == '"') {
        in_str ^= 1;
      }

      if (bracket_stack == 0) {
        break;
      }
      ++p;
    }

    if (strlst == NULL) { break; }

    memcpy(strlst, start, p - start);
    strlst[p-start] = '\0';

    list = list_parse_string(strlst);
    free(strlst);
    list->is_quoted = is_quoted;
    ret = atom_new_list(list);
    list_destroy(list);
    
    return ret;

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
    while (isspace(*str)) ++str;

    return atom_new_token(str);

  } while (0);

  /* will not runs to here */
  return NULL;
}



void atom_show(const Atom* atom, char** str) {
  char* buf = calloc(MAX_SHOW_BUFFER_SIZE, sizeof(char));
  switch (atom->type) {
  case ATOM_TYPE_UNKOWN:
    break;
  case ATOM_TYPE_INT: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "%+ld", *atom->i32);
  } break;
  case ATOM_TYPE_UINT: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "%lu", *atom->ui32);
  } break;
  case ATOM_TYPE_BOOLEAN: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "%s",   \
             *atom->boolean ? "t" : "nil");
  } break;
  case ATOM_TYPE_STRING: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "\"%s\"", atom->string);
  } break;
  case ATOM_TYPE_FUNCTION: {
    snprintf(buf, MAX_SHOW_BUFFER_SIZE, "FUNCTION(%p)", atom->function);
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

  
  *str = malloc(sizeof(char) * (strlen(buf)+1));
  strcpy(*str, buf);

  free(buf);
  return;
}



void atom_show_debug(const Atom* atom, char** str) {
  /* TODO */
}


#undef MAX_STRING_LENGTH
#undef MAX_TOKEN_LENGTH
#undef MAX_LIST_STRING_LENGTH
#undef MAX_SHOW_BUFFER_SIZE


