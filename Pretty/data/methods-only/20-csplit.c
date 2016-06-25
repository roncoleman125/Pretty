/* Write all lines from the beginning of the buffer up to, but
   not including, line LAST_LINE, to the current output file.
   If IGNORE is true, do not output lines selected here.
   ARGNUM is the index in ARGV of the current pattern. */

static void
write_to_file (uintmax_t last_line, bool ignore, int argnum)
{
  struct cstring *line;
  uintmax_t first_line;		/* First available input line. */
  uintmax_t lines;		/* Number of lines to output. */
  uintmax_t i;

  first_line = get_first_line_in_buffer ();

  if (first_line > last_line)
    {
      error (0, 0, _("%s: line number out of range"), global_argv[argnum]);
      cleanup_fatal ();
    }

  lines = last_line - first_line;

  for (i = 0; i < lines; i++)
    {
      line = remove_line ();
      if (line == NULL)
        {
          error (0, 0, _("%s: line number out of range"), global_argv[argnum]);
          cleanup_fatal ();
        }
      if (!ignore)
        save_line_to_file (line);
    }
}

