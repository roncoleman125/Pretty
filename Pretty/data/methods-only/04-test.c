/* If the characters pointed to by STRING constitute a valid number,
   return a pointer to the start of the number, skipping any blanks or
   leading '+'.  Otherwise, report an error and exit.  */
static char const *
find_int (char const *string)
{
  char const *p;
  char const *number_start;

  for (p = string; isblank (to_uchar (*p)); p++)
    continue;

  if (*p == '+')
    {
      p++;
      number_start = p;
    }
  else
    {
      number_start = p;
      p += (*p == '-');
    }

  if (ISDIGIT (*p++))
    {
      while (ISDIGIT (*p))
        p++;
      while (isblank (to_uchar (*p)))
        p++;
      if (!*p)
        return number_start;
    }

  test_syntax_error (_("invalid integer %s"), quote (string));
}

