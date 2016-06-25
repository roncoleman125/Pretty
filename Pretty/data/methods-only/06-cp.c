/* Given a string, ARG, containing a comma-separated list of arguments
   to the --preserve option, set the appropriate fields of X to ON_OFF.  */
static void
decode_preserve_arg (char const *arg, struct cp_options *x, bool on_off)
{
  enum File_attribute
    {
      PRESERVE_MODE,
      PRESERVE_TIMESTAMPS,
      PRESERVE_OWNERSHIP,
      PRESERVE_LINK,
      PRESERVE_CONTEXT,
      PRESERVE_XATTR,
      PRESERVE_ALL
    };
  static enum File_attribute const preserve_vals[] =
    {
      PRESERVE_MODE, PRESERVE_TIMESTAMPS,
      PRESERVE_OWNERSHIP, PRESERVE_LINK, PRESERVE_CONTEXT, PRESERVE_XATTR,
      PRESERVE_ALL
    };
  /* Valid arguments to the `--preserve' option. */
  static char const* const preserve_args[] =
    {
      "mode", "timestamps",
      "ownership", "links", "context", "xattr", "all", NULL
    };
  ARGMATCH_VERIFY (preserve_args, preserve_vals);

  char *arg_writable = xstrdup (arg);
  char *s = arg_writable;
  do
    {
      /* find next comma */
      char *comma = strchr (s, ',');
      enum File_attribute val;

      /* If we found a comma, put a NUL in its place and advance.  */
      if (comma)
        *comma++ = 0;

      /* process S.  */
      val = XARGMATCH ("--preserve", s, preserve_args, preserve_vals);
      switch (val)
        {
        case PRESERVE_MODE:
          x->preserve_mode = on_off;
          break;

        case PRESERVE_TIMESTAMPS:
          x->preserve_timestamps = on_off;
          break;

        case PRESERVE_OWNERSHIP:
          x->preserve_ownership = on_off;
          break;

        case PRESERVE_LINK:
          x->preserve_links = on_off;
          break;

        case PRESERVE_CONTEXT:
          x->preserve_security_context = on_off;
          x->require_preserve_context = on_off;
          break;

        case PRESERVE_XATTR:
          x->preserve_xattr = on_off;
          x->require_preserve_xattr = on_off;
          break;

        case PRESERVE_ALL:
          x->preserve_mode = on_off;
          x->preserve_timestamps = on_off;
          x->preserve_ownership = on_off;
          x->preserve_links = on_off;
          if (selinux_enabled)
            x->preserve_security_context = on_off;
          x->preserve_xattr = on_off;
          break;

        default:
          abort ();
        }
      s = comma;
    }
  while (s);

  free (arg_writable);
}

