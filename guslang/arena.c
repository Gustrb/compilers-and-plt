#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct arena_chunk_t {
  char initialized;

  char *data;
  size_t used;
} arena_chunk_t;

#define E_OOM 200 + 1

#define ARENA_MAX_CHUNKS 1024
#define ARENA_CHUNK_SIZE 4096

typedef struct {
  arena_chunk_t chunks[ARENA_MAX_CHUNKS];
  size_t num_chunks;
} arena_t;

int arena_initialize_chunk(arena_chunk_t *chunk);
void *arena_chunk_allocate(arena_chunk_t *chunk, size_t s);
void *arena_alloc(arena_t *arena, size_t s);
void arena_free(arena_t *arena);
void arena_init(arena_t *arena);

int arena_initialize_chunk(arena_chunk_t *chunk) {
  chunk->initialized = 1;
  chunk->data = malloc(ARENA_CHUNK_SIZE);
  if (chunk->data == NULL) {
    return E_OOM;
  }
  chunk->used = 0;
  return 0;
}

void *arena_chunk_allocate(arena_chunk_t *chunk, size_t s) {
  char *start = chunk->data + chunk->used;
  chunk->used += s;
  return start;
}

void *arena_alloc(arena_t *arena, size_t s) {
  assert(arena != NULL);

  for (size_t i = 0; i < arena->num_chunks; ++i) {
    arena_chunk_t *aux = &arena->chunks[i];
    if (aux->initialized && (aux->used + s < ARENA_CHUNK_SIZE)) {
      return arena_chunk_allocate(aux, s);
    }
  }

  if (arena->num_chunks >= ARENA_MAX_CHUNKS) {
    return NULL;
  }

  arena_chunk_t *aux = &arena->chunks[arena->num_chunks++];
  int err;
  if ((err = arena_initialize_chunk(aux)) != 0) {
    return NULL;
  }

  return arena_chunk_allocate(aux, s);
}

void arena_free(arena_t *arena) {
  for (size_t i = 0; i < arena->num_chunks; ++i) {
    arena_chunk_t *c = &arena->chunks[i];
    if (c->initialized)
      free(c->data);
  }
}

void arena_init(arena_t *arena) {
  memset(arena->chunks, 0, sizeof(arena_chunk_t) * ARENA_MAX_CHUNKS);
  arena->num_chunks = 0;
}
