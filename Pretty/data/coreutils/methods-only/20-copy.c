/* Copy the file SRC_NAME to the file DST_NAME.  The files may be of
   any type.  NONEXISTENT_DST should be true if the file DST_NAME
   is known not to exist (e.g., because its parent directory was just
   created);  NONEXISTENT_DST should be false if DST_NAME might already
   exist.  OPTIONS is ... FIXME-describe
   Set *COPY_INTO_SELF if SRC_NAME is a parent of (or the
   same as) DST_NAME; otherwise, set clear it.
   Return true if successful.  */

extern bool
copy (char const *src_name, char const *dst_name,
      bool nonexistent_dst, const struct cp_options *options,
      bool *copy_into_self, bool *rename_succeeded)
{
  assert (valid_options (options));

  /* Record the file names: they're used in case of error, when copying
     a directory into itself.  I don't like to make these tools do *any*
     extra work in the common case when that work is solely to handle
     exceptional cases, but in this case, I don't see a way to derive the
     top level source and destination directory names where they're used.
     An alternative is to use COPY_INTO_SELF and print the diagnostic
     from every caller -- but I don't want to do that.  */
  top_level_src_name = src_name;
  top_level_dst_name = dst_name;

  bool first_dir_created_per_command_line_arg = false;
  return copy_internal (src_name, dst_name, nonexistent_dst, 0, NULL,
                        options, true,
                        &first_dir_created_per_command_line_arg,
                        copy_into_self, rename_succeeded);
}

