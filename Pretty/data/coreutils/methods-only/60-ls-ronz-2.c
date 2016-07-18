/* Print the user or group name NAME, with numeric id ID, using a
   print width of WIDTH columns.  */

static void
format_user_or_group (char const *name, unsigned long int id, int width)
{
  size_t len;

  if (name)
    {
      int width_gap = width - mbswidth (name, 0);
      int pad = MAX (0, width_gap);
      fputs (name, stdout);
      len = strlen (name) + pad;

      do
        putchar (' ');
      while (pad--);
    }
  else
    {
      printf ("%*lu ", width, id);
      len = width;
    }

  dired_pos += len + 1;
}

