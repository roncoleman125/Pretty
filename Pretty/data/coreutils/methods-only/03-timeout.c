/* Given a long integer value *X, and a suffix character, SUFFIX_CHAR,
   scale *X by the multiplier implied by SUFFIX_CHAR.  SUFFIX_CHAR may
   be the NUL byte or `s' to denote seconds, `m' for minutes, `h' for
   hours, or `d' for days.  If SUFFIX_CHAR is invalid, don't modify *X
   and return false.  If *X would overflow an integer, don't modify *X
   and return false. Otherwise return true.  */

static bool
apply_time_suffix (unsigned long *x, char suffix_char)
{
  unsigned int multiplier = 1;

  switch (suffix_char)
    {
    case 0:
    case 's':
      return true;
    case 'd':
      multiplier *= 24;
    case 'h':
      multiplier *= 60;
    case 'm':
      if (multiplier > UINT_MAX / 60) /* 16 bit overflow */
        return false;
      multiplier *= 60;
      break;
    default:
      return false;
    }

  if (*x > UINT_MAX / multiplier)
    return false;

  *x *= multiplier;

  return true;
}

