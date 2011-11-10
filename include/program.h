/* header guard generate by genheader.pl */
#ifndef _program_h_
#define _program_h_

struct statement_t;
struct sym_table_t;

struct program_t {
  int num_statements;
  struct statement_t* step_ptr;
  struct statement_t** statements;
  struct scope_t* global_scope;
  struct 
};

struct program_t* new_program(void);
void destroy_program(struct program_t* program);

void program_step(struct program_t* program);
void program_run(struct program_t* program);


void program_append_statement(
    struct program_t* program,
    const statement* statement);




#endif /* _program_h_ */
