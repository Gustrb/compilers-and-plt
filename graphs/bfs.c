#include "graph.c"
#include "queue.c"

DECL_QUEUE(queue_size_t, size_t, NUM_NODES_IN_POOL)

void bfs(size_t at);

int main(void)
{
  size_t a = graph_new_vertex("a");
  size_t b = graph_new_vertex("b");
  size_t c = graph_new_vertex("c");
  size_t d = graph_new_vertex("d");
  size_t e = graph_new_vertex("e");
  size_t f = graph_new_vertex("f");
  size_t g = graph_new_vertex("g");
  
  graph_add_edge(a, b);
  graph_add_edge(a, c);

  graph_add_edge(b, d);
  graph_add_edge(b, e);

  graph_add_edge(c, f);
  graph_add_edge(c, g);

  bfs(a);
  return 0;
}

void bfs(size_t start)
{
  uint8_t visited[NUM_NODES_IN_POOL] = {0};
  queue_size_t queue = {0};

  size_t head = 0;
  size_t tail = 0;

  queue_enqueue(queue, start);
  while (!queue_is_empty(queue))
  {
    size_t curr = queue_dequeue(queue);
    if (visited[curr]) continue;

    visited[curr] = 1;
    printf("visited: %s\n", node_pool[curr].label);

    for (size_t i = 0; i < node_pool[curr].neighbors_len; ++i)
    {
      queue_enqueue(queue, node_pool[curr].neighbors[i]);      
    }
  }
}
