/* Formatting and actual output - planning routines.  */

/*--------------------------------------------------------------------.
| From information collected from command line options and input file |
| readings, compute and fix some output parameter values.	      |
`--------------------------------------------------------------------*/

static void
fix_output_parameters (void)
{
  int file_index;		/* index in text input file arrays */
  int line_ordinal;		/* line ordinal value for reference */
  char ordinal_string[12];	/* edited line ordinal for reference */
  int reference_width;		/* width for the whole reference */
  int character;		/* character ordinal */
  const char *cursor;		/* cursor in some constant strings */

  /* In auto reference mode, the maximum width of this field is
     precomputed and subtracted from the overall line width.  Add one for
     the column which separate the file name from the line number.  */

  if (auto_reference)
    {
      reference_max_width = 0;
      for (file_index = 0; file_index < number_input_files; file_index++)
        {
          line_ordinal = file_line_count[file_index] + 1;
          if (file_index > 0)
            line_ordinal -= file_line_count[file_index - 1];
          sprintf (ordinal_string, "%d", line_ordinal);
          reference_width = strlen (ordinal_string);
          if (input_file_name[file_index])
            reference_width += strlen (input_file_name[file_index]);
          if (reference_width > reference_max_width)
            reference_max_width = reference_width;
        }
      reference_max_width++;
      reference.start = xmalloc ((size_t) reference_max_width + 1);
    }

  /* If the reference appears to the left of the output line, reserve some
     space for it right away, including one gap size.  */

  if ((auto_reference || input_reference) && !right_reference)
    line_width -= reference_max_width + gap_size;

  /* The output lines, minimally, will contain from left to right a left
     context, a gap, and a keyword followed by the right context with no
     special intervening gap.  Half of the line width is dedicated to the
     left context and the gap, the other half is dedicated to the keyword
     and the right context; these values are computed once and for all here.
     There also are tail and head wrap around fields, used when the keyword
     is near the beginning or the end of the line, or when some long word
     cannot fit in, but leave place from wrapped around shorter words.  The
     maximum width of these fields are recomputed separately for each line,
     on a case by case basis.  It is worth noting that it cannot happen that
     both the tail and head fields are used at once.  */

  half_line_width = line_width / 2;
  before_max_width = half_line_width - gap_size;
  keyafter_max_width = half_line_width;

  /* If truncation_string is the empty string, make it NULL to speed up
     tests.  In this case, truncation_string_length will never get used, so
     there is no need to set it.  */

  if (truncation_string && *truncation_string)
    truncation_string_length = strlen (truncation_string);
  else
    truncation_string = NULL;

  if (gnu_extensions)
    {

      /* When flagging truncation at the left of the keyword, the
         truncation mark goes at the beginning of the before field,
         unless there is a head field, in which case the mark goes at the
         left of the head field.  When flagging truncation at the right
         of the keyword, the mark goes at the end of the keyafter field,
         unless there is a tail field, in which case the mark goes at the
         end of the tail field.  Only eight combination cases could arise
         for truncation marks:

         . None.
         . One beginning the before field.
         . One beginning the head field.
         . One ending the keyafter field.
         . One ending the tail field.
         . One beginning the before field, another ending the keyafter field.
         . One ending the tail field, another beginning the before field.
         . One ending the keyafter field, another beginning the head field.

         So, there is at most two truncation marks, which could appear both
         on the left side of the center of the output line, both on the
         right side, or one on either side.  */

      before_max_width -= 2 * truncation_string_length;
      if (before_max_width < 0)
        before_max_width = 0;
      keyafter_max_width -= 2 * truncation_string_length;
    }
  else
    {

      /* I never figured out exactly how UNIX' ptx plans the output width
         of its various fields.  If GNU extensions are disabled, do not
         try computing the field widths correctly; instead, use the
         following formula, which does not completely imitate UNIX' ptx,
         but almost.  */

      keyafter_max_width -= 2 * truncation_string_length + 1;
    }

  /* Compute which characters need special output processing.  Initialize
     by flagging any white space character.  Some systems do not consider
     form feed as a space character, but we do.  */

  for (character = 0; character < CHAR_SET_SIZE; character++)
    edited_flag[character] = !! isspace (character);
  edited_flag['\f'] = 1;

  /* Complete the special character flagging according to selected output
     format.  */

  switch (output_format)
    {
    case UNKNOWN_FORMAT:
      /* Should never happen.  */

    case DUMB_FORMAT:
      break;

    case ROFF_FORMAT:

      /* `Quote' characters should be doubled.  */

      edited_flag['"'] = 1;
      break;

    case TEX_FORMAT:

      /* Various characters need special processing.  */

      for (cursor = "$%&#_{}\\"; *cursor; cursor++)
        edited_flag[to_uchar (*cursor)] = 1;

      /* Any character with 8th bit set will print to a single space, unless
         it is diacriticized.  */

      for (character = 0200; character < CHAR_SET_SIZE; character++)
        edited_flag[character] = todiac (character) != 0;
      break;
    }
}

