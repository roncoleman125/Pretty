/* Read from stream STREAM, printing to standard output any selected bytes.  */

static void
cut_bytes (FILE *stream)
{
  size_t byte_idx;	/* Number of bytes in the line so far. */
  /* Whether to begin printing delimiters between ranges for the current line.
     Set after we've begun printing data corresponding to the first range.  */
  bool print_delimiter;

  byte_idx = 0;
  print_delimiter = false;
  while (1)
    {
      int c;		/* Each character from the file. */

      c = getc (stream);

      if (c == '\n')
        {
          putchar ('\n');
          byte_idx = 0;
          print_delimiter = false;
        }
      else if (c == EOF)
        {
          if (byte_idx > 0)
            putchar ('\n');
          break;
        }
      else
        {
          bool range_start;
          bool *rs = output_delimiter_specified ? &range_start : NULL;
          if (print_kth (++byte_idx, rs))
            {
              if (rs && *rs && print_delimiter)
                {
                  fwrite (output_delimiter_string, sizeof (char),
                          output_delimiter_length, stdout);
                }
              print_delimiter = true;
              putchar (c);
            }
        }
    }
}

