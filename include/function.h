/* header guard generate by genheader.pl */
#ifndef _function_h_
#define _function_h_

#include <defines.h>
#include <setjmp.h>

#define FUNC_TYPE_INTERNAL     1
#define FUNC_TYPE_USERDEFINED  2

struct Function_ {
  int n_args;
  char** args;
  int type;
  union {
    
  };
};

typedef Atom* (*FunctionCallback)(List* args,
                                  Scope* scope,
                                  char** error_message,
                                  jmp_buf trace_point);


#define RaiseError(errmsg) do {                 \
    *error_message = errmsg;                    \
    long_jmp(trace_point);                      \
  } while (0)

/*
  arg_type type character instruction
   * l: an int32 val
   * u: an uint32 val
   * s: a string
   * b: a boolean value
   * n: an integral number
   * L: a list
   * X: a expanded list
   * a: a non-list atom
 */

Function function_new(void);
void function_destroy(Function* function);

/*
  argument reset by new allocate function, should be destroy manually.
  returns zero if successed, otherwise -1.
 */
int function_parse(
  Function* function,
  const char* arguments, /* as `(x y)' form */
  const char* body,      /* as `(+ x y)' form*/
  );

/*
  returns zero if function avalible else -1 will be returned.
 */
int function_check(
  Function* function
  );




#endif /* _function_h_ */
