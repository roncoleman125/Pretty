/* FIXME: describe */
/* FIXME: rewrite this to use a hash table so we avoid the quadratic
   performance hit that's probably noticeable only on trees deeper
   than a few hundred levels.  See use of active_dir_map in remove.c  */

static bool
is_ancestor (const struct stat *sb, const struct dir_list *ancestors)
{
  while (ancestors != 0)
    {
      if (ancestors->ino == sb->st_ino && ancestors->dev == sb->st_dev)
        return true;
      ancestors = ancestors->parent;
    }
  return false;
}

static bool
errno_unsupported (int err)
{
  return err == ENOTSUP || err == ENODATA;
}

static void
copy_attr_error (struct error_context *ctx ATTRIBUTE_UNUSED,
                 char const *fmt, ...)
{
  if (!errno_unsupported (errno))
    {
      int err = errno;
      va_list ap;

      /* use verror module to print error message */
      va_start (ap, fmt);
      verror (0, err, fmt, ap);
      va_end (ap);
    }
}

static void
copy_attr_allerror (struct error_context *ctx ATTRIBUTE_UNUSED,
                 char const *fmt, ...)
{
  int err = errno;
  va_list ap;

  /* use verror module to print error message */
  va_start (ap, fmt);
  verror (0, err, fmt, ap);
  va_end (ap);
}

static char const *
copy_attr_quote (struct error_context *ctx ATTRIBUTE_UNUSED, char const *str)
{
  return quote (str);
}

static void
copy_attr_free (struct error_context *ctx ATTRIBUTE_UNUSED,
                char const *str ATTRIBUTE_UNUSED)
{
}

/* If positive SRC_FD and DST_FD descriptors are passed,
   then copy by fd, otherwise copy by name.  */

static bool
copy_attr (char const *src_path, int src_fd,
           char const *dst_path, int dst_fd, struct cp_options const *x)
{
  int ret;
  bool all_errors = (!x->data_copy_required || x->require_preserve_xattr);
  bool some_errors = (!all_errors && !x->reduce_diagnostics);
  struct error_context ctx =
  {
    .error = all_errors ? copy_attr_allerror : copy_attr_error,
    .quote = copy_attr_quote,
    .quote_free = copy_attr_free
  };
  if (0 <= src_fd && 0 <= dst_fd)
    ret = attr_copy_fd (src_path, src_fd, dst_path, dst_fd, 0,
                        (all_errors || some_errors ? &ctx : NULL));
  else
    ret = attr_copy_file (src_path, dst_path, 0,
                          (all_errors || some_errors ? &ctx : NULL));

  return ret == 0;
}

static bool
copy_attr (char const *src_path ATTRIBUTE_UNUSED,
           int src_fd ATTRIBUTE_UNUSED,
           char const *dst_path ATTRIBUTE_UNUSED,
           int dst_fd ATTRIBUTE_UNUSED,
           struct cp_options const *x ATTRIBUTE_UNUSED)
{
  return true;
}

