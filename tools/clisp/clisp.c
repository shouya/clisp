#include <stdlib.h>
#include <stdio.h>

#include <clisp_common.h>
/*#include <sy_file.h>*/

int main(int argc, char** argv) {
  Program* prog;
  char** i;
  if (argc <= 1) {
    /* argument too less */
    exit(-1);
  }

  prog = program_new();
  program_init(prog);

  for (i = &argv[1]; i != argv; ++i) {
    FILE* f = fopen(*i, "r");
    if (!f) {
      /* file cannot be open */
      program_destroy(prog);
      exit(-2);
    }
    fclose(f);

    program_exec_file(prog, *i);
  }

  program_destroy(prog);

  exit(0);
}
