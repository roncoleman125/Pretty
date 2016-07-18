/* FILE is the name of the file (or NULL for standard input)
   associated with the specified counters.  */
static void
write_counts (uintmax_t lines,
              uintmax_t words,
              uintmax_t chars,
              uintmax_t bytes,
              uintmax_t linelength,
              const char *file)
{
  static char const format_sp_int[] = " %*s";
  char const *format_int = format_sp_int + 1;
  char buf[INT_BUFSIZE_BOUND (uintmax_t)];

  if (print_lines)
    {
      printf (format_int, number_width, umaxtostr (lines, buf));
      format_int = format_sp_int;
    }
  if (print_words)
    {
      printf (format_int, number_width, umaxtostr (words, buf));
      format_int = format_sp_int;
    }
  if (print_chars)
    {
      printf (format_int, number_width, umaxtostr (chars, buf));
      format_int = format_sp_int;
    }
  if (print_bytes)
    {
      printf (format_int, number_width, umaxtostr (bytes, buf));
      format_int = format_sp_int;
    }
  if (print_linelength)
    {
      printf (format_int, number_width, umaxtostr (linelength, buf));
    }
  if (file)
    printf (" %s", file);
  putchar ('\n');
}

