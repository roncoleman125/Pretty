/* Given a list of one or more input filenames FILE_LIST, set the global
   file pointer IN_STREAM and the global string INPUT_FILENAME to the
   first one that can be successfully opened. Modify FILE_LIST to
   reference the next filename in the list.  A file name of "-" is
   interpreted as standard input.  If any file open fails, give an error
   message and return false.  */

static bool
open_next_file (void)
{
  bool ok = true;

  do
    {
      input_filename = *file_list;
      if (input_filename == NULL)
        return ok;
      ++file_list;

      if (STREQ (input_filename, "-"))
        {
          input_filename = _("standard input");
          in_stream = stdin;
          have_read_stdin = true;
          if (O_BINARY && ! isatty (STDIN_FILENO))
            xfreopen (NULL, "rb", stdin);
        }
      else
        {
          in_stream = fopen (input_filename, (O_BINARY ? "rb" : "r"));
          if (in_stream == NULL)
            {
              error (0, errno, "%s", input_filename);
              ok = false;
            }
        }
    }
  while (in_stream == NULL);

  if (limit_bytes_to_format && !flag_dump_strings)
    setvbuf (in_stream, NULL, _IONBF, 0);

  return ok;
}

