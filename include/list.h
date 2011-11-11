/* header guard generate by genheader.pl */
#ifndef _list_h_
#define _list_h_

#include <defines.h>

struct _List {
  int n_items;
  int is_quoted;
  Atom** items;
};

List* list_new(void);
void list_destroy(List* list);
List* list_duplicate(const List* src);

/* item will not be duplicated */
void list_append_item(List* list, Atom* item);
/* returns a new allocated list */
List* list_slice(const List* list, int start, int end);


List* list_parse_string(const char* str);

void list_show(const List* list, char** str_ret);

/*
  returns:
      0:   successed and stored result into atom
     -1:   no such function in scope
     -2:   function cannot be evaluate
 */
int list_eval(List* list, Scope* scope, Atom** atom);

#endif /* _list_h_ */
