#include <atom.h>

#include <scope.h>
#include <list.h>
#include <function.h>

#define MAX_STRING_LENGTH 4096
#define MAX_TOKEN_LENGTH 128
#define MAX_LIST_STRING_LENGTH 8192

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
  case ATOM_TYPE_INT:
    free(a->i32);
    break;
  case ATOM_TYPE_UINT:
    free(a->ui32);
    break;
  case ATOM_TYPE_BOOLEAN:
    free(a->boolean);
    break;


  case ATOM_TYPE_STRING:
    atom_destroy_string(a->string);
    break;
  case ATOM_TYPE_FUNCTION:
    atom_destroy_function(a->function);
    break;
  case ATOM_TYPE_LIST:
    atom_destroy_list(a->list);
    break;


  default: /* unkown */
    break;
  }
}

Atom* atom_duplicate(Atom* a) {
  /** TODO **/
}

Atom* atom_reference(Atom* ref) {
  Atom* a = atom_new();
  a->type = ref->type;
  switch (a->type) {
  case ATOM_TYPE_UNKOWN: break;
  case ATOM_TYPE_INT: a->i32 = ref->i32; break;
  case ATOM_TYPE_UINT: a->ui32 = ref->ui32; break;
  case ATOM_TYPE_BOOLEAN: a->boolean = ref->boolean; break;
  case ATOM_TYPE_STRING: a->string = ref->string; break;
  case ATOM_TYPE_FUNCTION: a->function = ref->function; break;
  case ATOM_TYPE_LIST: a->list = ref->list; break;
  default:
    ;
  }
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

void atom_destroy_string(char* str) {
  free(str);
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


void atom_set_atom(Atom* atom, const Atom* new_val) {
  atom_reset(atom);
  atom->type = new_val->type;
  /* TODO */
}

Atom* atom_parse_string(const Scope* scope, const char* str) {
  char* tokenval;

  /* parse string */
  {
    char* strval = malloc(sizeof(char) * MAX_STRING_LENGTH);
    /* i don't know how to implement a dynamic width check in efficient way */
    if (sscanf(str, "\"%4095s\"", strval)) {
      Atom* a = atom_new_string(strval);
      free(strval);
      return a;
    }
    free(strval);
  }

  /* parse integer */
  {
    long ival;
    unsigned long uival;
    if (sscanf(str, "%i", &ival)) {
      return atom_new_int(ival);;
    }
    if (sscanf(str, "%u", &uival)) {
      return atom_new_uint(uival);
    }
  }

  /* parse boolean */
  {
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
  }

  /* parse list & function */
  {
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
  }

  { /* parse references */
    char* p;
    Atom* sym;

    if (scope == NULL) {
      return NULL;
    }
    p = str;

    while (isspace(*p) && ++p) {}
    if (scope_find_symbol(scope, p, &sym) == 0) {
      return atom_reference(sym);
    }
  }


  return NULL;
  
}


