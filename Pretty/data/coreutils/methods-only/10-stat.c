/* Output the number of seconds since the Epoch, using a format that
   acts like printf's %f format.  */
static void
out_epoch_sec (char *pformat, size_t prefix_len, struct stat const *statbuf,
               struct timespec arg)
{
  char *dot = memchr (pformat, '.', prefix_len);
  size_t sec_prefix_len = prefix_len;
  int width = 0;
  int precision = 0;
  bool frac_left_adjust = false;

  if (dot)
    {
      sec_prefix_len = dot - pformat;
      pformat[prefix_len] = '\0';

      if (ISDIGIT (dot[1]))
        {
          long int lprec = strtol (dot + 1, NULL, 10);
          precision = (lprec <= INT_MAX ? lprec : INT_MAX);
        }
      else
        {
          precision = 9;
        }

      if (precision && ISDIGIT (dot[-1]))
        {
          /* If a nontrivial width is given, subtract the width of the
             decimal point and PRECISION digits that will be output
             later.  */
          char *p = dot;
          *dot = '\0';

          do
            --p;
          while (ISDIGIT (p[-1]));

          long int lwidth = strtol (p, NULL, 10);
          width = (lwidth <= INT_MAX ? lwidth : INT_MAX);
          if (1 < width)
            {
              p += (*p == '0');
              sec_prefix_len = p - pformat;
              int w_d = (decimal_point_len < width
                         ? width - decimal_point_len
                         : 0);
              if (1 < w_d)
                {
                  int w = w_d - precision;
                  if (1 < w)
                    {
                      char *dst = pformat;
                      for (char const *src = dst; src < p; src++)
                        {
                          if (*src == '-')
                            frac_left_adjust = true;
                          else
                            *dst++ = *src;
                        }
                      sec_prefix_len =
                        (dst - pformat
                         + (frac_left_adjust ? 0 : sprintf (dst, "%d", w)));
                    }
                }
            }
        }
    }

  int divisor = 1;
  for (int i = precision; i < 9; i++)
    divisor *= 10;
  int frac_sec = arg.tv_nsec / divisor;
  int int_len;

  if (TYPE_SIGNED (time_t))
    {
      bool minus_zero = false;
      if (arg.tv_sec < 0 && arg.tv_nsec != 0)
        {
          int frac_sec_modulus = 1000000000 / divisor;
          frac_sec = (frac_sec_modulus - frac_sec
                      - (arg.tv_nsec % divisor != 0));
          arg.tv_sec += (frac_sec != 0);
          minus_zero = (arg.tv_sec == 0);
        }
      int_len = (minus_zero
                 ? out_minus_zero (pformat, sec_prefix_len)
                 : out_int (pformat, sec_prefix_len, arg.tv_sec));
    }
  else
    int_len = out_uint (pformat, sec_prefix_len, arg.tv_sec);

  if (precision)
    {
      int prec = (precision < 9 ? precision : 9);
      int trailing_prec = precision - prec;
      int ilen = (int_len < 0 ? 0 : int_len);
      int trailing_width = (ilen < width && decimal_point_len < width - ilen
                            ? width - ilen - decimal_point_len - prec
                            : 0);
      printf ("%s%.*d%-*.*d", decimal_point, prec, frac_sec,
              trailing_width, trailing_prec, 0);
    }
}

