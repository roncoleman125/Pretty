/* Return true if the string at ES->s[IDX] matches the regular
   expression `\*[0-9]*\]', false otherwise.  The string does not
   match if any of its characters are escaped.  */

static bool
star_digits_closebracket (const struct E_string *es, size_t idx)
{
  size_t i;

  if (!es_match (es, idx, '*'))
    return false;

  for (i = idx + 1; i < es->len; i++)
    if (!ISDIGIT (to_uchar (es->s[i])) || es->escaped[i])
      return es_match (es, i, ']');
  return false;
}

