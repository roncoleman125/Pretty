/* Copy file FROM onto file TO, creating TO if necessary.
   Return true if successful.  */

static bool
copy_file (const char *from, const char *to, const struct cp_options *x)
{
  bool copy_into_self;

  if (copy_only_if_needed && !need_copy (from, to, x))
    return true;

  /* Allow installing from non-regular files like /dev/null.
     Charles Karney reported that some Sun version of install allows that
     and that sendmail's installation process relies on the behavior.
     However, since !x->recursive, the call to "copy" will fail if FROM
     is a directory.  */

  return copy (from, to, false, x, &copy_into_self, NULL);
}

