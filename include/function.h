/* header guard generate by genheader.pl */
#ifndef _function_h_
#define _function_h_

#include <defines.h>
#include <setjmp.h>

#define FUNC_TYPE_INTERNAL     1
#define FUNC_TYPE_USERDEFINED  2


typedef Atom* (*FunctionCallback)(const List* args,
                                  Scope* scope,
                                  char** error_message,
                                  jmp_buf trace_point);


struct _Function {
  int type;
  List* parameter; /* List of atom of string */
  union {
    List* user_defined;
    FunctionCallback internal;
  };
};


/* parameter and body in list will be duplicated */
Function* function_new_internal(const List* parameter, FunctionCallback body);
Function* function_new_userdefined(const List* parameter, const List* body);

Function* function_duplicate(const Function* function);

void function_destroy(Function* function);

/* 0: parameter are available
   else return number of parameter which invalid;
 */
int function_check_parameter(const Function* function);
int function_check_arguments(const Function* function,
                             const List* arguments);


/*
  returns:
     0: successed and stored result into `result'.
    -1: argument count error.
    -2: parameter invalid.
    -3: functionc evaluate error.
 */
int function_call(const Function* function,
                  const List* arguments,
                  Scope* scope,
                  Atom** result);
int function_call_internal(const FunctionCallback callback,
                           const List* args,
                           Scope* scope,
                           Atom** result);
int function_call_userdefined(const Function* function,
                              const List* args,
                              Scope* scope,
                              Atom** result);



/* interface for internal defined function */
#define RaiseError(errmsg)                      \
  do {                                          \
    *error_message = errmsg;                    \
    longjmp(trace_point, 1);                    \
  } while (0)



#endif /* _function_h_ */
