#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "arena.c"

#define E_WRONGNUMBEROFARGS 1
#define E_LITERAL_TOO_LARGE 3
#define E_UNKNOWN_TOKEN 4
#define E_UNKNOWN_EXPR 5

#define IS_DIGIT(c) (c >= '0' && c <= '9')

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
  TOKEN_TYPE_PLUS,
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
  arena_t *arena;
  lexer_t lexer;

  token_t curr_token;
  token_t peek_token;
} parser_t;

typedef enum {
  EXPRESSION_TYPE_INTEGER_LITERAL,
  EXPRESSION_TYPE_UNARY_PLUS,
} expression_type_t;

typedef struct {
  expression_type_t t;
  void *v;
} expression_t;

typedef struct {
  token_t tok;
  int value;
} expression_integer_literal_t;

typedef struct {
  expression_t *value;
} expression_unary_plus_t;

typedef int (*prefix_parse_fn_t)(parser_t *, expression_t *out);

int parse_numeric_literal(parser_t *p, expression_t *out);
int parse_unary_plus(parser_t *p, expression_t *out);

static const prefix_parse_fn_t infix_parse_fns[] = {
  [TOKEN_TYPE_PLUS] = parse_unary_plus,
  [TOKEN_TYPE_NUMERIC_LITERAL] = parse_numeric_literal,
};

int parser_next_token(parser_t *p);
int parser_init(parser_t *p, const char *, size_t);
int parser_parse_expression(parser_t *p, expression_t *e);
void debug_expressions(expression_t *root);

typedef enum {
  INSTRUCTION_TYPE_MOV_LITERAL,
} instruction_type_t;

typedef struct {
  instruction_type_t t;
  void *data;
} instruction_t;

typedef struct {
  int lit;
} instruction_move_literal_t;

// IR things
typedef struct {
  instruction_t *data;
  size_t len;
  size_t cap;
} instructions_t;

int push_instruction(arena_t *arena, instructions_t *inst, instruction_t *i);
int emit_mov_literal(arena_t *arena, int value, instructions_t *inst);
int compile_expression_into_ir(arena_t *arena, expression_t *expr, instructions_t *inst);

int compile_instructions_to_x86_64(instructions_t *inst, FILE *fptr);

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
  int err;

#ifdef DEBUG
  printf("expression to compile: '%s'\n", expr);

  lexer_t l;
  lexer_init(&l, expr, len);

  token_t tok;
  while ((err = lexer_next_token(&l, &tok)) == 0&& tok.t != TOKEN_TYPE_EOF)
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
      case TOKEN_TYPE_PLUS:
      {
        printf("token.t = TOKEN_TYPE_PLUS\n");
      }; break;
      default:
        printf("ERROR: token type not supported\n");
    }
  }

  if (err != 0)
  {
    printf("ERROR: code %d\n", err);
    return err;
  }
#endif

  parser_t p = {.arena=&arena};
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

  instructions_t inst = {.cap=0, .len=0};
  if ((err = compile_expression_into_ir(&arena, &root, &inst)) != 0)
  {
    return err;
  }

#ifdef DEBUG
  for (size_t i = 0; i < inst.len; ++i)
  {
    printf("inst[%zu].t = %d\n", i, inst.data[i].t);
  }
#endif

  FILE *outfile = fopen("out.asm", "w");

  if ((err = compile_instructions_to_x86_64(&inst, outfile)) != 0)
  {
    fclose(outfile);
    return err;
  }

  fclose(outfile);

  if ((err = system("nasm -f elf64 out.asm -o out.o")) != 0)
  {
    return err;
  }

  if ((err = system("ld out.o -o out")) != 0)
  {
    return err;
  }

  if ((err = system("rm out.o")) != 0)
  {
    return err;
  }

  if ((err = system("rm out.asm")) != 0)
  {
    return err;
  }


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
  case '+':
    token->t = TOKEN_TYPE_PLUS;
    break;
  default:
    if (IS_DIGIT(lexer->ch))
    {
        token->t = TOKEN_TYPE_NUMERIC_LITERAL; 
        err = lexer_read_integer_literal(lexer, token);
        if (err != 0)
        {
          return err;
        }
    }
    else
    {
        return E_UNKNOWN_TOKEN;
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

  expression_integer_literal_t *lit = arena_alloc(p->arena, sizeof(expression_integer_literal_t));
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

int parse_unary_plus(parser_t *p, expression_t *out)
{
  assert(p != NULL);
  assert(out != NULL);

  expression_unary_plus_t *lit = arena_alloc(p->arena, sizeof(expression_unary_plus_t));
  if (!lit)
  {
    return E_OOM;
  }

  expression_t *right = arena_alloc(p->arena, sizeof(expression_t));
  if (!right)
  {
    return E_OOM;
  }
  int err = parser_next_token(p);
  if (err != 0)
  {
    return err;
  }

  err = parser_parse_expression(p, right);
  if (err != 0)
  {
    return err;
  }

  lit->value = right;

  out->t = EXPRESSION_TYPE_UNARY_PLUS;
  out->v = (void*)lit;

  return 0;
}

// typedef int (*prefix_parse_fn_t)(parser_t *, expression_t *out);
int parser_parse_expression(parser_t *p, expression_t *e)
{
  prefix_parse_fn_t prefix = infix_parse_fns[p->curr_token.t];
  int err = prefix(p, e);
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
    case EXPRESSION_TYPE_UNARY_PLUS:
    {
      expression_unary_plus_t *lit = (expression_unary_plus_t*)root->v;
      printf("+");
      debug_expressions(lit->value);
    }; break;
  }
}

int push_instruction(arena_t *arena, instructions_t *inst, instruction_t *i)
{
  assert(arena != NULL);
  assert(inst != NULL);
  assert(i != NULL);

  if (inst->len + 1 >= inst->cap)
  {
    inst->cap = inst->cap == 0 ? 16 : inst->cap*2;
#ifdef DEBUG
    printf("reallocating instructions...\n");
    printf("new capacity: %zu\n", inst->cap);
#endif
    instruction_t *data = arena_alloc(arena, sizeof(instruction_t) * inst->cap);
    if (data == NULL)
    {
      return E_OOM;
    }

    for (size_t i = 0; i < inst->len; ++i)
    {
      data[i].t = inst->data[i].t;
      data[i].data = inst->data[i].data;
    }

    inst->data = data;
  }

  inst->data[inst->len++] = *i;
  return 0;
}

int emit_mov_literal(arena_t *arena, int value, instructions_t *inst)
{
  assert(arena != NULL);
  assert(inst != NULL);

  instruction_move_literal_t *movlit = arena_alloc(arena, sizeof(instruction_move_literal_t));
  if (movlit == NULL)
  {
    return E_OOM;
  }

  movlit->lit = value;

  instruction_t *instruction = arena_alloc(arena, sizeof(instruction_t));
  if (instruction == NULL)
  {
    return E_OOM;
  }

  instruction->t = INSTRUCTION_TYPE_MOV_LITERAL;
  instruction->data = (void*)movlit;

#ifdef DEBUG
  printf("Emmiting mov instruction, value: %d\n", movlit->lit);
#endif

  return push_instruction(arena, inst, instruction);
}

int compile_expression_into_ir(arena_t *arena, expression_t *expr, instructions_t *inst)
{
  assert(arena != NULL);
  assert(expr != NULL);
  assert(inst != NULL);

  int err;

  switch (expr->t)
  {
    case EXPRESSION_TYPE_INTEGER_LITERAL:
    {
      expression_integer_literal_t *lit = (expression_integer_literal_t*)expr->v;
      if ((err = emit_mov_literal(arena, lit->value, inst)) != 0) 
      {
          return err;
      }
    }; break;
    case EXPRESSION_TYPE_UNARY_PLUS:
    {
      // Just a noop
      expression_unary_plus_t *lit = (expression_unary_plus_t*)expr->v;
      if ((err = compile_expression_into_ir(arena, lit->value, inst)) != 0)
      {
          return err;
      }
    }; break;
    default: return E_UNKNOWN_EXPR;
  }

  return 0;
}

int compile_instructions_to_x86_64(instructions_t *inst, FILE *fptr)
{
  assert(inst != NULL);
  assert(fptr != NULL);
  const char *header = "bits 64\n"
    "section .text\n"
    "global _start\n"
    "\n_start:\n";

  fprintf(fptr, "%s", header);

  for (size_t i = 0; i < inst->len; ++i)
  {
    instruction_t *aux = &inst->data[i];
    switch(aux->t)
    {
      case INSTRUCTION_TYPE_MOV_LITERAL:
      {
          instruction_move_literal_t *movlit = (instruction_move_literal_t*) aux->data;
          fprintf(fptr, "\tmov rax, %d\n", movlit->lit);
      }; break;
    }
  }

  // Printing the stuff that were in rax
  fprintf(fptr, "\tmov rbx, 10\n");
  fprintf(fptr, "\tcall print_number\n");

  const char *exit_asm = "\n\tmov rax, 60\n"
    "\txor rdi, rdi\n"
    "\tsyscall\n";

  fprintf(fptr, "%s", exit_asm);

  const char *print_number_proc ="print_number:\n"
    "\tpush rbx\n"
    "\tpush rcx\n"
    "\tpush rdx\n"
    "\tsub rsp, 64\n"
    "\tlea rsi, [rsp + 64]\n"
    "\tmov rcx, 0\n"
    "\tdec rsi\n"
    "\tmov byte [rsi], 0\n"
    "\tinc rcx\n"
    "\tdec rsi\n"
    "\tmov byte [rsi], 10\n"
    "\tinc rcx\n"
    "\tcmp rax, 0\n"
    "\tjne .build_loop\n"
    "\tdec rsi\n"
    "\tmov byte [rsi], '0'\n"
    "\tinc rcx\n"
    "\tjmp .do_write\n"
".build_loop:\n"
".loop:\n"
    "\txor rdx, rdx\n"
    "\tdiv rbx\n"
    "\tmov r8, rdx\n"
    "\tcmp r8, 9\n"
    "\tjle .digit_dec\n"
    "\tadd r8, 7\n"
".digit_dec:\n"
    "\tadd r8, 48\n"
    "\tdec rsi\n"
    "\tmov byte [rsi], r8b\n"
    "\tinc rcx\n"
    "\tcmp rax, 0\n"
    "\tjne .loop\n"
".do_write:\n"
    "\tmov rdi, 1\n"
    "\tmov rdx, rcx\n"
    "\tmov rax, 1\n"
    "\tsyscall\n"
    "\tadd rsp, 64\n"
    "\tpop rdx\n"
    "\tpop rcx\n"
    "\tpop rbx\n"
    "\tret\n";

  fprintf(fptr, "%s", print_number_proc);

  return 0;
}

