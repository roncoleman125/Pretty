/* Clean up any remaining temporary files.  */

static void
cleanup (void)
{
  struct tempnode const *node;

  for (node = temphead; node; node = node->next)
    unlink (node->name);
  temphead = NULL;
}

