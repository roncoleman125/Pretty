/* Save line LINE to the output file and
   increment the character count for the current file. */

static void
save_line_to_file (const struct cstring *line)
{
  fwrite (line->str, sizeof (char), line->len, output_stream);
  bytes_written += line->len;
}

