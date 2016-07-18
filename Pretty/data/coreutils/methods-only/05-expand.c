/* Change tabs to spaces, writing to stdout.
   Read each file in `file_list', in order.  */

static void
expand (void)
{
  /* Input stream.  */
  FILE *fp = next_file (NULL);

  if (!fp)
    return;

  while (true)
    {
      /* Input character, or EOF.  */
      int c;

      /* If true, perform translations.  */
      bool convert = true;


      /* The following variables have valid values only when CONVERT
         is true:  */

      /* Column of next input character.  */
      uintmax_t column = 0;

      /* Index in TAB_LIST of next tab stop to examine.  */
      size_t tab_index = 0;


      /* Convert a line of text.  */

      do
        {
          while ((c = getc (fp)) < 0 && (fp = next_file (fp)))
            continue;

          if (convert)
            {
              if (c == '\t')
                {
                  /* Column the next input tab stop is on.  */
                  uintmax_t next_tab_column;

                  if (tab_size)
                    next_tab_column = column + (tab_size - column % tab_size);
                  else
                    while (true)
                      if (tab_index == first_free_tab)
                        {
                          next_tab_column = column + 1;
                          break;
                        }
                      else
                        {
                          uintmax_t tab = tab_list[tab_index++];
                          if (column < tab)
                            {
                              next_tab_column = tab;
                              break;
                            }
                        }

                  if (next_tab_column < column)
                    error (EXIT_FAILURE, 0, _("input line is too long"));

                  while (++column < next_tab_column)
                    if (putchar (' ') < 0)
                      error (EXIT_FAILURE, errno, _("write error"));

                  c = ' ';
                }
              else if (c == '\b')
                {
                  /* Go back one column, and force recalculation of the
                     next tab stop.  */
                  column -= !!column;
                  tab_index -= !!tab_index;
                }
              else
                {
                  column++;
                  if (!column)
                    error (EXIT_FAILURE, 0, _("input line is too long"));
                }

              convert &= convert_entire_line || !! isblank (c);
            }

          if (c < 0)
            return;

          if (putchar (c) < 0)
            error (EXIT_FAILURE, errno, _("write error"));
        }
      while (c != '\n');
    }
}

