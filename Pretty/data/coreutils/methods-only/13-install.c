/* Strip the symbol table from the file NAME.
   We could dig the magic number out of the file first to
   determine whether to strip it, but the header files and
   magic numbers vary so much from system to system that making
   it portable would be very difficult.  Not worth the effort. */

static void
strip (char const *name)
{
  int status;
  pid_t pid = fork ();

  switch (pid)
    {
    case -1:
      error (EXIT_FAILURE, errno, _("fork system call failed"));
      break;
    case 0:			/* Child. */
      execlp (strip_program, strip_program, name, NULL);
      error (EXIT_FAILURE, errno, _("cannot run %s"), strip_program);
      break;
    default:			/* Parent. */
      if (waitpid (pid, &status, 0) < 0)
        error (EXIT_FAILURE, errno, _("waiting for strip"));
      else if (! WIFEXITED (status) || WEXITSTATUS (status))
        error (EXIT_FAILURE, 0, _("strip process terminated abnormally"));
      break;
    }
}

