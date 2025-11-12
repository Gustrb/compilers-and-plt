#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GUSLANG_PROMPT "> "
#define LINE_LEN 1024

#define ERR_FAILED_TO_WRITE_TO_SCREEN 10 + 1
#define ERR_FAILED_TO_READ_FROM_STREAM 10 + 2

typedef struct {
  int32_t argc;
  char **argv;

  FILE *out_stream;
  FILE *in_stream;

  char linebuff[LINE_LEN];
} cli_context_t;

void cli_context_init(cli_context_t *ctx, int32_t argc, char **argv) {
  assert(ctx != NULL);
  assert(argc > 0);
  assert(argv != NULL);

  ctx->argc = argc;
  ctx->argv = argv;

  ctx->out_stream = stdout;
  ctx->in_stream = stdin;

  memset(ctx->linebuff, 0, sizeof(char) * LINE_LEN);
}

int32_t cli_display_prompt(cli_context_t *ctx) {
  assert(ctx != NULL);
  assert(ctx->out_stream != NULL);

  // UNIX syscall wrapper returns an int, we want to turn that into our err
  // code, which is of type int32_t (alias on most platforms I guess)
  int err = fprintf(ctx->out_stream, GUSLANG_PROMPT);
  if (err < 0) {
    // TODO: here we are not caring about errno, maybe it would make sense...
    return ERR_FAILED_TO_WRITE_TO_SCREEN;
  }

  return 0;
}

int32_t cli_read_user_input(cli_context_t *ctx) {
  assert(ctx != NULL);
  assert(ctx->linebuff != NULL);
  assert(ctx->in_stream != NULL);

  char *endptr = fgets(ctx->linebuff, LINE_LEN, ctx->in_stream);
  if (endptr == NULL) {
    // TODO: here we are not caring about errno, maybe it would make sense...
    return ERR_FAILED_TO_READ_FROM_STREAM;
  }

  return 0;
}
