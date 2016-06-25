/* Copy from file descriptor INPUT_FD (corresponding to the named FILE) to
   a temporary file, and set *G_TMP and *G_TEMPFILE to the resulting stream
   and file name.  Return true if successful.  */

static bool
copy_to_temp (FILE **g_tmp, char **g_tempfile, int input_fd, char const *file)
{
  static char *template = NULL;
  static char const *tempdir;
  char *tempfile;
  FILE *tmp;
  int fd;

  if (template == NULL)
    {
      char const * const Template = "%s/tacXXXXXX";
      tempdir = getenv ("TMPDIR");
      if (tempdir == NULL)
        tempdir = DEFAULT_TMPDIR;

      /* Subtract 2 for `%s' and add 1 for the trailing NUL byte.  */
      template = xmalloc (strlen (tempdir) + strlen (Template) - 2 + 1);
      sprintf (template, Template, tempdir);
    }

  /* FIXME: there's a small window between a successful mkstemp call
     and the unlink that's performed by record_or_unlink_tempfile.
     If we're interrupted in that interval, this code fails to remove
     the temporary file.  On systems that define DONT_UNLINK_WHILE_OPEN,
     the window is much larger -- it extends to the atexit-called
     unlink_tempfile.
     FIXME: clean up upon fatal signal.  Don't block them, in case
     $TMPFILE is a remote file system.  */

  tempfile = template;
  fd = mkstemp (template);
  if (fd < 0)
    {
      error (0, errno, _("cannot create temporary file in %s"),
             quote (tempdir));
      return false;
    }

  tmp = fdopen (fd, (O_BINARY ? "w+b" : "w+"));
  if (! tmp)
    {
      error (0, errno, _("cannot open %s for writing"), quote (tempfile));
      close (fd);
      unlink (tempfile);
      return false;
    }

  record_or_unlink_tempfile (tempfile, tmp);

  while (1)
    {
      size_t bytes_read = safe_read (input_fd, G_buffer, read_size);
      if (bytes_read == 0)
        break;
      if (bytes_read == SAFE_READ_ERROR)
        {
          error (0, errno, _("%s: read error"), quotearg_colon (file));
          goto Fail;
        }

      if (fwrite (G_buffer, 1, bytes_read, tmp) != bytes_read)
        {
          error (0, errno, _("%s: write error"), quotearg_colon (tempfile));
          goto Fail;
        }
    }

  if (fflush (tmp) != 0)
    {
      error (0, errno, _("%s: write error"), quotearg_colon (tempfile));
      goto Fail;
    }

  *g_tmp = tmp;
  *g_tempfile = tempfile;
  return true;

 Fail:
  fclose (tmp);
  return false;
}

