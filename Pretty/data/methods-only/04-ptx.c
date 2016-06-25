/* Sort and search routines.  */

/*--------------------------------------------------------------------------.
| Compare two words, FIRST and SECOND, and return 0 if they are identical.  |
| Return less than 0 if the first word goes before the second; return	    |
| greater than 0 if the first word goes after the second.		    |
|									    |
| If a word is indeed a prefix of the other, the shorter should go first.   |
`--------------------------------------------------------------------------*/

static int
compare_words (const void *void_first, const void *void_second)
{
  int length;			/* minimum of two lengths */
  int counter;			/* cursor in words */
  int value;			/* value of comparison */

  length = first->size < second->size ? first->size : second->size;

  if (ignore_case)
    {
      for (counter = 0; counter < length; counter++)
        {
          value = (folded_chars [to_uchar (first->start[counter])]
                   - folded_chars [to_uchar (second->start[counter])]);
          if (value != 0)
            return value;
        }
    }
  else
    {
      for (counter = 0; counter < length; counter++)
        {
          value = (to_uchar (first->start[counter])
                   - to_uchar (second->start[counter]));
          if (value != 0)
            return value;
        }
    }

  return first->size - second->size;
}

