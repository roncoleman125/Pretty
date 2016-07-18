static bool
binary_operator (bool l_is_l)
{
  int op;
  struct stat stat_buf, stat_spare;
  /* Is the right integer expression of the form '-l string'? */
  bool r_is_l;

  if (l_is_l)
    advance (false);
  op = pos + 1;

  if ((op < argc - 2) && STREQ (argv[op + 1], "-l"))
    {
      r_is_l = true;
      advance (false);
    }
  else
    r_is_l = false;

  if (argv[op][0] == '-')
    {
      /* check for eq, nt, and stuff */
      if ((((argv[op][1] == 'l' || argv[op][1] == 'g')
            && (argv[op][2] == 'e' || argv[op][2] == 't'))
           || (argv[op][1] == 'e' && argv[op][2] == 'q')
           || (argv[op][1] == 'n' && argv[op][2] == 'e'))
          && !argv[op][3])
        {
          char lbuf[INT_BUFSIZE_BOUND (uintmax_t)];
          char rbuf[INT_BUFSIZE_BOUND (uintmax_t)];
          char const *l = (l_is_l
                           ? umaxtostr (strlen (argv[op - 1]), lbuf)
                           : find_int (argv[op - 1]));
          char const *r = (r_is_l
                           ? umaxtostr (strlen (argv[op + 2]), rbuf)
                           : find_int (argv[op + 1]));
          int cmp = strintcmp (l, r);
          bool xe_operator = (argv[op][2] == 'e');
          pos += 3;
          return (argv[op][1] == 'l' ? cmp < xe_operator
                  : argv[op][1] == 'g' ? cmp > - xe_operator
                  : (cmp != 0) == xe_operator);
        }

      switch (argv[op][1])
        {
        default:
          break;

        case 'n':
          if (argv[op][2] == 't' && !argv[op][3])
            {
              /* nt - newer than */
              struct timespec lt, rt;
              bool le, re;
              pos += 3;
              if (l_is_l || r_is_l)
                test_syntax_error (_("-nt does not accept -l"), NULL);
              le = get_mtime (argv[op - 1], &lt);
              re = get_mtime (argv[op + 1], &rt);
              return le && (!re || timespec_cmp (lt, rt) > 0);
            }
          break;

        case 'e':
          if (argv[op][2] == 'f' && !argv[op][3])
            {
              /* ef - hard link? */
              pos += 3;
              if (l_is_l || r_is_l)
                test_syntax_error (_("-ef does not accept -l"), NULL);
              return (stat (argv[op - 1], &stat_buf) == 0
                      && stat (argv[op + 1], &stat_spare) == 0
                      && stat_buf.st_dev == stat_spare.st_dev
                      && stat_buf.st_ino == stat_spare.st_ino);
            }
          break;

        case 'o':
          if ('t' == argv[op][2] && '\000' == argv[op][3])
            {
              /* ot - older than */
              struct timespec lt, rt;
              bool le, re;
              pos += 3;
              if (l_is_l || r_is_l)
                test_syntax_error (_("-ot does not accept -l"), NULL);
              le = get_mtime (argv[op - 1], &lt);
              re = get_mtime (argv[op + 1], &rt);
              return re && (!le || timespec_cmp (lt, rt) < 0);
            }
          break;
        }

      /* FIXME: is this dead code? */
      test_syntax_error (_("unknown binary operator"), argv[op]);
    }

  if (argv[op][0] == '=' && !argv[op][1])
    {
      bool value = STREQ (argv[pos], argv[pos + 2]);
      pos += 3;
      return value;
    }

  if (STREQ (argv[op], "!="))
    {
      bool value = !STREQ (argv[pos], argv[pos + 2]);
      pos += 3;
      return value;
    }

  /* Not reached.  */
  abort ();
}

