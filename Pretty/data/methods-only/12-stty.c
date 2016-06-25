static void
display_recoverable (struct termios *mode)
{
  size_t i;

  printf ("%lx:%lx:%lx:%lx",
          (unsigned long int) mode->c_iflag,
          (unsigned long int) mode->c_oflag,
          (unsigned long int) mode->c_cflag,
          (unsigned long int) mode->c_lflag);
  for (i = 0; i < NCCS; ++i)
    printf (":%lx", (unsigned long int) mode->c_cc[i]);
  putchar ('\n');
}

