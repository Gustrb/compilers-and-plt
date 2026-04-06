#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_NEIGHBORS 32
#define NUM_NODES_IN_POOL MAX_NEIGHBORS * 1024

typedef struct {
  size_t neighbors[MAX_NEIGHBORS];
  size_t neighbors_len;

  const char *label;
} node_t;

void print_node(node_t *n);

// TODO: put those 2 inside a struct
static node_t node_pool[NUM_NODES_IN_POOL] = {0};
static size_t nodes_len = 0;

size_t graph_new_vertex(const char *label);
void graph_add_edge(size_t from, size_t to);

size_t graph_new_vertex(const char *label)
{
  if (nodes_len >= NUM_NODES_IN_POOL)
  {
    fprintf(stderr, "failed to allocate new vertex, not enough memory\n");
    exit(1);
  }

  size_t i = nodes_len;
  node_pool[i].label = label;
  memset(node_pool[i].neighbors, 0, MAX_NEIGHBORS);
  node_pool[i].neighbors_len = 0;
  nodes_len++;
  return i;
}

void graph_add_edge(size_t from, size_t to)
{
  if (node_pool[from].neighbors_len >= MAX_NEIGHBORS)
  {
    fprintf(stderr, "failed to allocate new edge, not enough memory\n");
    exit(1);
  }
  node_pool[from].neighbors[node_pool[from].neighbors_len++] = to;
}

void print_node(node_t *n)
{
    printf("visited: %s\n", n->label);
}

#define DECL_ADJ_MATRIX(name, type, s) \
  typedef struct {\
    type data[s];\
  } name; \
  void name##_add_edge(name *mat, size_t from, size_t to) \
  {\
    mat->data[from * s + to] = 1;\
  }

