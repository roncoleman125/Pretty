/* Read the input until a line matches the regexp in P, outputting
   it unless P->IGNORE is true.
   REPETITION is this repeat-count; 0 means the first time. */

static void
process_regexp (struct control *p, uintmax_t repetition)
{
  struct cstring *line;		/* From input file. */
  size_t line_len;		/* To make "$" in regexps work. */
  uintmax_t break_line;		/* First line number of next file. */
  bool ignore = p->ignore;	/* If true, skip this section. */
  regoff_t ret;

  if (!ignore)
    create_output_file ();

  /* If there is no offset for the regular expression, or
     it is positive, then it is not necessary to buffer the lines. */

  if (p->offset >= 0)
    {
      while (true)
        {
          line = find_line (++current_line);
          if (line == NULL)
            {
              if (p->repeat_forever)
                {
                  if (!ignore)
                    {
                      dump_rest_of_file ();
                      close_output_file ();
                    }
                  exit (EXIT_SUCCESS);
                }
              else
                regexp_error (p, repetition, ignore);
            }
          line_len = line->len;
          if (line->str[line_len - 1] == '\n')
            line_len--;
          ret = re_search (&p->re_compiled, line->str, line_len,
                           0, line_len, NULL);
          if (ret == -2)
            {
              error (0, 0, _("error in regular expression search"));
              cleanup_fatal ();
            }
          if (ret == -1)
            {
              line = remove_line ();
              if (!ignore)
                save_line_to_file (line);
            }
          else
            break;
        }
    }
  else
    {
      /* Buffer the lines. */
      while (true)
        {
          line = find_line (++current_line);
          if (line == NULL)
            {
              if (p->repeat_forever)
                {
                  if (!ignore)
                    {
                      dump_rest_of_file ();
                      close_output_file ();
                    }
                  exit (EXIT_SUCCESS);
                }
              else
                regexp_error (p, repetition, ignore);
            }
          line_len = line->len;
          if (line->str[line_len - 1] == '\n')
            line_len--;
          ret = re_search (&p->re_compiled, line->str, line_len,
                           0, line_len, NULL);
          if (ret == -2)
            {
              error (0, 0, _("error in regular expression search"));
              cleanup_fatal ();
            }
          if (ret != -1)
            break;
        }
    }

  /* Account for any offset from this regexp. */
  break_line = current_line + p->offset;

  write_to_file (break_line, ignore, p->argnum);

  if (!ignore)
    close_output_file ();

  if (p->offset > 0)
    current_line = break_line;
}

