/* Determine the line number that marks the end of this file,
   then get those lines and save them to the output file.
   P is the control record.
   REPETITION is the repetition number. */

static void
process_line_count (const struct control *p, uintmax_t repetition)
{
  uintmax_t linenum;
  uintmax_t last_line_to_save = p->lines_required * (repetition + 1);
  struct cstring *line;

  create_output_file ();

  linenum = get_first_line_in_buffer ();

  while (linenum++ < last_line_to_save)
    {
      line = remove_line ();
      if (line == NULL)
        handle_line_error (p, repetition);
      save_line_to_file (line);
    }

  close_output_file ();

  /* Ensure that the line number specified is not 1 greater than
     the number of lines in the file. */
  if (no_more_lines ())
    handle_line_error (p, repetition);
}

static void regexp_error (struct control *, uintmax_t, bool) ATTRIBUTE_NORETURN;
