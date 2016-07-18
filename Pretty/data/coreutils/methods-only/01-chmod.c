/* Tell the user how/if the MODE of FILE has been changed.
   CHANGED describes what (if anything) has happened. */

static void
describe_change (const char *file, mode_t mode,
                 enum Change_status changed)
{
  char perms[12];		/* "-rwxrwxrwx" ls-style modes. */
  const char *fmt;

  if (changed == CH_NOT_APPLIED)
    {
      printf (_("neither symbolic link %s nor referent has been changed\n"),
              quote (file));
      return;
    }

  strmode (mode, perms);
  perms[10] = '\0';		/* Remove trailing space.  */
  switch (changed)
    {
    case CH_SUCCEEDED:
      fmt = _("mode of %s changed to %04lo (%s)\n");
      break;
    case CH_FAILED:
      fmt = _("failed to change mode of %s to %04lo (%s)\n");
      break;
    case CH_NO_CHANGE_REQUESTED:
      fmt = _("mode of %s retained as %04lo (%s)\n");
      break;
    default:
      abort ();
    }
  printf (fmt, quote (file),
          (unsigned long int) (mode & CHMOD_MODE_BITS), &perms[1]);
}

