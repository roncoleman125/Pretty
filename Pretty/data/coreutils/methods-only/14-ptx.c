/*------------------------------------------------------------------.
| Compute the position and length of all the output fields, given a |
| pointer to some OCCURS.					    |
`------------------------------------------------------------------*/

static void
define_all_fields (OCCURS *occurs)
{
  int tail_max_width;		/* allowable width of tail field */
  int head_max_width;		/* allowable width of head field */
  char *cursor;			/* running cursor in source text */
  char *left_context_start;	/* start of left context */
  char *right_context_end;	/* end of right context */
  char *left_field_start;	/* conservative start for `head'/`before' */
  int file_index;		/* index in text input file arrays */
  const char *file_name;	/* file name for reference */
  int line_ordinal;		/* line ordinal for reference */

  /* Define `keyafter', start of left context and end of right context.
     `keyafter' starts at the saved position for keyword and extend to the
     right from the end of the keyword, eating separators or full words, but
     not beyond maximum allowed width for `keyafter' field or limit for the
     right context.  Suffix spaces will be removed afterwards.  */

  keyafter.start = occurs->key.start;
  keyafter.end = keyafter.start + occurs->key.size;
  left_context_start = keyafter.start + occurs->left;
  right_context_end = keyafter.start + occurs->right;

  cursor = keyafter.end;
  while (cursor < right_context_end
         && cursor <= keyafter.start + keyafter_max_width)
    {
      keyafter.end = cursor;
      SKIP_SOMETHING (cursor, right_context_end);
    }
  if (cursor <= keyafter.start + keyafter_max_width)
    keyafter.end = cursor;

  keyafter_truncation = truncation_string && keyafter.end < right_context_end;

  SKIP_WHITE_BACKWARDS (keyafter.end, keyafter.start);

  /* When the left context is wide, it might take some time to catch up from
     the left context boundary to the beginning of the `head' or `before'
     fields.  So, in this case, to speed the catchup, we jump back from the
     keyword, using some secure distance, possibly falling in the middle of
     a word.  A secure backward jump would be at least half the maximum
     width of a line, plus the size of the longest word met in the whole
     input.  We conclude this backward jump by a skip forward of at least
     one word.  In this manner, we should not inadvertently accept only part
     of a word.  From the reached point, when it will be time to fix the
     beginning of `head' or `before' fields, we will skip forward words or
     delimiters until we get sufficiently near.  */

  if (-occurs->left > half_line_width + maximum_word_length)
    {
      left_field_start
        = keyafter.start - (half_line_width + maximum_word_length);
      SKIP_SOMETHING (left_field_start, keyafter.start);
    }
  else
    left_field_start = keyafter.start + occurs->left;

  /* `before' certainly ends at the keyword, but not including separating
     spaces.  It starts after than the saved value for the left context, by
     advancing it until it falls inside the maximum allowed width for the
     before field.  There will be no prefix spaces either.  `before' only
     advances by skipping single separators or whole words. */

  before.start = left_field_start;
  before.end = keyafter.start;
  SKIP_WHITE_BACKWARDS (before.end, before.start);

  while (before.start + before_max_width < before.end)
    SKIP_SOMETHING (before.start, before.end);

  if (truncation_string)
    {
      cursor = before.start;
      SKIP_WHITE_BACKWARDS (cursor, text_buffer.start);
      before_truncation = cursor > left_context_start;
    }
  else
    before_truncation = 0;

  SKIP_WHITE (before.start, text_buffer.end);

  /* The tail could not take more columns than what has been left in the
     left context field, and a gap is mandatory.  It starts after the
     right context, and does not contain prefixed spaces.  It ends at
     the end of line, the end of buffer or when the tail field is full,
     whichever comes first.  It cannot contain only part of a word, and
     has no suffixed spaces.  */

  tail_max_width
    = before_max_width - (before.end - before.start) - gap_size;

  if (tail_max_width > 0)
    {
      tail.start = keyafter.end;
      SKIP_WHITE (tail.start, text_buffer.end);

      tail.end = tail.start;
      cursor = tail.end;
      while (cursor < right_context_end
             && cursor < tail.start + tail_max_width)
        {
          tail.end = cursor;
          SKIP_SOMETHING (cursor, right_context_end);
        }

      if (cursor < tail.start + tail_max_width)
        tail.end = cursor;

      if (tail.end > tail.start)
        {
          keyafter_truncation = 0;
          tail_truncation = truncation_string && tail.end < right_context_end;
        }
      else
        tail_truncation = 0;

      SKIP_WHITE_BACKWARDS (tail.end, tail.start);
    }
  else
    {

      /* No place left for a tail field.  */

      tail.start = NULL;
      tail.end = NULL;
      tail_truncation = 0;
    }

  /* `head' could not take more columns than what has been left in the right
     context field, and a gap is mandatory.  It ends before the left
     context, and does not contain suffixed spaces.  Its pointer is advanced
     until the head field has shrunk to its allowed width.  It cannot
     contain only part of a word, and has no suffixed spaces.  */

  head_max_width
    = keyafter_max_width - (keyafter.end - keyafter.start) - gap_size;

  if (head_max_width > 0)
    {
      head.end = before.start;
      SKIP_WHITE_BACKWARDS (head.end, text_buffer.start);

      head.start = left_field_start;
      while (head.start + head_max_width < head.end)
        SKIP_SOMETHING (head.start, head.end);

      if (head.end > head.start)
        {
          before_truncation = 0;
          head_truncation = (truncation_string
                             && head.start > left_context_start);
        }
      else
        head_truncation = 0;

      SKIP_WHITE (head.start, head.end);
    }
  else
    {

      /* No place left for a head field.  */

      head.start = NULL;
      head.end = NULL;
      head_truncation = 0;
    }

  if (auto_reference)
    {

      /* Construct the reference text in preallocated space from the file
         name and the line number.  Find out in which file the reference
         occurred.  Standard input yields an empty file name.  Insure line
         numbers are one based, even if they are computed zero based.  */

      file_index = 0;
      while (file_line_count[file_index] < occurs->reference)
        file_index++;

      file_name = input_file_name[file_index];
      if (!file_name)
        file_name = "";

      line_ordinal = occurs->reference + 1;
      if (file_index > 0)
        line_ordinal -= file_line_count[file_index - 1];

      sprintf (reference.start, "%s:%d", file_name, line_ordinal);
      reference.end = reference.start + strlen (reference.start);
    }
  else if (input_reference)
    {

      /* Reference starts at saved position for reference and extends right
         until some white space is met.  */

      reference.start = keyafter.start + (DELTA) occurs->reference;
      reference.end = reference.start;
      SKIP_NON_WHITE (reference.end, right_context_end);
    }
}

