#include "graph.c"

void dfs(size_t at, uint8_t *visited, uint64_t *components, uint64_t count);
uint64_t find_connected_components(uint64_t *);

int main(void)
{
  size_t a = graph_new_vertex("a");
  size_t b = graph_new_vertex("b");
  size_t c = graph_new_vertex("c");
  size_t d = graph_new_vertex("d");

  graph_add_edge(a, c);
  graph_add_edge(b, d);

  uint64_t components[NUM_NODES_IN_POOL] = {0};
  uint64_t num_components = find_connected_components(components);

  for (size_t i = 0; i < nodes_len; ++i)
  {
    printf("Label: %s, id: %llu\n", node_pool[i].label, components[i]);
  }
  return 0;
}

uint64_t find_connected_components(uint64_t *components)
{
  uint8_t visited[NUM_NODES_IN_POOL] = {0};
  uint64_t count = 0;

  for (size_t i = 0; i < nodes_len; ++i)
  {
    if (!visited[i])
    {
      count++;
      dfs(i, visited, components, count);
    }
  }

  return count;
}

void dfs(size_t at, uint8_t *visited, uint64_t *components, uint64_t count)
{
  visited[at] = 1;
  components[at] = count;

  for (size_t i = 0; i < node_pool[at].neighbors_len; ++i)
  {
    if (!visited[node_pool[at].neighbors[i]])
    {
      dfs(node_pool[at].neighbors[i], visited, components, count);
    }
  }
}
