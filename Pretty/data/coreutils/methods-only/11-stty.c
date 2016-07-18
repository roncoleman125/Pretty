static void
display_speed (struct termios *mode, bool fancy)
{
  if (cfgetispeed (mode) == 0 || cfgetispeed (mode) == cfgetospeed (mode))
    wrapf (fancy ? "speed %lu baud;" : "%lu\n",
           baud_to_value (cfgetospeed (mode)));
  else
    wrapf (fancy ? "ispeed %lu baud; ospeed %lu baud;" : "%lu %lu\n",
           baud_to_value (cfgetispeed (mode)),
           baud_to_value (cfgetospeed (mode)));
  if (!fancy)
    current_col = 0;
}

