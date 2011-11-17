#include <list.h>

#include <atom.h>
#include <function.h>

#define PARSE_BUFFER_SIZE 8192
#define MAX_SHOW_BUFFER_SIZE 4096

/*
struct _List {
  int n_items;
  int is_quoted;
  Atom* items;
};
*/

static void list_parse_commit(List* lst, const char* str);

List* list_new(void) {
  return calloc(1, sizeof(List));
}

void list_destroy(List* list) {
  Atom** p = list->items;
  while (p < list->items + list->n_items) {
    atom_destroy(*p++);
  }
  free(list);
}

List* list_duplicate(const List* src) {
  return list_slice(src, 0, -1);
}

void list_append_item(List* list, Atom* item) {
  list->items = realloc(list->items, sizeof(Atom*) * (++list->n_items));
  list->items[list->n_items - 1] = item;
}

void list_prepend_item(List* list, Atom* item) {
  list->items = realloc(list->items, sizeof(Atom) * (++list->n_items));
  memmove(list->items+1, list->items, sizeof(Atom*) * list->n_items-1);
  *list->items = item;
}


List* list_slice(const List* list, int start, int end) {
  Atom* const* s, * const* e;
  Atom** s_n;
  List* ret;

  if (list->n_items == 0) return list_new();

  s = list->items + start;
  e = list->items + end + 1;

  if (start < 0)
    s += list->n_items;
  if (end < 0)
    e += list->n_items;

  if (s > e) {
    Atom* const* temp = s;
    s = e; e = temp;
  }

  if (e == s) return list_new();

  ret = list_new();
  ret->items = calloc(e - s, sizeof(Atom*));
  ret->n_items = e - s;

  s_n = ret->items;

  while (s != e) {
    *s_n = atom_duplicate(*s);
    ++s; ++s_n;
  }

  return ret;
}

void list_dereference_token(List* list, const Scope* scope) {
  Atom** iter = list->items;
  while (iter != list->items + list->n_items) {
    atom_dereference_token(*iter, scope);

    ++iter;
  }
}


List* list_parse_string(const char* str) {
  char* parse_buffer = malloc(sizeof(char) * PARSE_BUFFER_SIZE);
  int bracket_level = 0;
  char in_str = 0, in_lst = 0, in_tok = 0, to_commit = 0;
  char* pb = parse_buffer;
  const char* ps = str;
  List* list = list_new();

  /* charater level state machine */
  for (; *ps; ++ps) {
    if (isspace(*ps) && !in_tok && !in_str && !in_lst) continue;

    *pb++ = *ps;
    if (*ps == '"' && ps[-1] != '\\') {
      in_str ^= 1;
    } else if (strncmp(ps, "'(", 2) == 0 && !in_str) {
      *pb++ = *++ps;
      in_lst = ++bracket_level;
    } else if (*ps == '(' && !in_str) {
      in_lst = ++bracket_level;
    } else if (*ps == ')' && !in_str) {
      in_lst = --bracket_level;
    } else if (isspace(*ps) && in_tok) {
      in_tok = 0;
    } else {
      in_tok = 1;
    }

    if (*ps == ')' && !in_lst && !in_str) {
      to_commit = 1;
    } else if (*ps == '"' && !in_lst && !in_str) {
      to_commit = 1;
    } else if (isspace(*ps) && !in_tok && !in_lst && !in_str) {
      --pb;
      to_commit = 1;
    } else if (ps[1] == '\0' && in_tok) {
      to_commit = 1;
    } else if (ps[1] == '\0' && !in_lst && !in_str) {
      to_commit = 1;
    }
   
    if (to_commit) {
      to_commit = 0;
      *pb = '\0';
      list_parse_commit(list, parse_buffer);
      pb = parse_buffer;
    }

  }


/*  list_parse_commit(list, parse_buffer);*/
  

  free(parse_buffer);
  return list;
}

static void list_parse_commit(List* lst, const char* str) {
  if (strlen(str) == 0) return;
  list_append_item(lst, atom_parse_string(str));
}


int list_eval(List* list, Scope* scope, Atom** atom) {
  if (list->is_quoted != 0) {
    *atom = atom_new_list(list);
    return 0;
  }

  if (list->n_items == 0) {
    *atom = atom_new_boolean(0);
    return 0;
  }

  return function_eval_list(
    list, scope, atom);
}

void list_eval_atoms(List* list, Scope* scope) {
  Atom** iter = list->items;

  while (iter != list->items + list->n_items) {
    atom_eval(*iter, scope);
    ++iter;
  }
}


void list_show(const List* list, char** str) {
  char* buf = calloc(MAX_SHOW_BUFFER_SIZE, sizeof(char));
  Atom* const* iter = list->items;
  char any_item = 0;

  if (list->is_quoted) {
    strcpy(buf, "'(");
  } else {
    strcpy(buf, "(");
  }

  while (iter - list->items < list->n_items) {
    char* atm_shw;
    atom_show(*iter, &atm_shw);
    /* unsafe strcat at here, may overflow */
    strcat(buf, atm_shw);
    free(atm_shw);

    strcat(buf, " ");
    any_item = 1;
    ++iter;
  }

  if (any_item) buf[strlen(buf)-1] = '\0';
  
  strcat(buf, ")");

  *str = malloc(sizeof(char) * (strlen(buf)+1));
  strcpy(*str, buf);

  free(buf);
  return;
}

int list_compare(const List* l1, const List* l2) {
  Atom* const* iter1, * const* iter2;
  
  if (l1->n_items != l2->n_items) return 2;
  if (l1->is_quoted != l2->is_quoted) return 2;

  iter1 = l1->items;
  iter2 = l2->items;

  while (iter1 != l1->items + l1->n_items) {
    if (atom_compare(*iter1, *iter2) != 0) {
      return 2;
    }
  }

  return 0;
}
