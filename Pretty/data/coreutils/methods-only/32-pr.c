/* Print a line from buff.

   If this function has been called, we know we have "something to
   print". But it remains to be seen whether we have a real text page
   or an empty page (a single form feed) with/without a header only.
   Therefore first we set pad_vertically to true and print a header
   if necessary.
   If FF_FOUND and we are using -t|-T option we omit any newline by
   setting pad_vertically to false (see print_page).
   Otherwise we pad across if necessary, print separators if necessary
   and text of COLUMN *p.

   Return true, meaning there is no need to call read_rest_of_line. */

static bool
print_stored (COLUMN *p)
{
  COLUMN *q;
  int i;

  int line = p->current_line++;
  char *first = &buff[line_vector[line]];
  /* FIXME
     UMR: Uninitialized memory read:
     * This is occurring while in:
     print_stored   [pr.c:2239]
     * Reading 4 bytes from 0x5148c in the heap.
     * Address 0x5148c is 4 bytes into a malloc'd block at 0x51488 of 676 bytes
     * This block was allocated from:
     malloc         [rtlib.o]
     xmalloc        [xmalloc.c:94]
     init_store_cols [pr.c:1648]
     */
  char *last = &buff[line_vector[line + 1]];

  pad_vertically = true;

  if (print_a_header)
    print_header ();

  if (p->status == FF_FOUND)
    {
      for (i = 1, q = column_vector; i <= columns; ++i, ++q)
        q->status = ON_HOLD;
      if (column_vector->lines_to_print <= 0)
        {
          if (!extremities)
            pad_vertically = false;
          return true;		/* print a header only */
        }
    }

  if (padding_not_printed - col_sep_length > 0)
    {
      pad_across_to (padding_not_printed - col_sep_length);
      padding_not_printed = ANYWHERE;
    }

  if (use_col_separator)
    print_sep_string ();

  while (first != last)
    print_char (*first++);

  if (spaces_not_printed == 0)
    {
      output_position = p->start_position + end_vector[line];
      if (p->start_position - col_sep_length == chars_per_margin)
        output_position -= col_sep_length;
    }

  return true;
}

/* Convert a character to the proper format and return the number of
   characters in the resulting clump.  Increment input_position by
   the width of the clump.

   Tabs are converted to clumps of spaces.
   Nonprintable characters may be converted to clumps of escape
   sequences or control prefixes.

   Note: the width of a clump is not necessarily equal to the number of
   characters in clump_buff.  (e.g, the width of '\b' is -1, while the
   number of characters is 1.) */

static int
char_to_clump (char c)
{
  unsigned char uc = c;
  char *s = clump_buff;
  int i;
  char esc_buff[4];
  int width;
  int chars;
  int chars_per_c = 8;

  if (c == input_tab_char)
    chars_per_c = chars_per_input_tab;

  if (c == input_tab_char || c == '\t')
    {
      width = TAB_WIDTH (chars_per_c, input_position);

      if (untabify_input)
        {
          for (i = width; i; --i)
            *s++ = ' ';
          chars = width;
        }
      else
        {
          *s = c;
          chars = 1;
        }

    }
  else if (! isprint (uc))
    {
      if (use_esc_sequence)
        {
          width = 4;
          chars = 4;
          *s++ = '\\';
          sprintf (esc_buff, "%03o", uc);
          for (i = 0; i <= 2; ++i)
            *s++ = esc_buff[i];
        }
      else if (use_cntrl_prefix)
        {
          if (uc < 0200)
            {
              width = 2;
              chars = 2;
              *s++ = '^';
              *s = c ^ 0100;
            }
          else
            {
              width = 4;
              chars = 4;
              *s++ = '\\';
              sprintf (esc_buff, "%03o", uc);
              for (i = 0; i <= 2; ++i)
                *s++ = esc_buff[i];
            }
        }
      else if (c == '\b')
        {
          width = -1;
          chars = 1;
          *s = c;
        }
      else
        {
          width = 0;
          chars = 1;
          *s = c;
        }
    }
  else
    {
      width = 1;
      chars = 1;
      *s = c;
    }

  /* Too many backspaces must put us in position 0 -- never negative.  */
  if (width < 0 && input_position == 0)
    {
      chars = 0;
      input_position = 0;
    }
  else if (width < 0 && input_position <= -width)
    input_position = 0;
  else
    input_position += width;

  return chars;
}

