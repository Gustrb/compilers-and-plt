#include <stdint.h>
#include <stdio.h>

#include "./arena.c"
#include "./cli.c"
#include "./lexer.c"

int main(int32_t argc, char **argv) {
  uint8_t is_running = 1;

  cli_context_t ctx;
  cli_context_init(&ctx, argc, argv);

  int32_t err;
  while (is_running) {
    err = cli_display_prompt(&ctx);
    if (err != 0) {
      // TODO: PRINT THE ERROR
      is_running = 0;
      continue;
    }

    err = cli_read_user_input(&ctx);
    if (err != 0) {
      // TODO: PRINT THE ERROR
      is_running = 0;
      continue;
    }

    uint64_t linelen = strlen(ctx.linebuff);

    lexer_t l;
    token_t t;
    lexer_init(&l, ctx.linebuff, linelen);

    while (1) {
      err = lexer_next_token(&l, &t);
      if (err != 0) {
        fprintf(stderr, "[ERROR]: Failed to lex expression\n");
        return err;
      }
      if (t.type == TOKEN_TYPE_EOF) {
        break;
      }

      printf("Token type: %d\n", t.type);
    }
  }

  return 0;
}
