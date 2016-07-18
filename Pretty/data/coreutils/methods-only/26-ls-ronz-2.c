/* Return true if NAME has a capability (see linux/capability.h) */
static bool
has_capability (char const *name)
{
  char *result;
  bool has_cap;

  cap_t cap_d = cap_get_file (name);
  if (cap_d == NULL)
    return false;

  result = cap_to_text (cap_d, NULL);
  cap_free (cap_d);
  if (!result)
    return false;

  /* check if human-readable capability string is empty */
  has_cap = !!*result;

  cap_free (result);
  return has_cap;
}
