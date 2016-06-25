/* Parse each line in INPUT_FILENAME as with --date and display each
   resulting time and date.  If the file cannot be opened, tell why
   then exit.  Issue a diagnostic for any lines that cannot be parsed.
   Return true if successful.  */

static bool
batch_convert (const char *input_filename, const char *format)
{
  bool ok;
  FILE *in_stream;
  char *line;
  size_t buflen;
  struct timespec when;

  if (STREQ (input_filename, "-"))
    {
      input_filename = _("standard input");
      in_stream = stdin;
    }
  else
    {
      in_stream = fopen (input_filename, "r");
      if (in_stream == NULL)
        {
          error (EXIT_FAILURE, errno, "%s", quote (input_filename));
        }
    }

  line = NULL;
  buflen = 0;
  ok = true;
  while (1)
    {
      ssize_t line_length = getline (&line, &buflen, in_stream);
      if (line_length < 0)
        {
          /* FIXME: detect/handle error here.  */
          break;
        }

      if (! parse_datetime (&when, line, NULL))
        {
          if (line[line_length - 1] == '\n')
            line[line_length - 1] = '\0';
          error (0, 0, _("invalid date %s"), quote (line));
          ok = false;
        }
      else
        {
          ok &= show_date (format, when);
        }
    }

  if (fclose (in_stream) == EOF)
    error (EXIT_FAILURE, errno, "%s", quote (input_filename));

  free (line);

  return ok;
}

