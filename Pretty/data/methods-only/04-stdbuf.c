static void
set_LD_PRELOAD (void)
{
  int ret;
  char *old_libs = getenv ("LD_PRELOAD");
  char *LD_PRELOAD;

  /* Note this would auto add the appropriate search path for "libstdbuf.so":
     gcc stdbuf.c -Wl,-rpath,'$ORIGIN' -Wl,-rpath,$PKGLIBDIR
     However we want the lookup done for the exec'd command not stdbuf.

     Since we don't link against libstdbuf.so add it to LIBDIR rather than
     LIBEXECDIR, as we'll search for it in the "sys default" case below.  */
  char const *const search_path[] = {
    program_path,
    PKGLIBDIR,
    "",                         /* sys default */
    NULL
  };

  char const *const *path = search_path;
  char *libstdbuf;

  do
    {
      struct stat sb;

      if (!**path)              /* system default  */
        {
          libstdbuf = xstrdup (LIB_NAME);
          break;
        }
      ret = asprintf (&libstdbuf, "%s/%s", *path, LIB_NAME);
      if (ret < 0)
        xalloc_die ();
      if (stat (libstdbuf, &sb) == 0)   /* file_exists  */
        break;
      free (libstdbuf);
    }
  while (*++path);

  /* FIXME: Do we need to support libstdbuf.dll, c:, '\' separators etc?  */

  if (old_libs)
    ret = asprintf (&LD_PRELOAD, "LD_PRELOAD=%s:%s", old_libs, libstdbuf);
  else
    ret = asprintf (&LD_PRELOAD, "LD_PRELOAD=%s", libstdbuf);

  if (ret < 0)
    xalloc_die ();

  free (libstdbuf);

  ret = putenv (LD_PRELOAD);

  if (ret != 0)
    {
      error (EXIT_CANCELED, errno,
             _("failed to update the environment with %s"),
             quote (LD_PRELOAD));
    }
}

