/*-----------------------------------------------------------------------.
| Read a file named FILE_NAME, containing one word per line, then	 |
| construct in TABLE a table of WORD descriptors for them.  The routine	 |
| swallows the whole file in memory; this is at the expense of space	 |
| needed for newlines, which are useless; however, the reading is fast.	 |
`-----------------------------------------------------------------------*/

static void
digest_word_file (const char *file_name, WORD_TABLE *table)
{
  BLOCK file_contents;		/* to receive a copy of the file */
  char *cursor;			/* cursor in file copy */
  char *word_start;		/* start of the current word */

  swallow_file_in_memory (file_name, &file_contents);

  table->start = NULL;
  table->alloc = 0;
  table->length = 0;

  /* Read the whole file.  */

  cursor = file_contents.start;
  while (cursor < file_contents.end)
    {

      /* Read one line, and save the word in contains.  */

      word_start = cursor;
      while (cursor < file_contents.end && *cursor != '\n')
        cursor++;

      /* Record the word in table if it is not empty.  */

      if (cursor > word_start)
        {
          if (table->length == table->alloc)
            {
              if ((SIZE_MAX / sizeof *table->start - 1) / 2 < table->alloc)
                xalloc_die ();
              table->alloc = table->alloc * 2 + 1;
              table->start = xrealloc (table->start,
                                       table->alloc * sizeof *table->start);
            }

          table->start[table->length].start = word_start;
          table->start[table->length].size = cursor - word_start;
          table->length++;
        }

      /* This test allows for an incomplete line at end of file.  */

      if (cursor < file_contents.end)
        cursor++;
    }

  /* Finally, sort all the words read.  */

  qsort (table->start, table->length, sizeof table->start[0], compare_words);
}

