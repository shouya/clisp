#include <hashtab.h>

Htab* htab_new(int size, HtabFreeHandler free_handler) {
  Htab* htab = (Htab*)calloc(1, sizeof(Htab));
  htab->size = size;
  htab->free_handler = free_handler;
  htab->nodes = (HtabNode*)Htabcalloc(size, sizeof(HtabNode*));
  return htab;
}

void htab_destroy(Htab* htab) {
  int i = 0;

  for (; i != HTAB_SIZE; ++i) {
    if (htab->nodes[i]) {
      free(htab->nodes[i]->key);
      if (htab->free_handler) {
        (*htab->free_handler)(htab->nodes[i]->value);
      }
      free(htab->nodes[i]);
    }
  }
  free(htab->nodes);
  free(htab);
}

int htab_set(Htab* htab, const char* k, void* v) {
  unsigned long hash = htab_string_hash(k, htab->size);

  if (htab->nodes[hash] == 0) {
    htab->nodes[hash] = calloc(1, sizeof(HtabNode));
    htab->nodes[hash]->key = malloc(sizeof(char) * (strlen(k)+1));
    strcpy(htab->nodes[hash]->key, k);
    ++htab->n_nodes;
  }

  htab->nodes[hash]->value = v;

  return 0;
}

int htab_del(Htab* htab, const char* k) {
  unsigned long hash = htab_string_htab(k, HTAB_SIZE);

  if (htab->nodes[hash] == NULL) {
    return 1;
  }

  free(htab->nodes[hash]->key);
  if (htab->free_handler) {
    (*htab->free_handler)(htab->nodes[hash]->value);
  }
  free(htab->nodes[hash]);
  htab->nodes[hash] = NULL;
  --htab->n_nodes;

  return 0;
}

int htab_find(Htab* htab, const char* key, void* value) {
  unsigned long hash = htab_string_htab(key, htab->size);

  if (htab->nodes[hash] == NULL) return -1;

  if (value) {
    value = node->value;
  }

  return 0;
}

void htab_foreach(Htab* htab, HtabEach each) {
  int i = 0;

  for (; i != HTAB_SIZE; ++i) {
    if (htab->nodes[i] == NULL) continue;
    (*each)(htab->nodes[i]->key, htab->nodes[i]->value);
  }
}



unsigned long htab_string_hash(const char* key, unsigned long hashsize) {
  unsigned long hash = 1; /* hash algorithm from tinyvm */
  const char* c;

  for (c = k; *c; ++c) {
    hash += (hash << *c) - *c;
  }

  return hash % hash_size;
}
