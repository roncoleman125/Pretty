/* Read from stream STREAM, printing to standard output any selected fields.  */

static void
cut_fields (FILE *stream)
{
  int c;
  size_t field_idx = 1;
  bool found_any_selected_field = false;
  bool buffer_first_field;

  c = getc (stream);
  if (c == EOF)
    return;

  ungetc (c, stream);

  /* To support the semantics of the -s flag, we may have to buffer
     all of the first field to determine whether it is `delimited.'
     But that is unnecessary if all non-delimited lines must be printed
     and the first field has been selected, or if non-delimited lines
     must be suppressed and the first field has *not* been selected.
     That is because a non-delimited line has exactly one field.  */
  buffer_first_field = (suppress_non_delimited ^ !print_kth (1, NULL));

  while (1)
    {
      if (field_idx == 1 && buffer_first_field)
        {
          ssize_t len;
          size_t n_bytes;

          len = getndelim2 (&field_1_buffer, &field_1_bufsize, 0,
                            GETNLINE_NO_LIMIT, delim, '\n', stream);
          if (len < 0)
            {
              free (field_1_buffer);
              field_1_buffer = NULL;
              if (ferror (stream) || feof (stream))
                break;
              xalloc_die ();
            }

          n_bytes = len;
          assert (n_bytes != 0);

          /* If the first field extends to the end of line (it is not
             delimited) and we are printing all non-delimited lines,
             print this one.  */
          if (to_uchar (field_1_buffer[n_bytes - 1]) != delim)
            {
              if (suppress_non_delimited)
                {
                  /* Empty.  */
                }
              else
                {
                  fwrite (field_1_buffer, sizeof (char), n_bytes, stdout);
                  /* Make sure the output line is newline terminated.  */
                  if (field_1_buffer[n_bytes - 1] != '\n')
                    putchar ('\n');
                }
              continue;
            }
          if (print_kth (1, NULL))
            {
              /* Print the field, but not the trailing delimiter.  */
              fwrite (field_1_buffer, sizeof (char), n_bytes - 1, stdout);
              found_any_selected_field = true;
            }
          ++field_idx;
        }

      if (c != EOF)
        {
          if (print_kth (field_idx, NULL))
            {
              if (found_any_selected_field)
                {
                  fwrite (output_delimiter_string, sizeof (char),
                          output_delimiter_length, stdout);
                }
              found_any_selected_field = true;

              while ((c = getc (stream)) != delim && c != '\n' && c != EOF)
                {
                  putchar (c);
                }
            }
          else
            {
              while ((c = getc (stream)) != delim && c != '\n' && c != EOF)
                {
                  /* Empty.  */
                }
            }
        }

      if (c == '\n')
        {
          c = getc (stream);
          if (c != EOF)
            {
              ungetc (c, stream);
              c = '\n';
            }
        }

      if (c == delim)
        ++field_idx;
      else if (c == '\n' || c == EOF)
        {
          if (found_any_selected_field
              || !(suppress_non_delimited && field_idx == 1))
            putchar ('\n');
          if (c == EOF)
            break;
          field_idx = 1;
          found_any_selected_field = false;
        }
    }
}

