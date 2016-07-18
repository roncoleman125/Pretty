static int
screen_columns (void)
{
  struct winsize win;

  /* With Solaris 2.[123], this ioctl fails and errno is set to
     EINVAL for telnet (but not rlogin) sessions.
     On ISC 3.0, it fails for the console and the serial port
     (but it works for ptys).
     It can also fail on any system when stdout isn't a tty.
     In case of any failure, just use the default.  */
  if (get_win_size (STDOUT_FILENO, &win) == 0 && 0 < win.ws_col)
    return win.ws_col;
  {
    /* Use $COLUMNS if it's in [1..INT_MAX].  */
    char *col_string = getenv ("COLUMNS");
    long int n_columns;
    if (!(col_string != NULL
          && xstrtol (col_string, NULL, 0, &n_columns, "") == LONGINT_OK
          && 0 < n_columns
          && n_columns <= INT_MAX))
      n_columns = 80;
    return n_columns;
  }
}

