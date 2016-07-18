static void
zaptemp (char const *name)
{
  struct tempnode *volatile *pnode;
  struct tempnode *node;
  struct tempnode *next;
  int unlink_status;
  int unlink_errno = 0;
  struct cs_status cs;

  for (pnode = &temphead; (node = *pnode)->name != name; pnode = &node->next)
    continue;

  if (node->state == UNREAPED)
    wait_proc (node->pid);

  /* Unlink the temporary file in a critical section to avoid races.  */
  next = node->next;
  cs = cs_enter ();
  unlink_status = unlink (name);
  unlink_errno = errno;
  *pnode = next;
  cs_leave (cs);

  if (unlink_status != 0)
    error (0, unlink_errno, _("warning: cannot remove: %s"), name);
  if (! next)
    temptail = pnode;
  free (node);
}

