/* Print stat info.  Return zero upon success, nonzero upon failure.  */
static bool
print_stat (char *pformat, size_t prefix_len, unsigned int m,
            char const *filename, void const *data)
{
  struct stat *statbuf = (struct stat *) data;
  struct passwd *pw_ent;
  struct group *gw_ent;
  bool fail = false;

  switch (m)
    {
    case 'n':
      out_string (pformat, prefix_len, filename);
      break;
    case 'N':
      out_string (pformat, prefix_len, quote (filename));
      if (S_ISLNK (statbuf->st_mode))
        {
          char *linkname = areadlink_with_size (filename, statbuf->st_size);
          if (linkname == NULL)
            {
              error (0, errno, _("cannot read symbolic link %s"),
                     quote (filename));
              return true;
            }
          printf (" -> ");
          out_string (pformat, prefix_len, quote (linkname));
          free (linkname);
        }
      break;
    case 'd':
      out_uint (pformat, prefix_len, statbuf->st_dev);
      break;
    case 'D':
      out_uint_x (pformat, prefix_len, statbuf->st_dev);
      break;
    case 'i':
      out_uint (pformat, prefix_len, statbuf->st_ino);
      break;
    case 'a':
      out_uint_o (pformat, prefix_len, statbuf->st_mode & CHMOD_MODE_BITS);
      break;
    case 'A':
      out_string (pformat, prefix_len, human_access (statbuf));
      break;
    case 'f':
      out_uint_x (pformat, prefix_len, statbuf->st_mode);
      break;
    case 'F':
      out_string (pformat, prefix_len, file_type (statbuf));
      break;
    case 'h':
      out_uint (pformat, prefix_len, statbuf->st_nlink);
      break;
    case 'u':
      out_uint (pformat, prefix_len, statbuf->st_uid);
      break;
    case 'U':
      setpwent ();
      pw_ent = getpwuid (statbuf->st_uid);
      out_string (pformat, prefix_len,
                  pw_ent ? pw_ent->pw_name : "UNKNOWN");
      break;
    case 'g':
      out_uint (pformat, prefix_len, statbuf->st_gid);
      break;
    case 'G':
      setgrent ();
      gw_ent = getgrgid (statbuf->st_gid);
      out_string (pformat, prefix_len,
                  gw_ent ? gw_ent->gr_name : "UNKNOWN");
      break;
    case 't':
      out_uint_x (pformat, prefix_len, major (statbuf->st_rdev));
      break;
    case 'm':
      fail |= out_mount_point (filename, pformat, prefix_len, statbuf);
      break;
    case 'T':
      out_uint_x (pformat, prefix_len, minor (statbuf->st_rdev));
      break;
    case 's':
      out_uint (pformat, prefix_len, statbuf->st_size);
      break;
    case 'B':
      out_uint (pformat, prefix_len, ST_NBLOCKSIZE);
      break;
    case 'b':
      out_uint (pformat, prefix_len, ST_NBLOCKS (*statbuf));
      break;
    case 'o':
      out_uint (pformat, prefix_len, statbuf->st_blksize);
      break;
    case 'w':
      {
        struct timespec t = get_stat_birthtime (statbuf);
        if (t.tv_nsec < 0)
          out_string (pformat, prefix_len, "-");
        else
          out_string (pformat, prefix_len, human_time (t));
      }
      break;
    case 'W':
      out_epoch_sec (pformat, prefix_len, statbuf,
                     neg_to_zero (get_stat_birthtime (statbuf)));
      break;
    case 'x':
      out_string (pformat, prefix_len, human_time (get_stat_atime (statbuf)));
      break;
    case 'X':
      out_epoch_sec (pformat, prefix_len, statbuf, get_stat_atime (statbuf));
      break;
    case 'y':
      out_string (pformat, prefix_len, human_time (get_stat_mtime (statbuf)));
      break;
    case 'Y':
      out_epoch_sec (pformat, prefix_len, statbuf, get_stat_mtime (statbuf));
      break;
    case 'z':
      out_string (pformat, prefix_len, human_time (get_stat_ctime (statbuf)));
      break;
    case 'Z':
      out_epoch_sec (pformat, prefix_len, statbuf, get_stat_ctime (statbuf));
      break;
    case 'C':
      fail |= out_file_context (pformat, prefix_len, filename);
      break;
    default:
      fputc ('?', stdout);
      break;
    }
  return fail;
}

