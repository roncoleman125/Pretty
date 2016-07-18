/* Fork a child process for piping to and do common cleanup.  The
   TRIES parameter tells us how many times to try to fork before
   giving up.  Return the PID of the child, or -1 (setting errno)
   on failure. */

static pid_t
pipe_fork (int pipefds[2], size_t tries)
{
  struct tempnode *saved_temphead;
  int saved_errno;
  double wait_retry = 0.25;
  pid_t pid IF_LINT ( = -1);
  struct cs_status cs;

  if (pipe (pipefds) < 0)
    return -1;

  /* At least NMERGE + 1 subprocesses are needed.  More could be created, but
     uncontrolled subprocess generation can hurt performance significantly.
     Allow at most NMERGE + 2 subprocesses, on the theory that there
     may be some useful parallelism by letting compression for the
     previous merge finish (1 subprocess) in parallel with the current
     merge (NMERGE + 1 subprocesses).  */

  if (nmerge + 1 < nprocs)
    reap_some ();

  while (tries--)
    {
      /* This is so the child process won't delete our temp files
         if it receives a signal before exec-ing.  */
      cs = cs_enter ();
      saved_temphead = temphead;
      temphead = NULL;

      pid = fork ();
      saved_errno = errno;
      if (pid)
        temphead = saved_temphead;

      cs_leave (cs);
      errno = saved_errno;

      if (0 <= pid || errno != EAGAIN)
        break;
      else
        {
          xnanosleep (wait_retry);
          wait_retry *= 2;
          reap_exited ();
        }
    }

  if (pid < 0)
    {
      saved_errno = errno;
      close (pipefds[0]);
      close (pipefds[1]);
      errno = saved_errno;
    }
  else if (pid == 0)
    {
      close (STDIN_FILENO);
      close (STDOUT_FILENO);
    }
  else
    ++nprocs;

  return pid;

  return -1;
}

