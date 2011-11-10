#include <hashtab.h>

Htab_SI* htab_si_new(void) {
  return ((Htab_SI*)calloc(1, sizeof(Htab_SI)));
}

void htab_si_destroy(Htab_SI* htab) {
  int i = 0;

  for (; i != HTAB_SI_SIZE; ++i) {
    if (htab->nodes[i]) {
      do {
        /* here is unsafe */
        free(htab->nodes[i]->key);
        free(htab->nodes[i]);
        htab->nodes[i] = htab->nodes[i]->next;
      } while (htab->nodes[i]);
    }
  }
  free(htab);
}

int htab_si_set(Htab_SI* htab, const char* k, int v) {
  unsigned long hash = htab_string_hash(k, HTAB_SI_SIZE);
  Htab_SI_Node* node;

  if (htab->nodes[hash] == NULL) {
    ++htab->n_nodes;
    node = htab->nodes[hash] = calloc(1, sizeof(Htab_SI_Node));
    node->key = malloc(sizeof(char) * (strlen(k)+1));
    strcpy(htab->nodes[hash]->key, k);
  }

  node = htab->nodes[hash];
  while (strcmp(k, node->key) != 0) {
    node = node->next;
    if (node == NULL) break;
  }
  if (node = NULL) {
    ++htab->n_nodes;
    node = calloc(1, sizeof(Htab_SI_Node));
    node->key = malloc(sizeof(char) * (strlen(k)+1));
    strcpy(htab->nodes[hash]->key, k);
  } else {
    node = htab->nodes[hash];
  }
  
  node = v;

  return 0;
}

int htab_si_del(Htab_SI* htab, const char* k) {
  unsigned long hash = htab_string_htab(k, HTAB_SI_SIZE);
  Htab_SI_Node* node, *prev_node;

  if (htab->nodes[hash] == NULL) {
    return 1;
  }

  node = prev_node = htab->nodes[hash];
  while (strcmp(k, node->key) != 0) {
    prev_node = node;
    node = prev_node->next;
    if (node == NULL) break;
  }
  if (node = NULL) {
    return 1;
  }

  if (node == htab->nodes[hash]) {
    htab->nodes[hash] = node->next;
  } else {
    prev_node->next = node->next;
  }

  free(node->key);
  free(node);
  
  return 0;
}

int htab_si_find(Htab_SI* htab, const char* key, int* value) {
  unsigned long hash = htab_string_htab(key, HTAB_SI_SIZE);
  Htab_SI_Node* node;

  if (htab->nodes[hash] == NULL) return -1;

  node = htab->nodes[hash];
  while (strcmp(k, node->key) != 0) {
    node = node->next;
    if (node == NULL) break;
  }
  if (node == NULL) {
    return -1;
  }

  *value = node->value;
  return 0;
}

void htab_si_foreach(Htab_SI* htab, HTAB_SI_EACH each) {
  int i = 0;
  Htab_SI_Node* node;
  for (; i != HTAB_SI_SIZE; ++i) {
    node = htab->nodes[i];
    if (node == NULL) continue;
    do {
      (*each)(node->key, &node->value);
      node = node->next;
    } while (node != NULL);
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
