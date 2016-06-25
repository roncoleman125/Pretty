/* stat the file and print what we find */
static bool ATTRIBUTE_WARN_UNUSED_RESULT
do_stat (char const *filename, bool terse, char const *format,
         char const *format2)
{
  struct stat statbuf;

  if (STREQ (filename, "-"))
    {
      if (fstat (STDIN_FILENO, &statbuf) != 0)
        {
          error (0, errno, _("cannot stat standard input"));
          return false;
        }
    }
  /* We can't use the shorter
     (follow_links?stat:lstat) (filename, &statbug)
     since stat might be a function-like macro.  */
  else if ((follow_links
            ? stat (filename, &statbuf)
            : lstat (filename, &statbuf)) != 0)
    {
      error (0, errno, _("cannot stat %s"), quote (filename));
      return false;
    }

  if (S_ISBLK (statbuf.st_mode) || S_ISCHR (statbuf.st_mode))
    format = format2;

  bool fail = print_it (format, filename, print_stat, &statbuf);
  return ! fail;
}

