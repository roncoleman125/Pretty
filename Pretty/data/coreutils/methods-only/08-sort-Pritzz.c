/* If PID is in the process table, remove it and return true.
   Otherwise, return false.  */

static bool
delete_proc (pid_t pid)
{
  struct tempnode test;

  test.pid = pid;
  struct tempnode *node = hash_delete (proctab, &test);
  if (! node)
    return false;
  node->state = REAPED;
  return true;
}

