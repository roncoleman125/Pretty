/* Return true if it's OK for chown to fail, where errno is
   the error number that chown failed with and X is the copying
   option set.  */

extern bool
chown_failure_ok (struct cp_options const *x)
{
  /* If non-root uses -p, it's ok if we can't preserve ownership.
     But root probably wants to know, e.g. if NFS disallows it,
     or if the target system doesn't support file ownership.  */

  return ((errno == EPERM || errno == EINVAL) && !x->chown_privileges);
}

