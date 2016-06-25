/* If FILE contains a component with a leading "-", report an error
   and return false; otherwise, return true.  */

static bool
no_leading_hyphen (char const *file)
{
  char const *p;

  for (p = file;  (p = strchr (p, '-'));  p++)
    if (p == file || p[-1] == '/')
      {
        error (0, 0, _("leading `-' in a component of file name %s"),
               quote (file));
        return false;
      }

  return true;
}

