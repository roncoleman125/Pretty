/* Change the context of FILE, using specified components.
   If it is a directory and -R is given, recurse.
   Return 0 if successful, 1 if errors occurred. */

static int
change_file_context (int fd, char const *file)
{
  security_context_t file_context = NULL;
  context_t context;
  security_context_t context_string;
  int errors = 0;

  if (specified_context == NULL)
    {
      int status = (affect_symlink_referent
                    ? getfileconat (fd, file, &file_context)
                    : lgetfileconat (fd, file, &file_context));

      if (status < 0 && errno != ENODATA)
        {
          error (0, errno, _("failed to get security context of %s"),
                 quote (file));
          return 1;
        }

      /* If the file doesn't have a context, and we're not setting all of
         the context components, there isn't really an obvious default.
         Thus, we just give up. */
      if (file_context == NULL)
        {
          error (0, 0, _("can't apply partial context to unlabeled file %s"),
                 quote (file));
          return 1;
        }

      if (compute_context_from_mask (file_context, &context))
        return 1;
    }
  else
    {
      /* FIXME: this should be done exactly once, in main.  */
      context = context_new (specified_context);
      if (!context)
        abort ();
    }

  context_string = context_str (context);

  if (file_context == NULL || ! STREQ (context_string, file_context))
    {
      int fail = (affect_symlink_referent
                  ?  setfileconat (fd, file, context_string)
                  : lsetfileconat (fd, file, context_string));

      if (fail)
        {
          errors = 1;
          error (0, errno, _("failed to change context of %s to %s"),
                 quote_n (0, file), quote_n (1, context_string));
        }
    }

  context_free (context);
  freecon (file_context);

  return errors;
}

