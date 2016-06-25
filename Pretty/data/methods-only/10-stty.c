static void
display_all (struct termios *mode, char const *device_name)
{
  int i;
  tcflag_t *bitsp;
  unsigned long mask;
  enum mode_type prev_type = control;

  display_speed (mode, true);
  display_window_size (true, device_name);
  wrapf ("line = %d;", mode->c_line);
  putchar ('\n');
  current_col = 0;

  for (i = 0; ! STREQ (control_info[i].name, "min"); ++i)
    {
      /* If swtch is the same as susp, don't print both.  */
      if (STREQ (control_info[i].name, "swtch"))
        continue;
      /* If eof uses the same slot as min, only print whichever applies.  */
      if ((mode->c_lflag & ICANON) == 0
          && (STREQ (control_info[i].name, "eof")
              || STREQ (control_info[i].name, "eol")))
        continue;
      wrapf ("%s = %s;", control_info[i].name,
             visible (mode->c_cc[control_info[i].offset]));
    }
  if ((mode->c_lflag & ICANON) == 0)
    wrapf ("min = %lu; time = %lu;",
           (unsigned long int) mode->c_cc[VMIN],
           (unsigned long int) mode->c_cc[VTIME]);
  if (current_col != 0)
    putchar ('\n');
  current_col = 0;

  for (i = 0; mode_info[i].name != NULL; ++i)
    {
      if (mode_info[i].flags & OMIT)
        continue;
      if (mode_info[i].type != prev_type)
        {
          putchar ('\n');
          current_col = 0;
          prev_type = mode_info[i].type;
        }

      bitsp = mode_type_flag (mode_info[i].type, mode);
      mask = mode_info[i].mask ? mode_info[i].mask : mode_info[i].bits;
      if ((*bitsp & mask) == mode_info[i].bits)
        wrapf ("%s", mode_info[i].name);
      else if (mode_info[i].flags & REV)
        wrapf ("-%s", mode_info[i].name);
    }
  putchar ('\n');
  current_col = 0;
}

