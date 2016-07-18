/* Decode the modern od format string S.  Append the decoded
   representation to the global array SPEC, reallocating SPEC if
   necessary.  Return true if S is valid.  */

static bool
decode_format_string (const char *s)
{
  const char *s_orig = s;
  assert (s != NULL);

  while (*s != '\0')
    {
      const char *next;

      if (n_specs_allocated <= n_specs)
        spec = X2NREALLOC (spec, &n_specs_allocated);

      if (! decode_one_format (s_orig, s, &next, &spec[n_specs]))
        return false;

      assert (s != next);
      s = next;
      ++n_specs;
    }

  return true;
}

