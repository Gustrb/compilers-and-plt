#include "graph.c"

#include "queue.c"

DECL_QUEUE(queue_size_t, size_t, NUM_NODES_IN_POOL);

void bfs(size_t start, size_t end, size_t *path);
void reconstruct_path(size_t, size_t, size_t *, uint8_t *);

int main(void)
{
  size_t a = graph_new_vertex("a");
  size_t b = graph_new_vertex("b");
  size_t c = graph_new_vertex("c");

  graph_add_edge(a, b);
  graph_add_edge(b, c);

  size_t path[NUM_NODES_IN_POOL] = {0};
  bfs(a, c, path);
  return 0;
}

void solve(size_t s, size_t *p, uint8_t *);

void bfs(size_t start, size_t end, size_t *path)
{
  uint8_t visited[NUM_NODES_IN_POOL] = {0};
  solve(start, path, visited);
  reconstruct_path(start, end, path, visited);
}

void solve(size_t s, size_t *p, uint8_t *visited)
{
  queue_size_t queue = {0};

  size_t head = 0;
  size_t tail = 0;

  queue_enqueue(queue, s);
  visited[s] = 1;
  while (!queue_is_empty(queue))
  {
    size_t n = queue_dequeue(queue);

    for (size_t i = 0; i < node_pool[n].neighbors_len; ++i)
    {
      size_t next = node_pool[n].neighbors[i];
      if (!visited[next]) {
        queue_enqueue(queue, next);
        visited[next] = 1;
        p[next] = n;
      }
    }
  }
}

void reconstruct_path(size_t start, size_t end, size_t *path, uint8_t *visited)
{
  if (!visited[end])
  {
    printf("No path found between %s and %s\n", node_pool[start].label,
           node_pool[end].label);
    return;
  }
  size_t final_path[NUM_NODES_IN_POOL] = {0};
  size_t final_path_len = 0;

  size_t at = end;
  while (at != start)
  {
    final_path[final_path_len++] = at;
    at = path[at];    
  }
  final_path[final_path_len++] = start;

  // reverse the list
  size_t head = 0;
  size_t tail = final_path_len-1;
  while (head < tail)
  {
    size_t temp = final_path[head];
    final_path[head] = final_path[tail];
    final_path[tail] = temp;

    tail--;
    head++;
  }

  if (final_path[0] == start)
  {
    for (size_t i = 0; i < final_path_len; ++i)
    {
      printf("Step %zu, goto %s\n", i, node_pool[final_path[i]].label);
    }
    return;
  }

  printf("No path found between, %zu, %zu\n", start, end);
}
