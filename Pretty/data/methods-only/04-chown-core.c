/* Tell the user how/if the user and group of FILE have been changed.
   If USER is NULL, give the group-oriented messages.
   CHANGED describes what (if anything) has happened. */

static void
describe_change (const char *file, enum Change_status changed,
                 char const *user, char const *group)
{
  const char *fmt;
  char const *spec;
  char *spec_allocated = NULL;

  if (changed == CH_NOT_APPLIED)
    {
      printf (_("neither symbolic link %s nor referent has been changed\n"),
              quote (file));
      return;
    }

  if (user)
    {
      if (group)
        {
          spec_allocated = xmalloc (strlen (user) + 1 + strlen (group) + 1);
          stpcpy (stpcpy (stpcpy (spec_allocated, user), ":"), group);
          spec = spec_allocated;
        }
      else
        {
          spec = user;
        }
    }
  else
    {
      spec = group;
    }

  switch (changed)
    {
    case CH_SUCCEEDED:
      fmt = (user ? _("changed ownership of %s to %s\n")
             : group ? _("changed group of %s to %s\n")
             : _("no change to ownership of %s\n"));
      break;
    case CH_FAILED:
      fmt = (user ? _("failed to change ownership of %s to %s\n")
             : group ? _("failed to change group of %s to %s\n")
             : _("failed to change ownership of %s\n"));
      break;
    case CH_NO_CHANGE_REQUESTED:
      fmt = (user ? _("ownership of %s retained as %s\n")
             : group ? _("group of %s retained as %s\n")
             : _("ownership of %s retained\n"));
      break;
    default:
      abort ();
    }

  printf (fmt, quote (file), spec);

  free (spec_allocated);
}

