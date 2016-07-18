/* Output the line in linebuffer LINE to stream STREAM
   provided the switches say it should be output.
   CLASS is 1 for a line found only in file 1,
   2 for a line only in file 2, 3 for a line in both. */

static void
writeline (struct linebuffer const *line, FILE *stream, int class)
{
  switch (class)
    {
    case 1:
      if (!only_file_1)
        return;
      break;

    case 2:
      if (!only_file_2)
        return;
      /* Print a delimiter if we are printing lines from file 1.  */
      if (only_file_1)
        fputs (delimiter, stream);
      break;

    case 3:
      if (!both)
        return;
      /* Print a delimiter if we are printing lines from file 1.  */
      if (only_file_1)
        fputs (delimiter, stream);
      /* Print a delimiter if we are printing lines from file 2.  */
      if (only_file_2)
        fputs (delimiter, stream);
      break;
    }

  fwrite (line->buffer, sizeof (char), line->length, stream);
}

