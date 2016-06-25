/* Change blanks to tabs, writing to stdout.
   Read each file in `file_list', in order.  */

static void
unexpand (void)
{
  /* Input stream.  */
  FILE *fp = next_file (NULL);

  /* The array of pending blanks.  In non-POSIX locales, blanks can
     include characters other than spaces, so the blanks must be
     stored, not merely counted.  */
  char *pending_blank;

  if (!fp)
    return;

  /* The worst case is a non-blank character, then one blank, then a
     tab stop, then MAX_COLUMN_WIDTH - 1 blanks, then a non-blank; so
     allocate MAX_COLUMN_WIDTH bytes to store the blanks.  */
  pending_blank = xmalloc (max_column_width);

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

      /* Column the next input tab stop is on.  */
      uintmax_t next_tab_column = 0;

      /* Index in TAB_LIST of next tab stop to examine.  */
      size_t tab_index = 0;

      /* If true, the first pending blank came just before a tab stop.  */
      bool one_blank_before_tab_stop = false;

      /* If true, the previous input character was a blank.  This is
         initially true, since initial strings of blanks are treated
         as if the line was preceded by a blank.  */
      bool prev_blank = true;

      /* Number of pending columns of blanks.  */
      size_t pending = 0;


      /* Convert a line of text.  */

      do
        {
          while ((c = getc (fp)) < 0 && (fp = next_file (fp)))
            continue;

          if (convert)
            {
              bool blank = !! isblank (c);

              if (blank)
                {
                  if (next_tab_column <= column)
                    {
                      if (tab_size)
                        next_tab_column =
                          column + (tab_size - column % tab_size);
                      else
                        while (true)
                          if (tab_index == first_free_tab)
                            {
                              convert = false;
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
                    }

                  if (convert)
                    {
                      if (next_tab_column < column)
                        error (EXIT_FAILURE, 0, _("input line is too long"));

                      if (c == '\t')
                        {
                          column = next_tab_column;

                          /* Discard pending blanks, unless it was a single
                             blank just before the previous tab stop.  */
                          if (! (pending == 1 && one_blank_before_tab_stop))
                            {
                              pending = 0;
                              one_blank_before_tab_stop = false;
                            }
                        }
                      else
                        {
                          column++;

                          if (! (prev_blank && column == next_tab_column))
                            {
                              /* It is not yet known whether the pending blanks
                                 will be replaced by tabs.  */
                              if (column == next_tab_column)
                                one_blank_before_tab_stop = true;
                              pending_blank[pending++] = c;
                              prev_blank = true;
                              continue;
                            }

                          /* Replace the pending blanks by a tab or two.  */
                          pending_blank[0] = c = '\t';
                          pending = one_blank_before_tab_stop;
                        }
                    }
                }
              else if (c == '\b')
                {
                  /* Go back one column, and force recalculation of the
                     next tab stop.  */
                  column -= !!column;
                  next_tab_column = column;
                  tab_index -= !!tab_index;
                }
              else
                {
                  column++;
                  if (!column)
                    error (EXIT_FAILURE, 0, _("input line is too long"));
                }

              if (pending)
                {
                  if (fwrite (pending_blank, 1, pending, stdout) != pending)
                    error (EXIT_FAILURE, errno, _("write error"));
                  pending = 0;
                  one_blank_before_tab_stop = false;
                }

              prev_blank = blank;
              convert &= convert_entire_line || blank;
            }

          if (c < 0)
            {
              free (pending_blank);
              return;
            }

          if (putchar (c) < 0)
            error (EXIT_FAILURE, errno, _("write error"));
        }
      while (c != '\n');
    }
}

