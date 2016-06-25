/*-----------------------------------------------------------------------.
| Decides which of two OCCURS, FIRST or SECOND, should lexicographically |
| go first.  In case of a tie, preserve the original order through a	 |
| pointer comparison.							 |
`-----------------------------------------------------------------------*/

static int
compare_occurs (const void *void_first, const void *void_second)
{
  int value;

  value = compare_words (&first->key, &second->key);
  return value == 0 ? first->key.start - second->key.start : value;
}

