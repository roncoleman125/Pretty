static void
list_entries_users (size_t n, const STRUCT_UTMP *this)
{
  char **u = xnmalloc (n, sizeof *u);
  size_t i;
  size_t n_entries = 0;

  while (n--)
    {
      if (IS_USER_PROCESS (this))
        {
          char *trimmed_name;

          trimmed_name = extract_trimmed_name (this);

          u[n_entries] = trimmed_name;
          ++n_entries;
        }
      this++;
    }

  qsort (u, n_entries, sizeof (u[0]), userid_compare);

  for (i = 0; i < n_entries; i++)
    {
      char c = (i < n_entries - 1 ? ' ' : '\n');
      fputs (u[i], stdout);
      putchar (c);
    }

  for (i = 0; i < n_entries; i++)
    free (u[i]);
  free (u);
}

