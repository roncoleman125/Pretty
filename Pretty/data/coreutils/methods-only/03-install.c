static void
cp_option_init (struct cp_options *x)
{
  cp_options_default (x);
  x->copy_as_regular = true;
  x->reflink_mode = REFLINK_NEVER;
  x->dereference = DEREF_ALWAYS;
  x->unlink_dest_before_opening = true;
  x->unlink_dest_after_failed_open = false;
  x->hard_link = false;
  x->interactive = I_UNSPECIFIED;
  x->move_mode = false;
  x->one_file_system = false;
  x->preserve_ownership = false;
  x->preserve_links = false;
  x->preserve_mode = false;
  x->preserve_timestamps = false;
  x->reduce_diagnostics=false;
  x->data_copy_required = true;
  x->require_preserve = false;
  x->require_preserve_context = false;
  x->require_preserve_xattr = false;
  x->recursive = false;
  x->sparse_mode = SPARSE_AUTO;
  x->symbolic_link = false;
  x->backup_type = no_backups;

  /* Create destination files initially writable so we can run strip on them.
     Although GNU strip works fine on read-only files, some others
     would fail.  */
  x->set_mode = true;
  x->mode = S_IRUSR | S_IWUSR;
  x->stdin_tty = false;

  x->open_dangling_dest_symlink = false;
  x->update = false;
  x->preserve_security_context = false;
  x->preserve_xattr = false;
  x->verbose = false;
  x->dest_info = NULL;
  x->src_info = NULL;
}

/* Modify file context to match the specified policy.
   If an error occurs the file will remain with the default directory
   context.  */
static void
setdefaultfilecon (char const *file)
{
  struct stat st;
  security_context_t scontext = NULL;
  static bool first_call = true;

  if (selinux_enabled != 1)
    {
      /* Indicate no context found. */
      return;
    }
  if (lstat (file, &st) != 0)
    return;

  if (first_call && IS_ABSOLUTE_FILE_NAME (file))
    {
      /* Calling matchpathcon_init_prefix (NULL, "/first_component/")
         is an optimization to minimize the expense of the following
         matchpathcon call.  Do it only once, just before the first
         matchpathcon call.  We *could* call matchpathcon_fini after
         the final matchpathcon call, but that's not necessary, since
         by then we're about to exit, and besides, the buffers it
         would free are still reachable.  */
      char const *p0;
      char const *p = file + 1;
      while (ISSLASH (*p))
        ++p;

      /* Record final leading slash, for when FILE starts with two or more.  */
      p0 = p - 1;

      if (*p)
        {
          char *prefix;
          do
            {
              ++p;
            }
          while (*p && !ISSLASH (*p));

          prefix = malloc (p - p0 + 2);
          if (prefix)
            {
              stpcpy (stpncpy (prefix, p0, p - p0), "/");
              matchpathcon_init_prefix (NULL, prefix);
              free (prefix);
            }
        }
    }
  first_call = false;

  /* If there's an error determining the context, or it has none,
     return to allow default context */
  if ((matchpathcon (file, st.st_mode, &scontext) != 0) ||
      STREQ (scontext, "<<none>>"))
    {
      if (scontext != NULL)
        freecon (scontext);
      return;
    }

  if (lsetfilecon (file, scontext) < 0 && errno != ENOTSUP)
    error (0, errno,
           _("warning: %s: failed to change context to %s"),
           quotearg_colon (file), scontext);

  freecon (scontext);
  return;
}
static void
setdefaultfilecon (char const *file)
{
  (void) file;
}

