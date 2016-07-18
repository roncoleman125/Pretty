/* Assuming a failure is serious if SERIOUS, use the printf-style
   MESSAGE to report the failure to access a file named FILE.  Assume
   errno is set appropriately for the failure.  */

static void
file_failure (bool serious, char const *message, char const *file)
{
  error (0, errno, message, quotearg_colon (file));
  set_exit_status (serious);
}

