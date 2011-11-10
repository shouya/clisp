/* header guard generate by genheader.pl */
#ifndef _list_h_
#define _list_h_

#include <defines.h>

struct _List {
  int n_items;
  int is_quoted;
  Atom* item;
}
List* list_new(void);
void list_destroy(List* list);
List* list_duplicate(List* src);

List* list_parse_string(const char* str);


#endif /* _list_h_ */
