/* This function makes two passes over the argument string S.  The first
   one converts all \c and \ddd escapes to their one-byte representations.
   The second constructs a linked specification list, SPEC_LIST, of the
   characters and constructs that comprise the argument string.  If either
   of these passes detects an error, this function returns false.  */

static bool
parse_str (char const *s, struct Spec_list *spec_list)
{
  struct E_string es;
  bool ok = unquote (s, &es) && build_spec_list (&es, spec_list);
  es_free (&es);
  return ok;
}

