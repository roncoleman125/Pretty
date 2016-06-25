/* Return false if two strings OLD and NEW match, true if not.
   OLD and NEW point not to the beginnings of the lines
   but rather to the beginnings of the fields to compare.
   OLDLEN and NEWLEN are their lengths. */

static bool
different (char *old, char *new, size_t oldlen, size_t newlen)
{
  if (check_chars < oldlen)
    oldlen = check_chars;
  if (check_chars < newlen)
    newlen = check_chars;

  if (ignore_case)
    {
      /* FIXME: This should invoke strcoll somehow.  */
      return oldlen != newlen || memcasecmp (old, new, oldlen);
    }
  else if (hard_LC_COLLATE)
    return xmemcoll (old, oldlen, new, newlen) != 0;
  else
    return oldlen != newlen || memcmp (old, new, oldlen);
}

