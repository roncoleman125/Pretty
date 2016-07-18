/* Increment NAME (with LEN bytes).  NAME must be a big-endian base N
   number with the digits taken from nameset.  Return true if
   successful if not (because NAME already has the greatest possible
   value.  */

static bool
incname (char *name, size_t len)
{
  while (len--)
    {
      char const *p = strchr (nameset, name[len]);

      /* If this character has a successor, use it.  */
      if (p[1])
        {
          name[len] = p[1];
          return true;
        }

      /* Otherwise, set this digit to 0 and increment the prefix.  */
      name[len] = nameset[0];
    }

  return false;
}

