static void
parse_line (char const *line, char **keyword, char **arg)
{
  char const *p;
  char const *keyword_start;
  char const *arg_start;

  *keyword = NULL;
  *arg = NULL;

  for (p = line; isspace (to_uchar (*p)); ++p)
    continue;

  /* Ignore blank lines and shell-style comments.  */
  if (*p == '\0' || *p == '#')
    return;

  keyword_start = p;

  while (!isspace (to_uchar (*p)) && *p != '\0')
    {
      ++p;
    }

  *keyword = xstrndup (keyword_start, p - keyword_start);
  if (*p  == '\0')
    return;

  do
    {
      ++p;
    }
  while (isspace (to_uchar (*p)));

  if (*p == '\0' || *p == '#')
    return;

  arg_start = p;

  while (*p != '\0' && *p != '#')
    ++p;

  for (--p; isspace (to_uchar (*p)); --p)
    continue;
  ++p;

  *arg = xstrndup (arg_start, p - arg_start);
}

