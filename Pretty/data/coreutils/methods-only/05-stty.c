static void
set_speed (enum speed_setting type, const char *arg, struct termios *mode)
{
  speed_t baud;

  baud = string_to_baud (arg);
  if (type == input_speed || type == both_speeds)
    cfsetispeed (mode, baud);
  if (type == output_speed || type == both_speeds)
    cfsetospeed (mode, baud);
}


static int
get_win_size (int fd, struct winsize *win)
{
  int err = ioctl (fd, TIOCGWINSZ, (char *) win);
  return err;
}

static void
set_window_size (int rows, int cols, char const *device_name)
{
  struct winsize win;

  if (get_win_size (STDIN_FILENO, &win))
    {
      if (errno != EINVAL)
        error (EXIT_FAILURE, errno, "%s", device_name);
      memset (&win, 0, sizeof (win));
    }

  if (rows >= 0)
    win.ws_row = rows;
  if (cols >= 0)
    win.ws_col = cols;

  /* Alexander Dupuy <dupuy@cs.columbia.edu> wrote:
     The following code deals with a bug in the SunOS 4.x (and 3.x?) kernel.
     This comment from sys/ttold.h describes Sun's twisted logic - a better
     test would have been (ts_lines > 64k || ts_cols > 64k || ts_cols == 0).
     At any rate, the problem is gone in Solaris 2.x.

     Unfortunately, the old TIOCSSIZE code does collide with TIOCSWINSZ,
     but they can be disambiguated by checking whether a "struct ttysize"
     structure's "ts_lines" field is greater than 64K or not.  If so,
     it's almost certainly a "struct winsize" instead.

     At any rate, the bug manifests itself when ws_row == 0; the symptom is
     that ws_row is set to ws_col, and ws_col is set to (ws_xpixel<<16) +
     ws_ypixel.  Since GNU stty sets rows and columns separately, this bug
     caused "stty rows 0 cols 0" to set rows to cols and cols to 0, while
     "stty cols 0 rows 0" would do the right thing.  On a little-endian
     machine like the sun386i, the problem is the same, but for ws_col == 0.

     The workaround is to do the ioctl once with row and col = 1 to set the
     pixel info, and then do it again using a TIOCSSIZE to set rows/cols.  */

  if (win.ws_row == 0 || win.ws_col == 0)
    {
      struct ttysize ttysz;

      ttysz.ts_lines = win.ws_row;
      ttysz.ts_cols = win.ws_col;

      win.ws_row = 1;
      win.ws_col = 1;

      if (ioctl (STDIN_FILENO, TIOCSWINSZ, (char *) &win))
        error (EXIT_FAILURE, errno, "%s", device_name);

      if (ioctl (STDIN_FILENO, TIOCSSIZE, (char *) &ttysz))
        error (EXIT_FAILURE, errno, "%s", device_name);
      return;
    }

  if (ioctl (STDIN_FILENO, TIOCSWINSZ, (char *) &win))
    error (EXIT_FAILURE, errno, "%s", device_name);
}

static void
display_window_size (bool fancy, char const *device_name)
{
  struct winsize win;

  if (get_win_size (STDIN_FILENO, &win))
    {
      if (errno != EINVAL)
        error (EXIT_FAILURE, errno, "%s", device_name);
      if (!fancy)
        error (EXIT_FAILURE, 0,
               _("%s: no size information for this device"), device_name);
    }
  else
    {
      wrapf (fancy ? "rows %d; columns %d;" : "%d %d\n",
             win.ws_row, win.ws_col);
      if (!fancy)
        current_col = 0;
    }
}

