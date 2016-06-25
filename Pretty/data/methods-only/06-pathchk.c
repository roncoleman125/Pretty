/* Make sure that
   strlen (FILE) <= PATH_MAX
   && strlen (each-existing-directory-in-FILE) <= NAME_MAX

   If CHECK_BASIC_PORTABILITY is true, compare against _POSIX_PATH_MAX and
   _POSIX_NAME_MAX instead, and make sure that FILE contains no
   characters not in the POSIX portable filename character set, which
   consists of A-Z, a-z, 0-9, ., _, - (plus / for separators).

   If CHECK_BASIC_PORTABILITY is false, make sure that all leading directories
   along FILE that exist are searchable.

   If CHECK_EXTRA_PORTABILITY is true, check that file name components do not
   begin with "-".

   If either CHECK_BASIC_PORTABILITY or CHECK_EXTRA_PORTABILITY is true,
   check that the file name is not empty.

   Return true if all of these tests are successful, false if any fail.  */

static bool
validate_file_name (char *file, bool check_basic_portability,
                    bool check_extra_portability)
{
  size_t filelen = strlen (file);

  /* Start of file name component being checked.  */
  char *start;

  /* True if component lengths need to be checked.  */
  bool check_component_lengths;

  /* True if the file is known to exist.  */
  bool file_exists = false;

  if (check_extra_portability && ! no_leading_hyphen (file))
    return false;

  if ((check_basic_portability || check_extra_portability)
      && filelen == 0)
    {
      /* Fail, since empty names are not portable.  As of
         2005-01-06 POSIX does not address whether "pathchk -p ''"
         should (or is allowed to) fail, so this is not a
         conformance violation.  */
      error (0, 0, _("empty file name"));
      return false;
    }

  if (check_basic_portability)
    {
      if (! portable_chars_only (file, filelen))
        return false;
    }
  else
    {
      /* Check whether a file name component is in a directory that
         is not searchable, or has some other serious problem.
         POSIX does not allow "" as a file name, but some non-POSIX
         hosts do (as an alias for "."), so allow "" if lstat does.  */

      struct stat st;
      if (lstat (file, &st) == 0)
        file_exists = true;
      else if (errno != ENOENT || filelen == 0)
        {
          error (0, errno, "%s", file);
          return false;
        }
    }

  if (check_basic_portability
      || (! file_exists && PATH_MAX_MINIMUM <= filelen))
    {
      size_t maxsize;

      if (check_basic_portability)
        maxsize = _POSIX_PATH_MAX;
      else
        {
          long int size;
          char const *dir = (*file == '/' ? "/" : ".");
          errno = 0;
          size = pathconf (dir, _PC_PATH_MAX);
          if (size < 0 && errno != 0)
            {
              error (0, errno,
                     _("%s: unable to determine maximum file name length"),
                     dir);
              return false;
            }
          maxsize = MIN (size, SSIZE_MAX);
        }

      if (maxsize <= filelen)
        {
          unsigned long int len = filelen;
          unsigned long int maxlen = maxsize - 1;
          error (0, 0, _("limit %lu exceeded by length %lu of file name %s"),
                 maxlen, len, quote (file));
          return false;
        }
    }

  /* Check whether pathconf (..., _PC_NAME_MAX) can be avoided, i.e.,
     whether all file name components are so short that they are valid
     in any file system on this platform.  If CHECK_BASIC_PORTABILITY, though,
     it's more convenient to check component lengths below.  */

  check_component_lengths = check_basic_portability;
  if (! check_component_lengths && ! file_exists)
    {
      for (start = file; *(start = component_start (start)); )
        {
          size_t length = component_len (start);

          if (NAME_MAX_MINIMUM < length)
            {
              check_component_lengths = true;
              break;
            }

          start += length;
        }
    }

  if (check_component_lengths)
    {
      /* The limit on file name components for the current component.
         This defaults to NAME_MAX_MINIMUM, for the sake of non-POSIX
         systems (NFS, say?) where pathconf fails on "." or "/" with
         errno == ENOENT.  */
      size_t name_max = NAME_MAX_MINIMUM;

      /* If nonzero, the known limit on file name components.  */
      size_t known_name_max = (check_basic_portability ? _POSIX_NAME_MAX : 0);

      for (start = file; *(start = component_start (start)); )
        {
          size_t length;

          if (known_name_max)
            name_max = known_name_max;
          else
            {
              long int len;
              char const *dir = (start == file ? "." : file);
              char c = *start;
              errno = 0;
              *start = '\0';
              len = pathconf (dir, _PC_NAME_MAX);
              *start = c;
              if (0 <= len)
                name_max = MIN (len, SSIZE_MAX);
              else
                switch (errno)
                  {
                  case 0:
                    /* There is no limit.  */
                    name_max = SIZE_MAX;
                    break;

                  case ENOENT:
                    /* DIR does not exist; use its parent's maximum.  */
                    known_name_max = name_max;
                    break;

                  default:
                    *start = '\0';
                    error (0, errno, "%s", dir);
                    *start = c;
                    return false;
                  }
            }

          length = component_len (start);

          if (name_max < length)
            {
              unsigned long int len = length;
              unsigned long int maxlen = name_max;
              char c = start[len];
              start[len] = '\0';
              error (0, 0,
                     _("limit %lu exceeded by length %lu "
                       "of file name component %s"),
                     maxlen, len, quote (start));
              start[len] = c;
              return false;
            }

          start += length;
        }
    }

  return true;
}
