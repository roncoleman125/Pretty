/* Remove PID from the process table, and wait for it to exit if it
   hasn't already.  */

static void
wait_proc (pid_t pid)
{
  if (delete_proc (pid))
    reap (pid);
}

/* Reap any exited children.  Do not block; reap only those that have
   already exited.  */

