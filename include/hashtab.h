/* header guard generated by genheader.pl */
#ifndef _hashtab_h_
#define _hashtab_h_

#include <defines.h>

#define HTAB_SIZE 2048

typedef void (*HtabEach)(const char* k, void* v);
typedef void (*HtabFreeHandler)(void* v);


/* string to pointer hash */
struct _HtabNode {
  char* key;
  void* value;
};
struct _Htab {
  int n_nodes;
  int size;
  HtabFreeHandler free_handler;
  HtabNode** nodes;
};

Htab* htab_new(int size, HtabFreeHandler free_handler);
void htab_destroy(Htab* htab);

int htab_set(Htab* htab, const char* k, void* v); /* it won't duplicate v */
int htab_del(Htab* htab, const char* k);

int htab_find(Htab* htab, const char* key, void* value);
void htab_foreach(Htab* htab, HtabEach each);

unsigned long htab_string_hash(const char* key, unsigned long hashsize);




#endif /* _hashtab_h_ */
