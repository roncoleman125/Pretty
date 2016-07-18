/* Report an integer overflow for operation OP and exit.  */
static void
integer_overflow (char op)
{
  error (EXPR_FAILURE, ERANGE, "%c", op);
  abort (); /* notreached */
}

static void die (int errno_val, char const *msg)
  ATTRIBUTE_NORETURN;
