/* Similarly, return true if it's OK for chmod and similar operations
   to fail, where errno is the error number that chmod failed with and
   X is the copying option set.  */

static bool
owner_failure_ok (struct cp_options const *x)
{
  return ((errno == EPERM || errno == EINVAL) && !x->owner_privileges);
}

