/*------------------------------------------------------------------------.
| Scan the whole occurs table and, for each entry, output one line in the |
| appropriate format.							  |
`------------------------------------------------------------------------*/

static void
generate_all_output (void)
{
  size_t occurs_index;		/* index of keyword entry being processed */
  OCCURS *occurs_cursor;	/* current keyword entry being processed */

  /* The following assignments are useful to provide default values in case
     line contexts or references are not used, in which case these variables
     would never be computed.  */

  tail.start = NULL;
  tail.end = NULL;
  tail_truncation = 0;

  head.start = NULL;
  head.end = NULL;
  head_truncation = 0;

  /* Loop over all keyword occurrences.  */

  occurs_cursor = occurs_table[0];

  for (occurs_index = 0; occurs_index < number_of_occurs[0]; occurs_index++)
    {
      /* Compute the exact size of every field and whenever truncation flags
         are present or not.  */

      define_all_fields (occurs_cursor);

      /* Produce one output line according to selected format.  */

      switch (output_format)
        {
        case UNKNOWN_FORMAT:
          /* Should never happen.  */

        case DUMB_FORMAT:
          output_one_dumb_line ();
          break;

        case ROFF_FORMAT:
          output_one_roff_line ();
          break;

        case TEX_FORMAT:
          output_one_tex_line ();
          break;
        }

      /* Advance the cursor into the occurs table.  */

      occurs_cursor++;
    }
}

