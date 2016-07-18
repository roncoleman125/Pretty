/* Return true and advance if the next token matches STR exactly.
   STR must not be NULL.  */

static bool
nextarg (char const *str)
{
  if (*args == NULL)
    return false;
  else
    {
      bool r = STREQ (*args, str);
      args += r;
      return r;
    }
}

