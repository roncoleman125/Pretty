/* Return true if S is a non-empty list in which exactly one
   character (but potentially, many instances of it) appears.
   E.g., [X*] or xxxxxxxx.  */

static bool
homogeneous_spec_list (struct Spec_list *s)
{
  int b, c;

  s->state = BEGIN_STATE;

  if ((b = get_next (s, NULL)) == -1)
    return false;

  while ((c = get_next (s, NULL)) != -1)
    if (c != b)
      return false;

  return true;
}

