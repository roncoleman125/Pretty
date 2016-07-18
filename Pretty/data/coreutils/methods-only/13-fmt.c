/* Compute the optimal formatting for the whole paragraph by computing
   and remembering the optimal formatting for each suffix from the empty
   one to the whole paragraph.  */

static void
fmt_paragraph (void)
{
  WORD *start, *w;
  int len;
  COST wcost, best;
  int saved_length;

  word_limit->best_cost = 0;
  saved_length = word_limit->length;
  word_limit->length = max_width;	/* sentinel */

  for (start = word_limit - 1; start >= word; start--)
    {
      best = MAXCOST;
      len = start == word ? first_indent : other_indent;

      /* At least one word, however long, in the line.  */

      w = start;
      len += w->length;
      do
        {
          w++;

          /* Consider breaking before w.  */

          wcost = line_cost (w, len) + w->best_cost;
          if (start == word && last_line_length > 0)
            wcost += RAGGED_COST (len - last_line_length);
          if (wcost < best)
            {
              best = wcost;
              start->next_break = w;
              start->line_length = len;
            }

          /* This is a kludge to keep us from computing `len' as the
             sum of the sentinel length and some non-zero number.
             Since the sentinel w->length may be INT_MAX, adding
             to that would give a negative result.  */
          if (w == word_limit)
            break;

          len += (w - 1)->space + w->length;	/* w > start >= word */
        }
      while (len < max_width);
      start->best_cost = best + base_cost (start);
    }

  word_limit->length = saved_length;
}

