/* Return the constant component of the cost of breaking before the
   word THIS.  */

static COST
base_cost (WORD *this)
{
  COST cost;

  cost = LINE_COST;

  if (this > word)
    {
      if ((this - 1)->period)
        {
          if ((this - 1)->final)
            cost -= SENTENCE_BONUS;
          else
            cost += NOBREAK_COST;
        }
      else if ((this - 1)->punct)
        cost -= PUNCT_BONUS;
      else if (this > word + 1 && (this - 2)->final)
        cost += WIDOW_COST ((this - 1)->length);
    }

  if (this->paren)
    cost -= PAREN_BONUS;
  else if (this->final)
    cost += ORPHAN_COST (this->length);

  return cost;
}

