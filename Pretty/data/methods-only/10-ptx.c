/* Keyword recognition and selection.  */

/*----------------------------------------------------------------------.
| For each keyword in the source text, constructs an OCCURS structure.  |
`----------------------------------------------------------------------*/

static void
find_occurs_in_text (void)
{
  char *cursor;			/* for scanning the source text */
  char *scan;			/* for scanning the source text also */
  char *line_start;		/* start of the current input line */
  char *line_scan;		/* newlines scanned until this point */
  int reference_length;		/* length of reference in input mode */
  WORD possible_key;		/* possible key, to ease searches */
  OCCURS *occurs_cursor;	/* current OCCURS under construction */

  char *context_start;		/* start of left context */
  char *context_end;		/* end of right context */
  char *word_start;		/* start of word */
  char *word_end;		/* end of word */
  char *next_context_start;	/* next start of left context */

  /* reference_length is always used within `if (input_reference)'.
     However, GNU C diagnoses that it may be used uninitialized.  The
     following assignment is merely to shut it up.  */

  reference_length = 0;

  /* Tracking where lines start is helpful for reference processing.  In
     auto reference mode, this allows counting lines.  In input reference
     mode, this permits finding the beginning of the references.

     The first line begins with the file, skip immediately this very first
     reference in input reference mode, to help further rejection any word
     found inside it.  Also, unconditionally assigning these variable has
     the happy effect of shutting up lint.  */

  line_start = text_buffer.start;
  line_scan = line_start;
  if (input_reference)
    {
      SKIP_NON_WHITE (line_scan, text_buffer.end);
      reference_length = line_scan - line_start;
      SKIP_WHITE (line_scan, text_buffer.end);
    }

  /* Process the whole buffer, one line or one sentence at a time.  */

  for (cursor = text_buffer.start;
       cursor < text_buffer.end;
       cursor = next_context_start)
    {

      /* `context_start' gets initialized before the processing of each
         line, or once for the whole buffer if no end of line or sentence
         sequence separator.  */

      context_start = cursor;

      /* If a end of line or end of sentence sequence is defined and
         non-empty, `next_context_start' will be recomputed to be the end of
         each line or sentence, before each one is processed.  If no such
         sequence, then `next_context_start' is set at the end of the whole
         buffer, which is then considered to be a single line or sentence.
         This test also accounts for the case of an incomplete line or
         sentence at the end of the buffer.  */

      next_context_start = text_buffer.end;
      if (context_regex.string)
        switch (re_search (&context_regex.pattern, cursor,
                           text_buffer.end - cursor,
                           0, text_buffer.end - cursor, &context_regs))
          {
          case -2:
            matcher_error ();

          case -1:
            break;

          default:
            next_context_start = cursor + context_regs.end[0];
            break;
          }

      /* Include the separator into the right context, but not any suffix
         white space in this separator; this insures it will be seen in
         output and will not take more space than necessary.  */

      context_end = next_context_start;
      SKIP_WHITE_BACKWARDS (context_end, context_start);

      /* Read and process a single input line or sentence, one word at a
         time.  */

      while (1)
        {
          if (word_regex.string)

            /* If a word regexp has been compiled, use it to skip at the
               beginning of the next word.  If there is no such word, exit
               the loop.  */

            {
              regoff_t r = re_search (&word_regex.pattern, cursor,
                                      context_end - cursor,
                                      0, context_end - cursor, &word_regs);
              if (r == -2)
                matcher_error ();
              if (r == -1)
                break;
              word_start = cursor + word_regs.start[0];
              word_end = cursor + word_regs.end[0];
            }
          else

            /* Avoid re_search and use the fastmap to skip to the
               beginning of the next word.  If there is no more word in
               the buffer, exit the loop.  */

            {
              scan = cursor;
              while (scan < context_end
                     && !word_fastmap[to_uchar (*scan)])
                scan++;

              if (scan == context_end)
                break;

              word_start = scan;

              while (scan < context_end
                     && word_fastmap[to_uchar (*scan)])
                scan++;

              word_end = scan;
            }

          /* Skip right to the beginning of the found word.  */

          cursor = word_start;

          /* Skip any zero length word.  Just advance a single position,
             then go fetch the next word.  */

          if (word_end == word_start)
            {
              cursor++;
              continue;
            }

          /* This is a genuine, non empty word, so save it as a possible
             key.  Then skip over it.  Also, maintain the maximum length of
             all words read so far.  It is mandatory to take the maximum
             length of all words in the file, without considering if they
             are actually kept or rejected, because backward jumps at output
             generation time may fall in *any* word.  */

          possible_key.start = cursor;
          possible_key.size = word_end - word_start;
          cursor += possible_key.size;

          if (possible_key.size > maximum_word_length)
            maximum_word_length = possible_key.size;

          /* In input reference mode, update `line_start' from its previous
             value.  Count the lines just in case auto reference mode is
             also selected. If it happens that the word just matched is
             indeed part of a reference; just ignore it.  */

          if (input_reference)
            {
              while (line_scan < possible_key.start)
                if (*line_scan == '\n')
                  {
                    total_line_count++;
                    line_scan++;
                    line_start = line_scan;
                    SKIP_NON_WHITE (line_scan, text_buffer.end);
                    reference_length = line_scan - line_start;
                  }
                else
                  line_scan++;
              if (line_scan > possible_key.start)
                continue;
            }

          /* Ignore the word if an `Ignore words' table exists and if it is
             part of it.  Also ignore the word if an `Only words' table and
             if it is *not* part of it.

             It is allowed that both tables be used at once, even if this
             may look strange for now.  Just ignore a word that would appear
             in both.  If regexps are eventually implemented for these
             tables, the Ignore table could then reject words that would
             have been previously accepted by the Only table.  */

          if (ignore_file && search_table (&possible_key, &ignore_table))
            continue;
          if (only_file && !search_table (&possible_key, &only_table))
            continue;

          /* A non-empty word has been found.  First of all, insure
             proper allocation of the next OCCURS, and make a pointer to
             where it will be constructed.  */

          if (number_of_occurs[0] == occurs_alloc[0])
            {
              if ((SIZE_MAX / sizeof *occurs_table[0] - 1) / 2
                  < occurs_alloc[0])
                xalloc_die ();
              occurs_alloc[0] = occurs_alloc[0] * 2 + 1;
              occurs_table[0] =
                xrealloc (occurs_table[0],
                          occurs_alloc[0] * sizeof *occurs_table[0]);
            }

          occurs_cursor = occurs_table[0] + number_of_occurs[0];

          /* Define the refence field, if any.  */

          if (auto_reference)
            {

              /* While auto referencing, update `line_start' from its
                 previous value, counting lines as we go.  If input
                 referencing at the same time, `line_start' has been
                 advanced earlier, and the following loop is never really
                 executed.  */

              while (line_scan < possible_key.start)
                if (*line_scan == '\n')
                  {
                    total_line_count++;
                    line_scan++;
                    line_start = line_scan;
                    SKIP_NON_WHITE (line_scan, text_buffer.end);
                  }
                else
                  line_scan++;

              occurs_cursor->reference = total_line_count;
            }
          else if (input_reference)
            {

              /* If only input referencing, `line_start' has been computed
                 earlier to detect the case the word matched would be part
                 of the reference.  The reference position is simply the
                 value of `line_start'.  */

              occurs_cursor->reference
                = (DELTA) (line_start - possible_key.start);
              if (reference_length > reference_max_width)
                reference_max_width = reference_length;
            }

          /* Exclude the reference from the context in simple cases.  */

          if (input_reference && line_start == context_start)
            {
              SKIP_NON_WHITE (context_start, context_end);
              SKIP_WHITE (context_start, context_end);
            }

          /* Completes the OCCURS structure.  */

          occurs_cursor->key = possible_key;
          occurs_cursor->left = context_start - possible_key.start;
          occurs_cursor->right = context_end - possible_key.start;

          number_of_occurs[0]++;
        }
    }
}

