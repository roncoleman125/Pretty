/* Copy INPUT_FD to a temporary, then tac that file.
   Return true if successful.  */

static bool
tac_nonseekable (int input_fd, const char *file)
{
  FILE *tmp_stream;
  char *tmp_file;
  return (copy_to_temp (&tmp_stream, &tmp_file, input_fd, file)
          && tac_seekable (fileno (tmp_stream), tmp_file));
}

