/* Output the line in linebuffer LINE to standard output
   provided that the switches say it should be output.
   MATCH is true if the line matches the previous line.
   If requested, print the number of times it occurred, as well;
   LINECOUNT + 1 is the number of times that the line occurred. */

static void
writeline (struct linebuffer const *line,
           bool match, uintmax_t linecount)
{
  if (! (linecount == 0 ? output_unique
         : !match ? output_first_repeated
         : output_later_repeated))
    return;

  if (countmode == count_occurrences)
    printf ("%7" PRIuMAX " ", linecount + 1);

  fwrite (line->buffer, sizeof (char), line->length, stdout);
}

