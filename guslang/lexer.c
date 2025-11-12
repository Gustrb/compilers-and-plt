#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#define IS_WHITESPACE(ch) (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
#define IS_LETTER(ch)                                                          \
  ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
#define IS_DIGIT(ch) (ch >= '0' && ch < '9')

#define ERR_UNTERMINATED_STRING 100 + 1

typedef struct {
  char *buff;
  uint64_t len;

  uint64_t pos;
  uint64_t read_pos;

  char ch;
} lexer_t;

typedef enum {
  // Keywords
  TOKEN_TYPE_PRINT,
  TOKEN_TYPE_END,

  TOKEN_TYPE_NUMERIC_LITERAL,
  TOKEN_TYPE_STRING_LITERAL,
  TOKEN_TYPE_IDENTIFIER,

  TOKEN_TYPE_EOF,
} token_type_t;

typedef struct {
  token_type_t type;

  const char *start;
  uint64_t len;
} token_t;

static void __lexer_skip_whitespace(lexer_t *);
static token_type_t __lexer_figure_out_token_type(const char *buff,
                                                  uint64_t len);

static void __lexer_read_char(lexer_t *);
static void __lexer_read_identifier(lexer_t *l, token_t *t);
static void __lexer_read_integer_literal(lexer_t *l, token_t *t);
static int32_t __lexer_read_string_literal(lexer_t *l, token_t *t);

int32_t lexer_next_token(lexer_t *l, token_t *tok);
void lexer_init(lexer_t *l, char *buff, uint64_t len);

int32_t lexer_next_token(lexer_t *l, token_t *tok) {
  assert(l != NULL);
  assert(tok != NULL);

  __lexer_skip_whitespace(l);

  int32_t err;

  switch (l->ch) {
  case 0:
    tok->type = TOKEN_TYPE_EOF;
    return 0;
  case '"':
    if ((err = __lexer_read_string_literal(l, tok)) != 0) {
      return err;
    }

    tok->type = TOKEN_TYPE_STRING_LITERAL;
    break;
  default:
    if (IS_LETTER(l->ch)) {
      __lexer_read_identifier(l, tok);
      tok->type = __lexer_figure_out_token_type(tok->start, tok->len);
      return 0;
    } else if (IS_DIGIT(l->ch)) {
      tok->type = TOKEN_TYPE_NUMERIC_LITERAL;
      __lexer_read_integer_literal(l, tok);
      return 0;
    }
  }

  __lexer_read_char(l);
  return 0;
}

void lexer_init(lexer_t *l, char *buff, uint64_t len) {
  assert(l != NULL);
  assert(buff != NULL);
  assert(len > 0);

  l->buff = buff;
  l->len = len;
  l->read_pos = 0;
  l->pos = 0;
  __lexer_read_char(l);
}

static void __lexer_read_char(lexer_t *l) {
  assert(l != NULL);
  if (l->read_pos >= l->len) {
    l->ch = 0;
  } else {
    l->ch = l->buff[l->read_pos];
  }

  l->pos = l->read_pos;
  l->read_pos++;
}

static void __lexer_skip_whitespace(lexer_t *l) {
  assert(l != NULL);

  while (IS_WHITESPACE(l->ch)) {
    __lexer_read_char(l);
  }
}

static void __lexer_read_integer_literal(lexer_t *l, token_t *t) {
  assert(l != NULL);
  assert(t != NULL);

  size_t pos = l->pos;

  while (IS_DIGIT(l->ch)) {
    __lexer_read_char(l);
  }

  t->start = l->buff + pos;
  t->len = l->pos - pos;
}

static void __lexer_read_identifier(lexer_t *l, token_t *t) {
  assert(l != NULL);
  assert(t != NULL);

  size_t pos = l->pos;

  while (IS_LETTER(l->ch)) {
    __lexer_read_char(l);
  }

  t->start = l->buff + pos;
  t->len = l->pos - pos;
}

static token_type_t __lexer_figure_out_token_type(const char *buff,
                                                  uint64_t len) {
  assert(buff != NULL);
  assert(len > 0);

  if (strncmp(buff, "END", len) == 0) {
    return TOKEN_TYPE_END;
  }

  if (strncmp(buff, "PRINT", len) == 0) {
    return TOKEN_TYPE_PRINT;
  }

  return TOKEN_TYPE_IDENTIFIER;
}

static int32_t __lexer_read_string_literal(lexer_t *l, token_t *t) {
  assert(l != NULL);
  assert(t != NULL);

  size_t pos = l->pos;

  while (1) {
    __lexer_read_char(l);
    if (l->ch == '"' || l->ch == 0) {
      break;
    }
  }

  if (l->ch == 0) {
    return ERR_UNTERMINATED_STRING;
  }

  t->start = l->buff + pos;
  t->len = l->pos - pos;

  return 0;
}
