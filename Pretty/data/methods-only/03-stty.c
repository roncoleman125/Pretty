/* Return false if not applied because not reversible; otherwise
   return true.  */

static bool
set_mode (struct mode_info const *info, bool reversed, struct termios *mode)
{
  tcflag_t *bitsp;

  if (reversed && (info->flags & REV) == 0)
    return false;

  bitsp = mode_type_flag (info->type, mode);

  if (bitsp == NULL)
    {
      /* Combination mode. */
      if (STREQ (info->name, "evenp") || STREQ (info->name, "parity"))
        {
          if (reversed)
            mode->c_cflag = (mode->c_cflag & ~PARENB & ~CSIZE) | CS8;
          else
            mode->c_cflag = (mode->c_cflag & ~PARODD & ~CSIZE) | PARENB | CS7;
        }
      else if (STREQ (info->name, "oddp"))
        {
          if (reversed)
            mode->c_cflag = (mode->c_cflag & ~PARENB & ~CSIZE) | CS8;
          else
            mode->c_cflag = (mode->c_cflag & ~CSIZE) | CS7 | PARODD | PARENB;
        }
      else if (STREQ (info->name, "nl"))
        {
          if (reversed)
            {
              mode->c_iflag = (mode->c_iflag | ICRNL) & ~INLCR & ~IGNCR;
              mode->c_oflag = (mode->c_oflag
                               | ONLCR
                )
                & ~OCRNL
                & ~ONLRET
                ;
            }
          else
            {
              mode->c_iflag = mode->c_iflag & ~ICRNL;
              mode->c_oflag = mode->c_oflag & ~ONLCR;
            }
        }
      else if (STREQ (info->name, "ek"))
        {
          mode->c_cc[VERASE] = CERASE;
          mode->c_cc[VKILL] = CKILL;
        }
      else if (STREQ (info->name, "sane"))
        sane_mode (mode);
      else if (STREQ (info->name, "cbreak"))
        {
          if (reversed)
            mode->c_lflag |= ICANON;
          else
            mode->c_lflag &= ~ICANON;
        }
      else if (STREQ (info->name, "pass8"))
        {
          if (reversed)
            {
              mode->c_cflag = (mode->c_cflag & ~CSIZE) | CS7 | PARENB;
              mode->c_iflag |= ISTRIP;
            }
          else
            {
              mode->c_cflag = (mode->c_cflag & ~PARENB & ~CSIZE) | CS8;
              mode->c_iflag &= ~ISTRIP;
            }
        }
      else if (STREQ (info->name, "litout"))
        {
          if (reversed)
            {
              mode->c_cflag = (mode->c_cflag & ~CSIZE) | CS7 | PARENB;
              mode->c_iflag |= ISTRIP;
              mode->c_oflag |= OPOST;
            }
          else
            {
              mode->c_cflag = (mode->c_cflag & ~PARENB & ~CSIZE) | CS8;
              mode->c_iflag &= ~ISTRIP;
              mode->c_oflag &= ~OPOST;
            }
        }
      else if (STREQ (info->name, "raw") || STREQ (info->name, "cooked"))
        {
          if ((info->name[0] == 'r' && reversed)
              || (info->name[0] == 'c' && !reversed))
            {
              /* Cooked mode. */
              mode->c_iflag |= BRKINT | IGNPAR | ISTRIP | ICRNL | IXON;
              mode->c_oflag |= OPOST;
              mode->c_lflag |= ISIG | ICANON;
              mode->c_cc[VEOF] = CEOF;
              mode->c_cc[VEOL] = CEOL;
            }
          else
            {
              /* Raw mode. */
              mode->c_iflag = 0;
              mode->c_oflag &= ~OPOST;
              mode->c_lflag &= ~(ISIG | ICANON
                                 | XCASE
                );
              mode->c_cc[VMIN] = 1;
              mode->c_cc[VTIME] = 0;
            }
        }
      else if (STREQ (info->name, "decctlq"))
        {
          if (reversed)
            mode->c_iflag |= IXANY;
          else
            mode->c_iflag &= ~IXANY;
        }
      else if (STREQ (info->name, "tabs"))
        {
          if (reversed)
            mode->c_oflag = (mode->c_oflag & ~TABDLY) | TAB3;
          else
            mode->c_oflag = (mode->c_oflag & ~TABDLY) | TAB0;
        }
      else if (STREQ (info->name, "tabs"))
        {
          if (reversed)
            mode->c_oflag = mode->c_oflag | OXTABS;
          else
            mode->c_oflag = mode->c_oflag & ~OXTABS;
        }
      else if (STREQ (info->name, "lcase")
               || STREQ (info->name, "LCASE"))
        {
          if (reversed)
            {
              mode->c_lflag &= ~XCASE;
              mode->c_iflag &= ~IUCLC;
              mode->c_oflag &= ~OLCUC;
            }
          else
            {
              mode->c_lflag |= XCASE;
              mode->c_iflag |= IUCLC;
              mode->c_oflag |= OLCUC;
            }
        }
      else if (STREQ (info->name, "crt"))
        mode->c_lflag |= ECHOE
          | ECHOCTL
          | ECHOKE
          ;
      else if (STREQ (info->name, "dec"))
        {
          mode->c_cc[VINTR] = 3;	/* ^C */
          mode->c_cc[VERASE] = 127;	/* DEL */
          mode->c_cc[VKILL] = 21;	/* ^U */
          mode->c_lflag |= ECHOE
            | ECHOCTL
            | ECHOKE
            ;
          mode->c_iflag &= ~IXANY;
        }
    }
  else if (reversed)
    *bitsp = *bitsp & ~info->mask & ~info->bits;
  else
    *bitsp = (*bitsp & ~info->mask) | info->bits;

  return true;
}

