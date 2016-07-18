static void
dup2_or_die (int oldfd, int newfd)
{
  if (dup2 (oldfd, newfd) < 0)
    error (SORT_FAILURE, errno, _("dup2 failed"));
}

