/* Trim space from the front and back of the string P, yielding the prefix,
   and record the lengths of the prefix and the space trimmed.  */

static void
set_prefix (char *p)
{
  char *s;

  prefix_lead_space = 0;
  while (*p == ' ')
    {
      prefix_lead_space++;
      p++;
    }
  prefix = p;
  prefix_full_length = strlen (p);
  s = p + prefix_full_length;
  while (s > p && s[-1] == ' ')
    s--;
  *s = '\0';
  prefix_length = s - p;
}

