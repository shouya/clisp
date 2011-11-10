#include <list.h>

#include <atom.h>

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
  Atom* p = list->items;
  while (p - list->items < list->n_items) {
    free(p++);
  }
  free(list);
}

List* list_duplicate(List* src) {
  return list_slice(src, 0, -1);
}

void list_append_item(List* list, Atom* item) {
  list->items = realloc(sizeof(Atom) * (++list->n_items));
  list->items[list->n_items - 1] = item;
}

List* list_slice(List* list, int start, int end) {
  Atom* s, *e;
  List* ret;

  s = list->items + start;
  e = list->items + end;

  if (start < 0)
    s += list->n_items;
  if (end < 0)
    e += list->n_items;

  if (start > end) {
    Atom* temp = s;
    s = e; e = temp;
  }

  ret = list_new();
  ret->items = calloc(e - s, sizeof(Atom));
  memcpy(ret->items, s, e - s);

  return ret;
}


#define STATE_FREE   1
#define STATE_STRING 2
#define STATE_LIST   3
#define STATE_TOKEN  4

List* list_parse_string(const char* str) {
  char* parse_buffer = malloc(sizeof(char) * PARSE_BUFFER_SIZE);
  int state = STATE_FREE, bracket_level = 0;
  char* pb = parbuf, ps = str;
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
        *pb = '\0';
        list_parse_commit(list, parse_buffer);
        pb = parse_buffer;
      }

    } break;

    default:
      ;
    }
  }

  free(parse_buffer);
  return list;
}
#undef STATE_FREE   1
#undef STATE_STRING 2
#undef STATE_LIST   3
#undef STATE_TOKEN  4

static void list_parse_commit(List* lst, const char* str) {
  list_append_item(lst, atom_parse_str(a));
}


Atom* list_eval(List* list, Scope* scope) {
  Atom* iter;
  if (list->is_quoted == 0) {
    return atom_new_list(list);
  }

  iter = list->items;
  while (iter - list->items < list->n_items) {
    atom_eval(iter, scope);
    ++iter;
  }

  if (list->items->type == ATOM_TYPE_FUNCTION) {
    List* args = list_slice(list, 1, -1);
    function_call(list->items[0], args);
  }
}

void list_show(List* list, char** str) {
  char* buf = calloc(MAX_SHOW_BUFFER_SIZE, sizeof(char));
  Atom* iter = list->items;

  strcpy(buf, "(");
  while (iter - list->items < list->n_items) {
    char* atm_shw;
    atom_show(iter, &atm_shw);
    /* unsafe strcat at here, may overflow */
    strcat(buf, atm_shw);
    free(atm_shw);

    strcat(buf, ", ");
    ++iter;
  }

  *str = malloc(sizeof(char) * strlen(buf));
  strcpy(*str, buf);

  free(buf);
  return;
}
