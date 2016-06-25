/* Print formatted output line. Uses mostly arbitrary field sizes, probably
   will need tweaking if any of the localization stuff is done, or for 64 bit
   pids, etc. */
static void
print_line (int userlen, const char *user, const char state,
            int linelen, const char *line,
            const char *time_str, const char *idle, const char *pid,
            const char *comment, const char *exitstr)
{
  static char mesg[3] = { ' ', 'x', '\0' };
  char *buf;
  char x_idle[1 + IDLESTR_LEN + 1];
  char x_pid[1 + INT_STRLEN_BOUND (pid_t) + 1];
  char *x_exitstr;
  int err;

  mesg[1] = state;

  if (include_idle && !short_output && strlen (idle) < sizeof x_idle - 1)
    sprintf (x_idle, " %-6s", idle);
  else
    *x_idle = '\0';

  if (!short_output && strlen (pid) < sizeof x_pid - 1)
    sprintf (x_pid, " %10s", pid);
  else
    *x_pid = '\0';

  x_exitstr = xmalloc (include_exit ? 1 + MAX (12, strlen (exitstr)) + 1 : 1);
  if (include_exit)
    sprintf (x_exitstr, " %-12s", exitstr);
  else
    *x_exitstr = '\0';

  err = asprintf (&buf,
                  "%-8.*s"
                  "%s"
                  " %-12.*s"
                  " %-*s"
                  "%s"
                  "%s"
                  " %-8s"
                  "%s"
                  ,
                  userlen, user ? user : "   .",
                  include_mesg ? mesg : "",
                  linelen, line,
                  time_format_width,
                  time_str,
                  x_idle,
                  x_pid,
                  /* FIXME: it's not really clear whether the following
                     field should be in the short_output.  A strict reading
                     of SUSv2 would suggest not, but I haven't seen any
                     implementations that actually work that way... */
                  comment,
                  x_exitstr
                  );
  if (err == -1)
    xalloc_die ();

  {
    /* Remove any trailing spaces.  */
    char *p = buf + strlen (buf);
    while (*--p == ' ')
      /* empty */;
    *(p + 1) = '\0';
  }

  puts (buf);
  free (buf);
  free (x_exitstr);
}

