/* Return the component of the cost of breaking before word NEXT that
   depends on LEN, the length of the line beginning there.  */

static COST
line_cost (WORD *next, int len)
{
  int n;
  COST cost;

  if (next == word_limit)
    return 0;
  n = best_width - len;
  cost = SHORT_COST (n);
  if (next->next_break != word_limit)
    {
      n = len - next->line_length;
      cost += RAGGED_COST (n);
    }
  return cost;
}

