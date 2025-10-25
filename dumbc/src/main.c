#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define E_WRONGNUMBEROFARGS 1
#define E_OOM 2
#define E_LITERAL_TOO_LARGE 3

#define IS_DIGIT(c) (c >= '0' && c <= '9')

typedef struct arena_chunk_t {
  char initialized;

  char *data;
  size_t used;
} arena_chunk_t;

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

typedef struct {
  const char *expr;
  size_t expr_len;

  size_t position;
  size_t read_position;
  char ch;
} lexer_t;

void lexer_init(lexer_t *lexer, const char *expr, size_t len);
void lexer_read_char(lexer_t *lexer);

typedef enum {
  TOKEN_TYPE_NUMERIC_LITERAL,
  TOKEN_TYPE_EOF,
} token_type_t;

typedef struct {
  token_type_t t;

  const char *start;
  size_t len;
} token_t;

int lexer_next_token(lexer_t *lexer, token_t *token);
int lexer_read_integer_literal(lexer_t *l, token_t *t);
int lexer_skip_whitespace(lexer_t *lexer);

// Parser things
typedef struct {
  arena_t arena;
  lexer_t lexer;

  token_t curr_token;
  token_t peek_token;
} parser_t;

typedef enum {
  EXPRESSION_TYPE_INTEGER_LITERAL,
} expression_type_t;

typedef struct {
  expression_type_t t;
  void *v;
} expression_t;

typedef struct {
  token_t tok;
  int value;
} expression_integer_literal_t;

typedef int (*prefix_parse_fn_t)(parser_t *, expression_t *out);

int parse_numeric_literal(parser_t *p, expression_t *out);

static const prefix_parse_fn_t infix_parse_fns[] = {
  [TOKEN_TYPE_NUMERIC_LITERAL] = parse_numeric_literal,
};

int parser_next_token(parser_t *p);
int parser_init(parser_t *p, const char *, size_t);
int parser_parse_expression(parser_t *p, expression_t *e);
void debug_expressions(expression_t *root);

int main(int argc, const char **argv)
{
  arena_t arena;
  arena_init(&arena);
  if (argc != 2)
  {
    printf("you dumb fuck use only one argument plis\n");
    return E_WRONGNUMBEROFARGS;
  }

  const char *expr = argv[1];
  size_t len = strlen(expr);

#ifdef DEBUG
  printf("expression to compile: '%s'\n", expr);

  lexer_t l;
  lexer_init(&l, expr, len);

  token_t tok;
  while ((lexer_next_token(&l, &tok) == 0) && tok.t != TOKEN_TYPE_EOF)
  {
    switch (tok.t)
    {
      case TOKEN_TYPE_NUMERIC_LITERAL:
      {
        printf("token.t = TOKEN_TYPE_NUMERIC_LITERAL: ");
        for (size_t i = 0; i < tok.len; ++i)
        {
          printf("%c", tok.start[i]);
        }
        printf("\n");
      }; break;
      default:
        printf("ERROR: token type not supported\n");
    }
  }
#endif

  parser_t p = {.arena=arena};
  int err;
  if ((err = parser_init(&p, expr, len)) != 0)
  {
    return err;
  }

  expression_t root;
  if ((err = parser_parse_expression(&p, &root)) != 0)
  {
    return err;
  }
#ifdef DEBUG
  debug_expressions(&root);
#endif

  arena_free(&arena);
  return 0;
}

void lexer_init(lexer_t *lexer, const char *expr, size_t len)
{
  assert(lexer != NULL);

  lexer->expr = expr;
  lexer->expr_len = len;
  lexer->position = 0;
  lexer->read_position = 0;
  lexer->ch = 0;

  lexer_read_char(lexer);
}

void lexer_read_char(lexer_t *lexer)
{
  assert(lexer != NULL);
  if (lexer->read_position >= lexer->expr_len)
  {
    lexer->ch = 0;
  }
  else
  {
    lexer->ch = lexer->expr[lexer->read_position];
  }

  lexer->position = lexer->read_position;
  lexer->read_position++;
}

int lexer_next_token(lexer_t *lexer, token_t *token)
{
  assert(lexer != NULL);
  assert(token != NULL);

  int err;
  if ((err = lexer_skip_whitespace(lexer)) != 0)
  {
    return err;
  }

  switch (lexer->ch)
  {
  case 0:
    token->t = TOKEN_TYPE_EOF;
    return 0;
  default:
    if (IS_DIGIT(lexer->ch))
    {
        token->t = TOKEN_TYPE_NUMERIC_LITERAL; 
        err = lexer_read_integer_literal(lexer, token);
        return err;
    }
    break;
  }

  lexer_read_char(lexer);
  return 0;
}

int lexer_skip_whitespace(lexer_t *l)
{
  assert(l != NULL);

	while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
    lexer_read_char(l);
	}

  return 0;
}

int lexer_read_integer_literal(lexer_t *l, token_t *t)
{
  assert(l != NULL);
  assert(t != NULL);

  size_t pos = l->position;

  while (IS_DIGIT(l->ch)) {
    lexer_read_char(l);
  }

  t->start = l->expr + pos;
  t->len = l->position;

  return 0;
}

int parser_init(parser_t *p, const char *expr, size_t len)
{
  assert(p != NULL);
  assert(expr != NULL);

  lexer_init(&p->lexer, expr, len);

  int err;
  if ((err = parser_next_token(p)) != 0) return err;
  if ((err = parser_next_token(p)) != 0) return err;

  return 0;
}

int parser_next_token(parser_t *p)
{
  assert(p != NULL);

  p->curr_token = p->peek_token;
  int err;
  if ((err = lexer_next_token(&p->lexer, &p->peek_token)) != 0)
  {
    return err;
  }

  return 0;
}

int parse_numeric_literal(parser_t *p, expression_t *out)
{
  assert(p != NULL);
  assert(out != NULL);


  expression_integer_literal_t *lit = arena_alloc(&p->arena, sizeof(expression_integer_literal_t));
  if (!lit)
  {
    return E_OOM;
  }
  
  char buff[33];
  if (p->curr_token.len > 32)
  {
    return E_LITERAL_TOO_LARGE;
  }

  for (size_t i = 0; i < p->curr_token.len; ++i)
  {
    buff[i] = p->curr_token.start[i];
  }

  buff[p->curr_token.len] = '\0';

  int d = atoi(buff);

  lit->value = d;

  out->t = EXPRESSION_TYPE_INTEGER_LITERAL;
  out->v = (void *)lit;

  return 0;
}

// typedef int (*prefix_parse_fn_t)(parser_t *, expression_t *out);
int parser_parse_expression(parser_t *p, expression_t *e)
{
  prefix_parse_fn_t fn = infix_parse_fns[p->curr_token.t];
  int err = fn(p, e);
  return err;
}

void debug_expressions(expression_t *root)
{
  switch (root->t)
  {
    case EXPRESSION_TYPE_INTEGER_LITERAL:
    {
      expression_integer_literal_t *lit = (expression_integer_literal_t*)root->v;
      printf("(%d)\n", lit->value);
    }; break;
  }
}

int arena_initialize_chunk(arena_chunk_t *chunk)
{
  chunk->initialized = 1;
  chunk->data = malloc(ARENA_CHUNK_SIZE);
  if (chunk->data == NULL)
  {
    return E_OOM;
  }
  chunk->used = 0;
  return 0;
}

void *arena_chunk_allocate(arena_chunk_t *chunk, size_t s)
{
  char *start = chunk->data + chunk->used;
  chunk->used += s;
  return start;
}

void *arena_alloc(arena_t *arena, size_t s)
{
  assert(arena != NULL);

  for (size_t i = 0; i < arena->num_chunks; ++i)
  {
    arena_chunk_t *aux = &arena->chunks[i];
    if (aux->initialized && (aux->used + s < ARENA_CHUNK_SIZE))
    {
      return arena_chunk_allocate(aux, s);
    }
  }

  if (arena->num_chunks >= ARENA_MAX_CHUNKS)
  {
    return NULL;
  }

  arena_chunk_t *aux = &arena->chunks[arena->num_chunks++];
  int err;
  if ((err = arena_initialize_chunk(aux)) != 0)
  {
    return NULL;
  }

  return arena_chunk_allocate(aux, s);
}

void arena_free(arena_t *arena)
{
  for (size_t i = 0; i < arena->num_chunks; ++i)
  {
    arena_chunk_t *c = &arena->chunks[i];
    if (c->initialized) free(c->data);
  }
}

void arena_init(arena_t *arena)
{
  memset(arena->chunks, 0, sizeof(arena_chunk_t) * ARENA_MAX_CHUNKS);
  arena->num_chunks = 0;
}
