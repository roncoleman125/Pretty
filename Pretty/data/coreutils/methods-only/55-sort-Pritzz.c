/* Compare two lines A and B, returning negative, zero, or positive
   depending on whether A compares less than, equal to, or greater than B. */

static int
compare (struct line const *a, struct line const *b)
{
  int diff;
  size_t alen, blen;

  /* First try to compare on the specified keys (if any).
     The only two cases with no key at all are unadorned sort,
     and unadorned sort -r. */
  if (keylist)
    {
      diff = keycompare (a, b);
      if (diff || unique || stable)
        return diff;
    }

  /* If the keys all compare equal (or no keys were specified)
     fall through to the default comparison.  */
  alen = a->length - 1, blen = b->length - 1;

  if (alen == 0)
    diff = - NONZERO (blen);
  else if (blen == 0)
    diff = 1;
  else if (hard_LC_COLLATE)
    {
      /* Note xmemcoll0 is a performance enhancement as
         it will not unconditionally write '\0' after the
         passed in buffers, which was seen to give around
         a 3% increase in performance for short lines.  */
      diff = xmemcoll0 (a->text, alen + 1, b->text, blen + 1);
    }
  else if (! (diff = memcmp (a->text, b->text, MIN (alen, blen))))
    diff = alen < blen ? -1 : alen != blen;

  return reverse ? -diff : diff;
}

