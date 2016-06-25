static void
display_changed (struct termios *mode)
{
  int i;
  bool empty_line;
  tcflag_t *bitsp;
  unsigned long mask;
  enum mode_type prev_type = control;

  display_speed (mode, true);
  wrapf ("line = %d;", mode->c_line);
  putchar ('\n');
  current_col = 0;

  empty_line = true;
  for (i = 0; !STREQ (control_info[i].name, "min"); ++i)
    {
      if (mode->c_cc[control_info[i].offset] == control_info[i].saneval)
        continue;
      /* If swtch is the same as susp, don't print both.  */
      if (STREQ (control_info[i].name, "swtch"))
        continue;
      /* If eof uses the same slot as min, only print whichever applies.  */
      if ((mode->c_lflag & ICANON) == 0
          && (STREQ (control_info[i].name, "eof")
              || STREQ (control_info[i].name, "eol")))
        continue;

      empty_line = false;
      wrapf ("%s = %s;", control_info[i].name,
             visible (mode->c_cc[control_info[i].offset]));
    }
  if ((mode->c_lflag & ICANON) == 0)
    {
      wrapf ("min = %lu; time = %lu;\n",
             (unsigned long int) mode->c_cc[VMIN],
             (unsigned long int) mode->c_cc[VTIME]);
    }
  else if (!empty_line)
    putchar ('\n');
  current_col = 0;

  empty_line = true;
  for (i = 0; mode_info[i].name != NULL; ++i)
    {
      if (mode_info[i].flags & OMIT)
        continue;
      if (mode_info[i].type != prev_type)
        {
          if (!empty_line)
            {
              putchar ('\n');
              current_col = 0;
              empty_line = true;
            }
          prev_type = mode_info[i].type;
        }

      bitsp = mode_type_flag (mode_info[i].type, mode);
      mask = mode_info[i].mask ? mode_info[i].mask : mode_info[i].bits;
      if ((*bitsp & mask) == mode_info[i].bits)
        {
          if (mode_info[i].flags & SANE_UNSET)
            {
              wrapf ("%s", mode_info[i].name);
              empty_line = false;
            }
        }
      else if ((mode_info[i].flags & (SANE_SET | REV)) == (SANE_SET | REV))
        {
          wrapf ("-%s", mode_info[i].name);
          empty_line = false;
        }
    }
  if (!empty_line)
    putchar ('\n');
  current_col = 0;
}

