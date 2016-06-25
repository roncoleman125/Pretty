/*------------------------------------------------------------.
| Return !0 if WORD appears in TABLE.  Uses a binary search.  |
`------------------------------------------------------------*/

static int
search_table (WORD *word, WORD_TABLE *table)
{
  int lowest;			/* current lowest possible index */
  int highest;			/* current highest possible index */
  int middle;			/* current middle index */
  int value;			/* value from last comparison */

  lowest = 0;
  highest = table->length - 1;
  while (lowest <= highest)
    {
      middle = (lowest + highest) / 2;
      value = compare_words (word, table->start + middle);
      if (value < 0)
        highest = middle - 1;
      else if (value > 0)
        lowest = middle + 1;
      else
        return 1;
    }
  return 0;
}

