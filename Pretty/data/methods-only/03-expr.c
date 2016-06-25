static void
die (int errno_val, char const *msg)
{
  error (EXPR_FAILURE, errno_val, "%s", msg);
  abort (); /* notreached */
}

