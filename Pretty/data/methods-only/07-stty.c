static tcflag_t *
mode_type_flag (enum mode_type type, struct termios *mode)
{
  switch (type)
    {
    case control:
      return &mode->c_cflag;

    case input:
      return &mode->c_iflag;

    case output:
      return &mode->c_oflag;

    case local:
      return &mode->c_lflag;

    case combination:
      return NULL;

    default:
      abort ();
    }
}

