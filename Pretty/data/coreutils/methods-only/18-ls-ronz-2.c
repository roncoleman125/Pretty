/* Set the exit status to report a failure.  If SERIOUS, it is a
   serious failure; otherwise, it is merely a minor problem.  */

static void
set_exit_status (bool serious)
{
  if (serious)
    exit_status = LS_FAILURE;
  else if (exit_status == EXIT_SUCCESS)
    exit_status = LS_MINOR_PROBLEM;
}

