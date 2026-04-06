#include "graph.c"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void dfs(size_t at, void (*fn)(node_t *));

int main(void)
{
  size_t a = graph_new_vertex("a");
  size_t aa = graph_new_vertex("aa");

  size_t b = graph_new_vertex("b");
  size_t c = graph_new_vertex("c");

  graph_add_edge(a, aa);
  graph_add_edge(a, c);
  graph_add_edge(aa, b);
  graph_add_edge(b, c);

  dfs(a, print_node);

  return 0;
}

void __dfs(size_t at, uint8_t *visited, void (*fn)(node_t *n))
{
  if (visited[at]) return;
  visited[at] = 1;
  fn(&node_pool[at]);

  for (size_t i = 0; i < node_pool[at].neighbors_len; ++i)
  {
    __dfs(node_pool[at].neighbors[i], visited, fn);
  }
}

void dfs(size_t at, void (*fn)(node_t *))
{
  uint8_t visited[NUM_NODES_IN_POOL] = {0};
  __dfs(at, visited, fn);
}
