/* Output any lines left after all regexps have been processed. */

static void
dump_rest_of_file (void)
{
  struct cstring *line;

  while ((line = remove_line ()) != NULL)
    save_line_to_file (line);
}

/* Handle an attempt to read beyond EOF under the control of record P,
   on iteration REPETITION if nonzero. */

static void handle_line_error (const struct control *, uintmax_t)
     ATTRIBUTE_NORETURN;
