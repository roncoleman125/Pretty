/* Fold file FILENAME, or standard input if FILENAME is "-",
   to stdout, with maximum line length WIDTH.
   Return true if successful.  */

static bool
fold_file (char const *filename, size_t width)
{
  FILE *istream;
  int c;
  size_t column = 0;		/* Screen column where next char will go. */
  size_t offset_out = 0;	/* Index in `line_out' for next char. */
  static char *line_out = NULL;
  static size_t allocated_out = 0;
  int saved_errno;

  if (STREQ (filename, "-"))
    {
      istream = stdin;
      have_read_stdin = true;
    }
  else
    istream = fopen (filename, "r");

  if (istream == NULL)
    {
      error (0, errno, "%s", filename);
      return false;
    }

  fadvise (istream, FADVISE_SEQUENTIAL);

  while ((c = getc (istream)) != EOF)
    {
      if (offset_out + 1 >= allocated_out)
        line_out = X2REALLOC (line_out, &allocated_out);

      if (c == '\n')
        {
          line_out[offset_out++] = c;
          fwrite (line_out, sizeof (char), offset_out, stdout);
          column = offset_out = 0;
          continue;
        }

    rescan:
      column = adjust_column (column, c);

      if (column > width)
        {
          /* This character would make the line too long.
             Print the line plus a newline, and make this character
             start the next line. */
          if (break_spaces)
            {
              bool found_blank = false;
              size_t logical_end = offset_out;

              /* Look for the last blank. */
              while (logical_end)
                {
                  --logical_end;
                  if (isblank (to_uchar (line_out[logical_end])))
                    {
                      found_blank = true;
                      break;
                    }
                }

              if (found_blank)
                {
                  size_t i;

                  /* Found a blank.  Don't output the part after it. */
                  logical_end++;
                  fwrite (line_out, sizeof (char), (size_t) logical_end,
                          stdout);
                  putchar ('\n');
                  /* Move the remainder to the beginning of the next line.
                     The areas being copied here might overlap. */
                  memmove (line_out, line_out + logical_end,
                           offset_out - logical_end);
                  offset_out -= logical_end;
                  for (column = i = 0; i < offset_out; i++)
                    column = adjust_column (column, line_out[i]);
                  goto rescan;
                }
            }

          if (offset_out == 0)
            {
              line_out[offset_out++] = c;
              continue;
            }

          line_out[offset_out++] = '\n';
          fwrite (line_out, sizeof (char), (size_t) offset_out, stdout);
          column = offset_out = 0;
          goto rescan;
        }

      line_out[offset_out++] = c;
    }

  saved_errno = errno;

  if (offset_out)
    fwrite (line_out, sizeof (char), (size_t) offset_out, stdout);

  if (ferror (istream))
    {
      error (0, saved_errno, "%s", filename);
      if (!STREQ (filename, "-"))
        fclose (istream);
      return false;
    }
  if (!STREQ (filename, "-") && fclose (istream) == EOF)
    {
      error (0, errno, "%s", filename);
      return false;
    }

  return true;
}

