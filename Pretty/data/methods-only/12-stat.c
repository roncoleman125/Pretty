/* Print statfs info.  Return zero upon success, nonzero upon failure.  */
static bool ATTRIBUTE_WARN_UNUSED_RESULT
print_statfs (char *pformat, size_t prefix_len, unsigned int m,
              char const *filename,
              void const *data)
{
  STRUCT_STATVFS const *statfsbuf = data;
  bool fail = false;

  switch (m)
    {
    case 'n':
      out_string (pformat, prefix_len, filename);
      break;

    case 'i':
      {
        uintmax_t fsid = statfsbuf->f_fsid;
        typedef unsigned int fsid_word;
        verify (alignof (STRUCT_STATVFS) % alignof (fsid_word) == 0);
        verify (offsetof (STRUCT_STATVFS, f_fsid) % alignof (fsid_word) == 0);
        verify (sizeof statfsbuf->f_fsid % alignof (fsid_word) == 0);
        fsid_word const *p = (fsid_word *) &statfsbuf->f_fsid;

        /* Assume a little-endian word order, as that is compatible
           with glibc's statvfs implementation.  */
        uintmax_t fsid = 0;
        int words = sizeof statfsbuf->f_fsid / sizeof *p;
        int i;
        for (i = 0; i < words && i * sizeof *p < sizeof fsid; i++)
          {
            uintmax_t u = p[words - 1 - i];
            fsid |= u << (i * CHAR_BIT * sizeof *p);
          }
        out_uint_x (pformat, prefix_len, fsid);
      }
      break;

    case 'l':
      OUT_NAMEMAX (pformat, prefix_len, SB_F_NAMEMAX (statfsbuf));
      break;
    case 't':
      out_uint_x (pformat, prefix_len, statfsbuf->f_type);
      fputc ('?', stdout);
      break;
    case 'T':
      out_string (pformat, prefix_len, human_fstype (statfsbuf));
      break;
    case 'b':
      out_int (pformat, prefix_len, statfsbuf->f_blocks);
      break;
    case 'f':
      out_int (pformat, prefix_len, statfsbuf->f_bfree);
      break;
    case 'a':
      out_int (pformat, prefix_len, statfsbuf->f_bavail);
      break;
    case 's':
      out_uint (pformat, prefix_len, statfsbuf->f_bsize);
      break;
    case 'S':
      {
        uintmax_t frsize = STATFS_FRSIZE (statfsbuf);
        if (! frsize)
          frsize = statfsbuf->f_bsize;
        out_uint (pformat, prefix_len, frsize);
      }
      break;
    case 'c':
      out_uint (pformat, prefix_len, statfsbuf->f_files);
      break;
    case 'd':
      out_int (pformat, prefix_len, statfsbuf->f_ffree);
      break;
    default:
      fputc ('?', stdout);
      break;
    }
  return fail;
}

