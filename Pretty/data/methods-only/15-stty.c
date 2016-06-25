/* Parse the output of display_recoverable.
   Return false if any part of it is invalid.  */
static bool
recover_mode (char const *arg, struct termios *mode)
{
  tcflag_t flag[4];
  char const *s = arg;
  size_t i;
  for (i = 0; i < 4; i++)
    {
      char *p;
      if (strtoul_tcflag_t (s, 16, &p, flag + i, ':') != 0)
        return false;
      s = p + 1;
    }
  mode->c_iflag = flag[0];
  mode->c_oflag = flag[1];
  mode->c_cflag = flag[2];
  mode->c_lflag = flag[3];

  for (i = 0; i < NCCS; ++i)
    {
      char *p;
      char delim = i < NCCS - 1 ? ':' : '\0';
      if (strtoul_cc_t (s, 16, &p, mode->c_cc + i, delim) != 0)
        return false;
      s = p + 1;
    }

  return true;
}

struct speed_map
{
  const char *string;		/* ASCII representation. */
  speed_t speed;		/* Internal form. */
  unsigned long int value;	/* Numeric value. */
};

static struct speed_map const speeds[] =
{
  {"0", B0, 0},
  {"50", B50, 50},
  {"75", B75, 75},
  {"110", B110, 110},
  {"134", B134, 134},
  {"134.5", B134, 134},
  {"150", B150, 150},
  {"200", B200, 200},
  {"300", B300, 300},
  {"600", B600, 600},
  {"1200", B1200, 1200},
  {"1800", B1800, 1800},
  {"2400", B2400, 2400},
  {"4800", B4800, 4800},
  {"9600", B9600, 9600},
  {"19200", B19200, 19200},
  {"38400", B38400, 38400},
  {"exta", B19200, 19200},
  {"extb", B38400, 38400},
  {"57600", B57600, 57600},
  {"115200", B115200, 115200},
  {"230400", B230400, 230400},
  {"460800", B460800, 460800},
  {"500000", B500000, 500000},
  {"576000", B576000, 576000},
  {"921600", B921600, 921600},
  {"1000000", B1000000, 1000000},
  {"1152000", B1152000, 1152000},
  {"1500000", B1500000, 1500000},
  {"2000000", B2000000, 2000000},
  {"2500000", B2500000, 2500000},
  {"3000000", B3000000, 3000000},
  {"3500000", B3500000, 3500000},
  {"4000000", B4000000, 4000000},
  {NULL, 0, 0}
};

