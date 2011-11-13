/* header guard generate by genheader.pl */
#ifndef _program_h_
#define _program_h_

#include <defines.h>

struct _Program {
  Scope* global_scope;
};

Program* program_new(void);
void program_destroy(Program* program);

void program_init(Program* program);

void program_exec_file(Program* program, const char* filename);
void program_exec_string(Program* program, const char* string);

void exec_one_line(const char* string);

#endif /* _program_h_ */
