#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "arena.c"

#define E_WRONGNUMBEROFARGS 1
#define E_LITERAL_TOO_LARGE 3
#define E_UNKNOWN_TOKEN 4
#define E_UNKNOWN_EXPR 5
#define E_NOT_IMPLEMENTED 6
#define E_INVALID_PREFIX_FN 7
#define E_BADREGISTER 8

#define IS_DIGIT(c) (c >= '0' && c <= '9')

#define PREDENCE_LOWEST     1
#define PREDENCE_SUM        2
#define PREDENCE_PRODUCT    3
#define PREDENCE_PREFIX     4

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
  TOKEN_TYPE_MINUS,
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
  EXPRESSION_TYPE_UNARY_MINUS,
  EXPRESSION_TYPE_BINARY_PLUS,
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

typedef struct {
  expression_t *value;
} expression_unary_minus_t;

typedef struct {
  expression_t *left;
  expression_t *right;
} expression_binary_plus_t;

typedef int (*prefix_parse_fn_t)(parser_t *, expression_t *out);
typedef int (*infix_parse_fn_t)(parser_t *, expression_t *left, expression_t *out);

int parse_numeric_literal(parser_t *p, expression_t *out);
int parse_unary_plus(parser_t *p, expression_t *out);
int parse_unary_minus(parser_t *p, expression_t *out);

int parse_binary_plus(parser_t *p, expression_t *left, expression_t *out);
int parse_binary_minus(parser_t *p, expression_t *left, expression_t *out);

#define PREFIX_PARSERS 3

static const token_type_t prefix_parse_fns_ts[PREFIX_PARSERS] = {
  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_NUMERIC_LITERAL,
};

static const prefix_parse_fn_t prefix_parse_fns[PREFIX_PARSERS] = {
  parse_unary_plus,
  parse_unary_minus,
  parse_numeric_literal,
};

#define INFIX_PARSERS 2

static const token_type_t infix_parse_fns_ts[INFIX_PARSERS] = {
  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
};

static const infix_parse_fn_t infix_parse_fns[INFIX_PARSERS] = {
  parse_binary_plus,
  parse_binary_minus,
};

int prefix_parse_fns_lookup(token_type_t v, prefix_parse_fn_t *out);
int infix_parse_fns_lookup(token_type_t v, infix_parse_fn_t *out);

int parser_next_token(parser_t *p);
char parser_peek_token_is(parser_t *p, token_type_t v);
int parser_peek_precedence(token_type_t v);
int parser_init(parser_t *p, const char *, size_t);
int parser_parse_expression(parser_t *p, expression_t **e, int precedence);
void debug_expressions(expression_t *root);

typedef enum {
  INSTRUCTION_TYPE_MOV_LITERAL,
  INSTRUCTION_TYPE_MUL_CONSTANT,
  INSTRUCTION_TYPE_ADD_BINARY,
} instruction_type_t;

typedef struct {
  instruction_type_t t;
  void *data;
} instruction_t;

typedef enum
{
  REG1, REG2,
} reg_t;

typedef struct {
  int lit;
  reg_t r;
} instruction_move_literal_t;

typedef struct {
  int lit;
  reg_t r;
} instruction_mul_constant_t;

typedef struct {
  reg_t reg_a;
  reg_t reg_b;
} instruction_add_expressions_t;

// IR things
typedef struct {
  instruction_t *data;
  size_t len;
  size_t cap;
} instructions_t;

int push_instruction(arena_t *arena, instructions_t *inst, instruction_t *i);

int emit_mul_instruction(arena_t *arena, int value, reg_t reg, instructions_t *inst);
int emit_add_instruction(arena_t *arena, reg_t reg_a, reg_t reg_b, instructions_t *inst);
int emit_mov_literal(arena_t *arena, int value, reg_t reg, instructions_t *inst);
int compile_expression_into_ir(arena_t *arena, expression_t *expr, reg_t reg, instructions_t *inst);

const char *compile_get_register_name_from_enum(reg_t reg);
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
      case TOKEN_TYPE_MINUS:
      {
        printf("token.t = TOKEN_TYPE_MINUS\n");
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
    printf("ERROR: Failed to initialize parser: Code = %d\n", err);
    return err;
  }

  expression_t *root = arena_alloc(&arena, sizeof(expression_t));
  if ((err = parser_parse_expression(&p, &root, PREDENCE_LOWEST)) != 0)
  {
    printf("ERROR: Failed to parse expression: Code = %d\n", err);
    return err;
  }
#ifdef DEBUG
  debug_expressions(root);
  puts("");
#endif

  instructions_t inst = {.cap=0, .len=0};
  if ((err = compile_expression_into_ir(&arena, root, REG1, &inst)) != 0)
  {
    printf("ERROR: Failed to compile expression into IR: Code = %d\n", err);
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
    printf("ERROR: Failed to compile expression into asm: Code = %d\n", err);
    fclose(outfile);
    return err;
  }

  fclose(outfile);

  if ((err = system("nasm -f elf64 out.asm -o out.o")) != 0)
  {
    printf("ERROR: Failed to compile assembly: Code = %d\n", err);
    return err;
  }

  if ((err = system("ld out.o -o out")) != 0)
  {
    printf("ERROR: Failed to link code: Code = %d\n", err);
    return err;
  }

  if ((err = system("rm out.o")) != 0)
  {
    printf("ERROR: Failed to remove junk: Code = %d\n", err);
    return err;
  }

  if ((err = system("rm out.asm")) != 0)
  {

    printf("ERROR: Failed to remove junk: Code = %d\n", err);
    return err;
  }

  arena_free(&arena);

  printf("Compiled expression ./out\n");
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
  case '-':
    token->t = TOKEN_TYPE_MINUS;
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
        return 0;
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
  t->len = l->position - pos;

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

char parser_peek_token_is(parser_t *p, token_type_t v)
{
  return p->peek_token.t == v;
}

int parser_peek_precedence(token_type_t v)
{
  switch (v)
  { 
    case TOKEN_TYPE_PLUS: return PREDENCE_SUM;
    case TOKEN_TYPE_MINUS: return PREDENCE_SUM;

    default:
      return PREDENCE_LOWEST;
  }
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

  err = parser_parse_expression(p, &right, PREDENCE_PREFIX);
  if (err != 0)
  {
    return err;
  }

  lit->value = right;

  out->t = EXPRESSION_TYPE_UNARY_PLUS;
  out->v = (void*)lit;

  return 0;
}

int parse_unary_minus(parser_t *p, expression_t *out)
{
  assert(p != NULL);
  assert(out != NULL);

  expression_unary_minus_t *lit = arena_alloc(p->arena, sizeof(expression_unary_minus_t));
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

  err = parser_parse_expression(p, &right, PREDENCE_PREFIX);
  if (err != 0)
  {
    return err;
  }

  lit->value = right;

  out->t = EXPRESSION_TYPE_UNARY_MINUS;
  out->v = (void*)lit;

  return 0;
}

int parse_binary_plus(parser_t *p, expression_t *left, expression_t *out)
{
  assert(p != NULL);
  assert(left != NULL);
  assert(out != NULL);

  expression_binary_plus_t *binplus = arena_alloc(p->arena, sizeof(expression_binary_plus_t));
  if (binplus == NULL)
  {
    return E_OOM;
  }

  binplus->left = left;
  int precedence = parser_peek_precedence(p->curr_token.t);
  parser_next_token(p);

  expression_t *right = arena_alloc(p->arena, sizeof(expression_t));
  if (right == NULL)
  {
    return E_OOM;
  }

  int err = parser_parse_expression(p, &right, precedence);
  if (err != 0)
  {
    return err;
  }

  binplus->right = right;

  out->v = (void *)binplus;
  out->t = EXPRESSION_TYPE_BINARY_PLUS;

  return 0;
}

int parse_binary_minus(parser_t *p, expression_t *left, expression_t *out)
{
  assert(p != NULL);
  assert(left != NULL);
  assert(out != NULL);

  return 0;
}

int prefix_parse_fns_lookup(token_type_t v, prefix_parse_fn_t *out)
{
  for (size_t i = 0; i < PREFIX_PARSERS; ++i)
  {
    if (prefix_parse_fns_ts[i] == v)
    {
      *out = prefix_parse_fns[i];
      return 0;
    }
  }
  return E_INVALID_PREFIX_FN;
}

int infix_parse_fns_lookup(token_type_t v, infix_parse_fn_t *out)
{
  for (size_t i = 0; i < INFIX_PARSERS; ++i)
  {
    if (infix_parse_fns_ts[i] == v)
    {
      *out = infix_parse_fns[i];
      return 0;
    }
  }
  return E_INVALID_PREFIX_FN;

}

int parser_parse_expression(parser_t *p, expression_t **e, int precedence)
{
  prefix_parse_fn_t prefix;
  int err = prefix_parse_fns_lookup(p->curr_token.t, &prefix);
  if (err != 0)
  {
    return err;
  }

  err = prefix(p, *e);

  while (!parser_peek_token_is(p, TOKEN_TYPE_EOF) && precedence < parser_peek_precedence(p->peek_token.t))
  {
    infix_parse_fn_t infix;
    if ((err = infix_parse_fns_lookup(p->peek_token.t, &infix)) != 0)
    {
      return err;
    }

    parser_next_token(p);

    expression_t *expr = arena_alloc(p->arena, sizeof(expression_t));
    if (expr == NULL)
    {
      return E_OOM;
    }

    err = infix(p, *e, expr);
    if (err != 0)
    {
      return err;
    }

    *e = expr;
  }

  return err;
}

void debug_expressions(expression_t *root)
{
  switch (root->t)
  {
    case EXPRESSION_TYPE_INTEGER_LITERAL:
    {
      expression_integer_literal_t *lit = (expression_integer_literal_t*)root->v;
      printf("(%d)", lit->value);
    }; break;
    case EXPRESSION_TYPE_UNARY_PLUS:
    {
      expression_unary_plus_t *lit = (expression_unary_plus_t*)root->v;
      printf("+");
      debug_expressions(lit->value);
    }; break;
    case EXPRESSION_TYPE_UNARY_MINUS:
    {
      expression_unary_minus_t *lit = (expression_unary_minus_t*)root->v;
      printf("-");
      debug_expressions(lit->value);
    }; break;
    case EXPRESSION_TYPE_BINARY_PLUS:
    {
      expression_binary_plus_t *lit = (expression_binary_plus_t*)root->v;
      printf("(");
      debug_expressions(lit->left);
      printf("+");
      debug_expressions(lit->right);
      printf(")");
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

int emit_mov_literal(arena_t *arena, int value, reg_t reg, instructions_t *inst)
{
  assert(arena != NULL);
  assert(inst != NULL);

  instruction_move_literal_t *movlit = arena_alloc(arena, sizeof(instruction_move_literal_t));
  if (movlit == NULL)
  {
    return E_OOM;
  }

  movlit->lit = value;
  movlit->r = reg;

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

int emit_mul_instruction(arena_t *arena, int value, reg_t reg, instructions_t *inst)
{
  assert(arena != NULL);
  assert(inst != NULL);

  instruction_mul_constant_t *mulconst = arena_alloc(arena, sizeof(instruction_mul_constant_t));
  if (mulconst == NULL)
  {
    return E_OOM;
  }

  mulconst->lit = value;
  mulconst->r = reg;

  instruction_t *instruction = arena_alloc(arena, sizeof(instruction_t));
  if (instruction == NULL)
  {
    return E_OOM;
  }

  instruction->t = INSTRUCTION_TYPE_MUL_CONSTANT;
  instruction->data = (void*)mulconst;

#ifdef DEBUG
  printf("Emmiting mul instruction, value: %d\n", mulconst->lit);
#endif

  return push_instruction(arena, inst, instruction);

}

int emit_add_instruction(arena_t *arena, reg_t reg_a, reg_t reg_b, instructions_t *inst)
{
  assert(arena != NULL);
  assert(inst != NULL);

  instruction_add_expressions_t *addexpr = arena_alloc(arena, sizeof(instruction_add_expressions_t));
  if (addexpr == NULL)
  {
    return E_OOM;
  }

  addexpr->reg_a = reg_a;
  addexpr->reg_b = reg_b;

  instruction_t *instruction = arena_alloc(arena, sizeof(instruction_t));
  if (instruction == NULL)
  {
    return E_OOM;
  }

  instruction->t = INSTRUCTION_TYPE_ADD_BINARY;
  instruction->data = (void*)addexpr;

  return push_instruction(arena, inst, instruction);
}

int compile_expression_into_ir(arena_t *arena, expression_t *expr, reg_t reg, instructions_t *inst)
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
      if ((err = emit_mov_literal(arena, lit->value, reg, inst)) != 0) 
      {
          return err;
      }
    }; break;
    case EXPRESSION_TYPE_UNARY_PLUS:
    {
      // Just a noop
      expression_unary_plus_t *lit = (expression_unary_plus_t*)expr->v;
      if ((err = compile_expression_into_ir(arena, lit->value, reg, inst)) != 0)
      {
          return err;
      }
    }; break;
    case EXPRESSION_TYPE_UNARY_MINUS:
    {
      expression_unary_minus_t *lit = (expression_unary_minus_t*)expr->v;
      if ((err = compile_expression_into_ir(arena, lit->value, reg, inst)) != 0)
      {
          return err;
      }

      if ((err = emit_mul_instruction(arena, -1, reg, inst)) != 0) 
      {
          return err;
      }
    }; break;
    case EXPRESSION_TYPE_BINARY_PLUS:
    {
      expression_binary_plus_t *lit = (expression_binary_plus_t*)expr->v;
      if ((err = compile_expression_into_ir(arena, lit->left, REG1, inst)) != 0)
      {
          return err;
      }

      if ((err = compile_expression_into_ir(arena, lit->right, REG2, inst)) != 0)
      {
          return err;
      }

      if ((err = emit_add_instruction(arena, REG1, REG2, inst)) != 0)
      {
          return err;
      }
    }; break;
    default: return E_UNKNOWN_EXPR;
  }

  return 0;
}

const char *compile_get_register_name_from_enum(reg_t reg)
{
  switch (reg)
  {
    case REG1: return "rax";
    case REG2: return "rbx";
    default: return NULL;
  }
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
          const char *reg_name = compile_get_register_name_from_enum(movlit->r);
          if (reg_name == NULL)
          {
            return E_BADREGISTER;
          }
          fprintf(fptr, "\tmov %s, %d\n", reg_name, movlit->lit);
      }; break;
      case INSTRUCTION_TYPE_MUL_CONSTANT:
      {
          instruction_mul_constant_t *mulconst = (instruction_mul_constant_t*) aux->data;
          if (mulconst->lit == -1)
          {
            fprintf(fptr, "\tneg rax\n");
          }
          else
          {
            return E_NOT_IMPLEMENTED;
          }
      }; break;
      case INSTRUCTION_TYPE_ADD_BINARY:
      {
        instruction_add_expressions_t *addexpr = (instruction_add_expressions_t*)aux->data;
        // reg_a => rax
        // reg_b => rbx
        if (addexpr->reg_a != REG1)
        {
            return E_BADREGISTER;
        }

        if (addexpr->reg_b != REG2)
        {
            return E_BADREGISTER;
        }

        fprintf(fptr, "\tadd rax, rbx\n");
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


  const char *print_number_proc =
    "print_number:\n"
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
    "\tjne .check_negative\n"
    "\tdec rsi\n"
    "\tmov byte [rsi], '0'\n"
    "\tinc rcx\n"
    "\tjmp .do_write\n"
    ".check_negative:\n"
    "\tmov r9, 0\n"
    "\tcmp rax, 0\n"
    "\tjge .build_loop\n"
    "\tneg rax\n"
    "\tmov r9, 1\n"
    ".build_loop:\n"
    ".loop:\n"
    "\txor rdx, rdx\n"
    "\tmov rbx, 10\n"
    "\tdiv rbx\n"
    "\tadd dl, '0'\n"
    "\tdec rsi\n"
    "\tmov [rsi], dl\n"
    "\tinc rcx\n"
    "\ttest rax, rax\n"
    "\tjnz .loop\n"
    "\tcmp r9, 0\n"
    "\tje .do_write\n"
    "\tdec rsi\n"
    "\tmov byte [rsi], '-'\n"
    "\tinc rcx\n"
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

