/* header guard generate by genheader.pl */
#ifndef _function_h_
#define _function_h_

#include <defines.h>
/*#define FUNCTION_NAME_ANONYMOUS ((char*)0L)*/

struct Function_ {
  char* args;
  int n_insts;
  Atom* insts;
  /* todo: content of a function */
};


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
  Function* function,
  );




#endif /* _function_h_ */
