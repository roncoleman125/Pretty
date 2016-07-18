/* Print the username of each valid entry and the number of valid entries
   in UTMP_BUF, which should have N elements. */
static void
list_entries_who (size_t n, const STRUCT_UTMP *utmp_buf)
{
  unsigned long int entries = 0;
  char const *separator = "";

  while (n--)
    {
      if (IS_USER_PROCESS (utmp_buf))
        {
          char *trimmed_name;

          trimmed_name = extract_trimmed_name (utmp_buf);

          printf ("%s%s", separator, trimmed_name);
          free (trimmed_name);
          separator = " ";
          entries++;
        }
      utmp_buf++;
    }
  printf (_("\n# users=%lu\n"), entries);
}

