#include <program.h>

#include <atom.h>
#include <scope.h>
#include <internal_functions.h>

Program* program_new(void) {
  return calloc(1, sizeof(Program));
}

void program_destroy(Program* program) {
  if (program->global_scope) {
    scope_destroy(program->global_scope);
  }
  free(program);
}

void program_init(Program* program) {
  if (program->global_scope != NULL) {
    scope_destroy(program->global_scope);
  }

  program->global_scope = scope_new(NULL);
  init_internal_functions(program->global_scope);
}

void program_exec_file(Program* program, const char* filename) {
  FILE* fh = fopen(filename, "r");
  size_t size = 0;
  char* fullbuf = NULL;
  char* buf;

  if (!fh) return;
  
  buf = malloc(1024);
  while (!feof(fh)) {
    size_t read_size = fread(buf, 1, 1024, fh);
    fullbuf = realloc(fullbuf, size + read_size + 1);
    memcpy(&fullbuf[size], buf, read_size);
    size += read_size;
  }
  fclose(fh);
  free(buf);

  fullbuf[size] = '\0';

  program_exec_string(program, fullbuf);

  free(fullbuf);
}

void program_exec_string(Program* program, const char* string) {
  Atom* atom;

  atom = atom_parse_string(string);
  atom_eval(atom, program->global_scope);
  atom_destroy(atom);

}

void exec_one_line(const char* string) {
  Program* program = program_new();
  program_init(program);
  program_exec_string(program, string);
  program_destroy(program);
}

