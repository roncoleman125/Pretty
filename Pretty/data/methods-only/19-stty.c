/* Return a string that is the printable representation of character CH.  */
/* Adapted from `cat' by Torbjorn Granlund.  */

static const char *
visible (cc_t ch)
{
  static char buf[10];
  char *bpout = buf;

  if (ch == _POSIX_VDISABLE)
    return "<undef>";

  if (ch >= 32)
    {
      if (ch < 127)
        *bpout++ = ch;
      else if (ch == 127)
        {
          *bpout++ = '^';
          *bpout++ = '?';
        }
      else
        {
          *bpout++ = 'M';
          *bpout++ = '-';
          if (ch >= 128 + 32)
            {
              if (ch < 128 + 127)
                *bpout++ = ch - 128;
              else
                {
                  *bpout++ = '^';
                  *bpout++ = '?';
                }
            }
          else
            {
              *bpout++ = '^';
              *bpout++ = ch - 128 + 64;
            }
        }
    }
  else
    {
      *bpout++ = '^';
      *bpout++ = ch + 64;
    }
  *bpout = '\0';
  return (const char *) buf;
}

