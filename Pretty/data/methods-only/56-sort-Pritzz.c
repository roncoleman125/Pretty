/* Write LINE to output stream FP; the output file's name is
   OUTPUT_FILE if OUTPUT_FILE is nonnull, and is the standard output
   otherwise.  If debugging is enabled and FP is standard output,
   append some debugging information.  */

static void
write_line (struct line const *line, FILE *fp, char const *output_file)
{
  char *buf = line->text;
  size_t n_bytes = line->length;
  char *ebuf = buf + n_bytes;

  if (!output_file && debug)
    {
      /* Convert TAB to '>' and EOL to \n, and then output debugging info.  */
      char const *c = buf;

      while (c < ebuf)
        {
          char wc = *c++;
          if (wc == '\t')
            wc = '>';
          else if (c == ebuf)
            wc = '\n';
          if (fputc (wc, fp) == EOF)
            die (_("write failed"), output_file);
        }

      debug_line (line);
    }
  else
    {
      ebuf[-1] = eolchar;
      if (fwrite (buf, 1, n_bytes, fp) != n_bytes)
        die (_("write failed"), output_file);
      ebuf[-1] = '\0';
    }
}

