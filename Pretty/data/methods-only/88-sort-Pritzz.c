/* Set the ordering options for KEY specified in S.
   Return the address of the first character in S that
   is not a valid ordering option.
   BLANKTYPE is the kind of blanks that 'b' should skip. */

static char *
set_ordering (char const *s, struct keyfield *key, enum blanktype blanktype)
{
  while (*s)
    {
      switch (*s)
        {
        case 'b':
          if (blanktype == bl_start || blanktype == bl_both)
            key->skipsblanks = true;
          if (blanktype == bl_end || blanktype == bl_both)
            key->skipeblanks = true;
          break;
        case 'd':
          key->ignore = nondictionary;
          break;
        case 'f':
          key->translate = fold_toupper;
          break;
        case 'g':
          key->general_numeric = true;
          break;
        case 'h':
          key->human_numeric = true;
          break;
        case 'i':
          /* Option order should not matter, so don't let -i override
             -d.  -d implies -i, but -i does not imply -d.  */
          if (! key->ignore)
            key->ignore = nonprinting;
          break;
        case 'M':
          key->month = true;
          break;
        case 'n':
          key->numeric = true;
          break;
        case 'R':
          key->random = true;
          break;
        case 'r':
          key->reverse = true;
          break;
        case 'V':
          key->version = true;
          break;
        default:
          return (char *) s;
        }
      ++s;
    }
  return (char *) s;
}

