static void
sane_mode (struct termios *mode)
{
  int i;
  tcflag_t *bitsp;

  for (i = 0; control_info[i].name; ++i)
    {
      if (STREQ (control_info[i].name, "min"))
        break;
      mode->c_cc[control_info[i].offset] = control_info[i].saneval;
    }

  for (i = 0; mode_info[i].name != NULL; ++i)
    {
      if (mode_info[i].flags & SANE_SET)
        {
          bitsp = mode_type_flag (mode_info[i].type, mode);
          *bitsp = (*bitsp & ~mode_info[i].mask) | mode_info[i].bits;
        }
      else if (mode_info[i].flags & SANE_UNSET)
        {
          bitsp = mode_type_flag (mode_info[i].type, mode);
          *bitsp = *bitsp & ~mode_info[i].mask & ~mode_info[i].bits;
        }
    }
}

