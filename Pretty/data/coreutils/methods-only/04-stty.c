static void
set_control_char (struct control_info const *info, const char *arg,
                  struct termios *mode)
{
  unsigned long int value;

  if (STREQ (info->name, "min") || STREQ (info->name, "time"))
    value = integer_arg (arg, TYPE_MAXIMUM (cc_t));
  else if (arg[0] == '\0' || arg[1] == '\0')
    value = to_uchar (arg[0]);
  else if (STREQ (arg, "^-") || STREQ (arg, "undef"))
    value = _POSIX_VDISABLE;
  else if (arg[0] == '^' && arg[1] != '\0')	/* Ignore any trailing junk. */
    {
      if (arg[1] == '?')
        value = 127;
      else
        value = to_uchar (arg[1]) & ~0140; /* Non-letters get weird results. */
    }
  else
    value = integer_arg (arg, TYPE_MAXIMUM (cc_t));
  mode->c_cc[info->offset] = value;
}

