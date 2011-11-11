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
  while (p - list->items < list->n_items) {
    atom_destroy(*p++);
  }
  free(list);
}

List* list_duplicate(const List* src) {
  return list_slice(src, 0, -1);
}

void list_append_item(List* list, Atom* item) {
  list->items = realloc(list->items, sizeof(Atom) * (++list->n_items));
  list->items[list->n_items - 1] = item;
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


#define STATE_FREE   1
#define STATE_STRING 2
#define STATE_LIST   3
#define STATE_TOKEN  4

List* list_parse_string(const char* str) {
  char* parse_buffer = malloc(sizeof(char) * PARSE_BUFFER_SIZE);
  int state = STATE_FREE, bracket_level = 0;
  char* pb = parse_buffer;
  const char* ps = str;
  List* list = list_new();

  /* charater level state machine */
  for (; *ps; ++ps) {
    switch (state) {
    case STATE_FREE: {

      if (isspace(*ps)) {
        continue;
      }

      *pb++ = *ps;
      if (*ps == '(') {
        state = STATE_LIST;
        ++bracket_level;
      } else if (*ps == '"') {
        state = STATE_STRING;
      } else {
        state = STATE_TOKEN;
      }

    } break;
    case STATE_STRING: {

      *pb++ = *ps;
      if (*ps == '"') {
        state = STATE_FREE;
        *pb = '\0';
        list_parse_commit(list, parse_buffer);
        pb = parse_buffer;
      }

    } break;
    case STATE_LIST: {
      
      *pb++ = *ps;
      if (*ps == '(') {
        ++bracket_level;
      } else if (*ps == ')') {
        --bracket_level;
      }

      if (bracket_level == 0) {
        state = STATE_FREE;
        *pb = '\0';
        list_parse_commit(list, parse_buffer);
        pb = parse_buffer;
      }

    } break;
    case STATE_TOKEN: {

      *pb++ = *ps;
      if (isspace(*ps)) {
        state = STATE_FREE;
        pb[-1] = '\0';
        list_parse_commit(list, parse_buffer);
        pb = parse_buffer;
      }

    } break;

    default:
      ;
    }
  }
/*  list_parse_commit(list, parse_buffer);*/
  

  free(parse_buffer);
  return list;
}
#undef STATE_FREE
#undef STATE_STRING
#undef STATE_LIST
#undef STATE_TOKEN

static void list_parse_commit(List* lst, const char* str) {
  list_append_item(lst, atom_parse_string(str));
}


int list_eval(List* list, Scope* scope, Atom** atom) {
  Atom** iter;
  if (list->is_quoted != 0) {
    *atom = atom_new_list(list);
    return 0;
  }

  iter = list->items;
  while (iter - list->items < list->n_items) {
    atom_eval(*iter, scope);
    ++iter;
  }

  {
    List* args = list_slice(list, 1, -1);
    Function* func;
    Atom* result;

    if (atom_get_function(list->items[0], &func) != 0) {
      *atom = NULL;
      list_destroy(args);
      return -1;
    }

    if (function_call(func, args, scope, &result) != 0) {
      *atom = NULL;
      list_destroy(args);
      return -2;
    }

    *atom = result;
    list_destroy(args);

    return 0;
  }
}

void list_show(const List* list, char** str) {
  char* buf = calloc(MAX_SHOW_BUFFER_SIZE, sizeof(char));
  Atom* const* iter = list->items;

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

    strcat(buf, ", ");
    ++iter;
  }

  *str = malloc(sizeof(char) * (strlen(buf)+1));
  strcpy(*str, buf);

  free(buf);
  return;
}
