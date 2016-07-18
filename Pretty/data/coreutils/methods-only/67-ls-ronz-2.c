/* Print information about F in long format.  */
static void
print_long_format (const struct fileinfo *f)
{
  char modebuf[12];
  char buf
    [LONGEST_HUMAN_READABLE + 1		/* inode */
     + LONGEST_HUMAN_READABLE + 1	/* size in blocks */
     + sizeof (modebuf) - 1 + 1		/* mode string */
     + INT_BUFSIZE_BOUND (uintmax_t)	/* st_nlink */
     + LONGEST_HUMAN_READABLE + 2	/* major device number */
     + LONGEST_HUMAN_READABLE + 1	/* minor device number */
     + TIME_STAMP_LEN_MAXIMUM + 1	/* max length of time/date */
     ];
  size_t s;
  char *p;
  struct timespec when_timespec;
  struct tm *when_local;

  /* Compute the mode string, except remove the trailing space if no
     file in this directory has an ACL or SELinux security context.  */
  if (f->stat_ok)
    filemodestring (&f->stat, modebuf);
  else
    {
      modebuf[0] = filetype_letter[f->filetype];
      memset (modebuf + 1, '?', 10);
      modebuf[11] = '\0';
    }
  if (! any_has_acl)
    modebuf[10] = '\0';
  else if (f->acl_type == ACL_T_SELINUX_ONLY)
    modebuf[10] = '.';
  else if (f->acl_type == ACL_T_YES)
    modebuf[10] = '+';

  switch (time_type)
    {
    case time_ctime:
      when_timespec = get_stat_ctime (&f->stat);
      break;
    case time_mtime:
      when_timespec = get_stat_mtime (&f->stat);
      break;
    case time_atime:
      when_timespec = get_stat_atime (&f->stat);
      break;
    default:
      abort ();
    }

  p = buf;

  if (print_inode)
    {
      char hbuf[INT_BUFSIZE_BOUND (uintmax_t)];
      sprintf (p, "%*s ", inode_number_width,
               format_inode (hbuf, sizeof hbuf, f));
      /* Increment by strlen (p) here, rather than by inode_number_width + 1.
         The latter is wrong when inode_number_width is zero.  */
      p += strlen (p);
    }

  if (print_block_size)
    {
      char hbuf[LONGEST_HUMAN_READABLE + 1];
      char const *blocks =
        (! f->stat_ok
         ? "?"
         : human_readable (ST_NBLOCKS (f->stat), hbuf, human_output_opts,
                           ST_NBLOCKSIZE, output_block_size));
      int pad;
      for (pad = block_size_width - mbswidth (blocks, 0); 0 < pad; pad--)
        *p++ = ' ';
      while ((*p++ = *blocks++))
        continue;
      p[-1] = ' ';
    }

  /* The last byte of the mode string is the POSIX
     "optional alternate access method flag".  */
  {
    char hbuf[INT_BUFSIZE_BOUND (uintmax_t)];
    sprintf (p, "%s %*s ", modebuf, nlink_width,
             ! f->stat_ok ? "?" : umaxtostr (f->stat.st_nlink, hbuf));
  }
  /* Increment by strlen (p) here, rather than by, e.g.,
     sizeof modebuf - 2 + any_has_acl + 1 + nlink_width + 1.
     The latter is wrong when nlink_width is zero.  */
  p += strlen (p);

  DIRED_INDENT ();

  if (print_owner || print_group || print_author || print_scontext)
    {
      DIRED_FPUTS (buf, stdout, p - buf);

      if (print_owner)
        format_user (f->stat.st_uid, owner_width, f->stat_ok);

      if (print_group)
        format_group (f->stat.st_gid, group_width, f->stat_ok);

      if (print_author)
        format_user (f->stat.st_author, author_width, f->stat_ok);

      if (print_scontext)
        format_user_or_group (f->scontext, 0, scontext_width);

      p = buf;
    }

  if (f->stat_ok
      && (S_ISCHR (f->stat.st_mode) || S_ISBLK (f->stat.st_mode)))
    {
      char majorbuf[INT_BUFSIZE_BOUND (uintmax_t)];
      char minorbuf[INT_BUFSIZE_BOUND (uintmax_t)];
      int blanks_width = (file_size_width
                          - (major_device_number_width + 2
                             + minor_device_number_width));
      sprintf (p, "%*s, %*s ",
               major_device_number_width + MAX (0, blanks_width),
               umaxtostr (major (f->stat.st_rdev), majorbuf),
               minor_device_number_width,
               umaxtostr (minor (f->stat.st_rdev), minorbuf));
      p += file_size_width + 1;
    }
  else
    {
      char hbuf[LONGEST_HUMAN_READABLE + 1];
      char const *size =
        (! f->stat_ok
         ? "?"
         : human_readable (unsigned_file_size (f->stat.st_size),
                           hbuf, human_output_opts, 1, file_output_block_size));
      int pad;
      for (pad = file_size_width - mbswidth (size, 0); 0 < pad; pad--)
        *p++ = ' ';
      while ((*p++ = *size++))
        continue;
      p[-1] = ' ';
    }

  when_local = localtime (&when_timespec.tv_sec);
  s = 0;
  *p = '\1';

  if (f->stat_ok && when_local)
    {
      struct timespec six_months_ago;
      bool recent;
      char const *fmt;

      /* If the file appears to be in the future, update the current
         time, in case the file happens to have been modified since
         the last time we checked the clock.  */
      if (timespec_cmp (current_time, when_timespec) < 0)
        {
          /* Note that gettime may call gettimeofday which, on some non-
             compliant systems, clobbers the buffer used for localtime's result.
             But it's ok here, because we use a gettimeofday wrapper that
             saves and restores the buffer around the gettimeofday call.  */
          gettime (&current_time);
        }

      /* Consider a time to be recent if it is within the past six
         months.  A Gregorian year has 365.2425 * 24 * 60 * 60 ==
         31556952 seconds on the average.  Write this value as an
         integer constant to avoid floating point hassles.  */
      six_months_ago.tv_sec = current_time.tv_sec - 31556952 / 2;
      six_months_ago.tv_nsec = current_time.tv_nsec;

      recent = (timespec_cmp (six_months_ago, when_timespec) < 0
                && (timespec_cmp (when_timespec, current_time) < 0));
      fmt = long_time_format[recent];

      /* We assume here that all time zones are offset from UTC by a
         whole number of seconds.  */
      s = align_nstrftime (p, TIME_STAMP_LEN_MAXIMUM + 1, fmt,
                           when_local, 0, when_timespec.tv_nsec);
    }

  if (s || !*p)
    {
      p += s;
      *p++ = ' ';

      /* NUL-terminate the string -- fputs (via DIRED_FPUTS) requires it.  */
      *p = '\0';
    }
  else
    {
      /* The time cannot be converted using the desired format, so
         print it as a huge integer number of seconds.  */
      char hbuf[INT_BUFSIZE_BOUND (intmax_t)];
      sprintf (p, "%*s ", long_time_expected_width (),
               (! f->stat_ok
                ? "?"
                : timetostr (when_timespec.tv_sec, hbuf)));
      /* FIXME: (maybe) We discarded when_timespec.tv_nsec. */
      p += strlen (p);
    }

  DIRED_FPUTS (buf, stdout, p - buf);
  size_t w = print_name_with_quoting (f, false, &dired_obstack, p - buf);

  if (f->filetype == symbolic_link)
    {
      if (f->linkname)
        {
          DIRED_FPUTS_LITERAL (" -> ", stdout);
          print_name_with_quoting (f, true, NULL, (p - buf) + w + 4);
          if (indicator_style != none)
            print_type_indicator (true, f->linkmode, unknown);
        }
    }
  else if (indicator_style != none)
    print_type_indicator (f->stat_ok, f->stat.st_mode, f->filetype);
}

