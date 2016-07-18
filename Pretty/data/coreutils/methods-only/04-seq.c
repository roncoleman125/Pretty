/* Return the default format given FIRST, STEP, and LAST.  */
static char const *
get_default_format (operand first, operand step, operand last)
{
  static char format_buf[sizeof "%0.Lf" + 2 * INT_STRLEN_BOUND (int)];

  int prec = MAX (first.precision, step.precision);

  if (prec != INT_MAX && last.precision != INT_MAX)
    {
      if (equal_width)
        {
          /* increase first_width by any increased precision in step */
          size_t first_width = first.width + (prec - first.precision);
          /* adjust last_width to use precision from first/step */
          size_t last_width = last.width + (prec - last.precision);
          if (last.precision && prec == 0)
            last_width--;  /* don't include space for '.' */
          if (last.precision == 0 && prec)
            last_width++;  /* include space for '.' */
          size_t width = MAX (first_width, last_width);
          if (width <= INT_MAX)
            {
              int w = width;
              sprintf (format_buf, "%%0%d.%dLf", w, prec);
              return format_buf;
            }
        }
      else
        {
          sprintf (format_buf, "%%.%dLf", prec);
          return format_buf;
        }
    }

  return "%Lg";
}

