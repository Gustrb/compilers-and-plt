#include "queue.c"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define R 5
#define C 7

typedef enum {
  MAZE_ENTRY_START = 'S',
  MAZE_ENTRY_EMPTY = '.',
  MAZE_ENTRY_ROCK = '#',
  MAZE_ENTRY_END = 'E',
} maze_entry_t;

#define S MAZE_ENTRY_START
#define DOT MAZE_ENTRY_EMPTY
#define ROCK MAZE_ENTRY_ROCK
#define E MAZE_ENTRY_END

static maze_entry_t maze[R][C] = {
  {S, DOT, DOT, ROCK, DOT, DOT, DOT},
  {DOT, ROCK, DOT, DOT, DOT, ROCK, DOT},
  {DOT, ROCK, DOT, DOT, DOT, DOT, DOT},
  {DOT, DOT, ROCK, ROCK, DOT, DOT, DOT},
  {ROCK, DOT, ROCK, E, DOT, ROCK, DOT},
};

static int32_t dr[] = {-1, 1, 0, 0};
static int32_t dc[] = {0, 0, 1, -1};

// Index of the start node is (sr, sc) => (0,0)
static int32_t sr = 0;
static int32_t sc = 0;

DECL_QUEUE(int32_queue, int32_t, R*C)

int32_t solve(void);

int main(void)
{
  printf("Solved in: %d\n", solve());
  return 0;
}

int32_t explore_neighbors(int32_t r, int32_t c, int32_queue *rqueue, int32_queue *cqueue, uint8_t visited[R][C], int32_t *path);

int32_t solve(void)
{
  uint8_t visited[R][C] = {0};
  uint8_t reached_end = 0;
  int32_t rend, cend;
  int32_t path[R*C] = {0};

  int32_queue rqueue = {0};
  int32_queue cqueue = {0};
  queue_enqueue(rqueue, sr)
  queue_enqueue(cqueue, sc)

  int32_t nodes_left_in_layer = 1;
  int32_t nodes_in_next_layer = 0;
  int32_t move_count = 0;

  visited[sr][sc] = 1;
  while (!queue_is_empty(rqueue) || !queue_is_empty(cqueue))
  {
    int32_t r = queue_dequeue(rqueue);
    int32_t c = queue_dequeue(cqueue);

    if (maze[r][c] == MAZE_ENTRY_END)
    {
      reached_end = 1;
      rend = r;
      cend = c;
      break;
    }

    nodes_in_next_layer = explore_neighbors(r, c, &rqueue, &cqueue, visited, path);
    nodes_left_in_layer--;
    if (nodes_left_in_layer == 0)
    {
      nodes_left_in_layer = nodes_in_next_layer;
      nodes_in_next_layer = 0;
      move_count++;
    }
  }

  if (reached_end)
  {
    int32_t final_path[R*C] = {0};
    int32_t final_path_len = 0;

    int32_t start_idx = sr * C + sc;
    int32_t end_idx = rend * C + cend;

    int32_t at = end_idx;
    while (at != start_idx) {
      final_path[final_path_len++] = at;
      at = path[at];
    }
    final_path[final_path_len++] = start_idx;

    // reverse the list
    int32_t head = 0;
    int32_t tail = final_path_len - 1;
    while (head < tail) {
      int32_t temp = final_path[head];
      final_path[head] = final_path[tail];
      final_path[tail] = temp;

      tail--;
      head++;
    }

    printf("\nPath found! Length: %d moves\n", move_count);
    printf("Path:\n");
    for (int32_t i = 0; i < final_path_len; ++i) {
      int32_t idx = final_path[i];
      int32_t row = idx / C;
      int32_t col = idx % C;
      printf("  Step %d: (%d, %d)", i, row, col);
      if (i == 0) printf(" [START]");
      else if (i == final_path_len - 1) printf(" [END]");
      printf("\n");
    }

    return move_count;
  }

  return 0;
}

int32_t explore_neighbors(int32_t r, int32_t c, int32_queue *rqueue, int32_queue *cqueue, uint8_t visited[R][C], int32_t *path)
{
  int32_t layers = 0;
  for (size_t i = 0; i < 4; ++i)
  {
    int32_t rr = r + dr[i];
    int32_t cc = c + dc[i];

    if (rr < 0 || cc < 0) continue;
    if (rr >= R || cc >= C) continue;

    if (visited[rr][cc]) continue;
    if (maze[rr][cc] == MAZE_ENTRY_ROCK) continue;

    queue_enqueue(*rqueue, rr);
    queue_enqueue(*cqueue, cc);
    visited[rr][cc] = 1;
    path[rr * C + cc] = r * C + c;
    layers++;
  }
  return layers;
}
