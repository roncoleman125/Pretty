/* Actually print the sequence of numbers in the specified range, with the
   given or default stepping and format.  */

static void
print_numbers (char const *fmt, struct layout layout,
               long double first, long double step, long double last)
{
  bool out_of_range = (step < 0 ? first < last : last < first);

  if (! out_of_range)
    {
      long double x = first;
      long double i;

      for (i = 1; ; i++)
        {
          long double x0 = x;
          printf (fmt, x);
          if (out_of_range)
            break;
          x = first + i * step;
          out_of_range = (step < 0 ? x < last : last < x);

          if (out_of_range)
            {
              /* If the number just past LAST prints as a value equal
                 to LAST, and prints differently from the previous
                 number, then print the number.  This avoids problems
                 with rounding.  For example, with the x86 it causes
                 "seq 0 0.000001 0.000003" to print 0.000003 instead
                 of stopping at 0.000002.  */

              bool print_extra_number = false;
              long double x_val;
              char *x_str;
              int x_strlen;
              setlocale (LC_NUMERIC, "C");
              x_strlen = asprintf (&x_str, fmt, x);
              setlocale (LC_NUMERIC, "");
              if (x_strlen < 0)
                xalloc_die ();
              x_str[x_strlen - layout.suffix_len] = '\0';

              if (xstrtold (x_str + layout.prefix_len, NULL, &x_val, c_strtold)
                  && x_val == last)
                {
                  char *x0_str = NULL;
                  if (asprintf (&x0_str, fmt, x0) < 0)
                    xalloc_die ();
                  print_extra_number = !STREQ (x0_str, x_str);
                  free (x0_str);
                }

              free (x_str);
              if (! print_extra_number)
                break;
            }

          fputs (separator, stdout);
        }

      fputs (terminator, stdout);
    }
}

