/* Return true if the last component of NAME is `.' or `..'
   This is so we don't try to recurse on `././././. ...' */

static bool
basename_is_dot_or_dotdot (const char *name)
{
  char const *base = last_component (name);
  return dot_or_dotdot (base);
}

