/* Stat the file system and print what we find.  */
static bool ATTRIBUTE_WARN_UNUSED_RESULT
do_statfs (char const *filename, bool terse, char const *format)
{
  STRUCT_STATVFS statfsbuf;

  if (STREQ (filename, "-"))
    {
      error (0, 0, _("using %s to denote standard input does not work"
                     " in file system mode"), quote (filename));
      return false;
    }

  if (STATFS (filename, &statfsbuf) != 0)
    {
      error (0, errno, _("cannot read file system information for %s"),
             quote (filename));
      return false;
    }

  bool fail = print_it (format, filename, print_statfs, &statfsbuf);
  return ! fail;
}

