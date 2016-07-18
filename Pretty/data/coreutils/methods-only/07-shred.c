/*
 * Generate a random wiping pass pattern with num passes.
 * This is a two-stage process.  First, the passes to include
 * are chosen, and then they are shuffled into the desired
 * order.
 */
static void
genpattern (int *dest, size_t num, struct randint_source *s)
{
  size_t randpasses;
  int const *p;
  int *d;
  size_t n;
  size_t accum, top, swap;
  int k;

  if (!num)
    return;

  /* Stage 1: choose the passes to use */
  p = patterns;
  randpasses = 0;
  d = dest;			/* Destination for generated pass list */
  n = num;			/* Passes remaining to fill */

  while (true)
    {
      k = *p++;			/* Block descriptor word */
      if (!k)
        {			/* Loop back to the beginning */
          p = patterns;
        }
      else if (k < 0)
        {			/* -k random passes */
          k = -k;
          if ((size_t) k >= n)
            {
              randpasses += n;
              break;
            }
          randpasses += k;
          n -= k;
        }
      else if ((size_t) k <= n)
        {			/* Full block of patterns */
          memcpy (d, p, k * sizeof (int));
          p += k;
          d += k;
          n -= k;
        }
      else if (n < 2 || 3 * n < (size_t) k)
        {			/* Finish with random */
          randpasses += n;
          break;
        }
      else
        {			/* Pad out with k of the n available */
          do
            {
              if (n == (size_t) k || randint_choose (s, k) < n)
                {
                  *d++ = *p;
                  n--;
                }
              p++;
            }
          while (n);
          break;
        }
    }
  top = num - randpasses;	/* Top of initialized data */
  /* assert (d == dest+top); */

  /*
   * We now have fixed patterns in the dest buffer up to
   * "top", and we need to scramble them, with "randpasses"
   * random passes evenly spaced among them.
   *
   * We want one at the beginning, one at the end, and
   * evenly spaced in between.  To do this, we basically
   * use Bresenham's line draw (a.k.a DDA) algorithm
   * to draw a line with slope (randpasses-1)/(num-1).
   * (We use a positive accumulator and count down to
   * do this.)
   *
   * So for each desired output value, we do the following:
   * - If it should be a random pass, copy the pass type
   *   to top++, out of the way of the other passes, and
   *   set the current pass to -1 (random).
   * - If it should be a normal pattern pass, choose an
   *   entry at random between here and top-1 (inclusive)
   *   and swap the current entry with that one.
   */
  randpasses--;			/* To speed up later math */
  accum = randpasses;		/* Bresenham DDA accumulator */
  for (n = 0; n < num; n++)
    {
      if (accum <= randpasses)
        {
          accum += num - 1;
          dest[top++] = dest[n];
          dest[n] = -1;
        }
      else
        {
          swap = n + randint_choose (s, top - n);
          k = dest[n];
          dest[n] = dest[swap];
          dest[swap] = k;
        }
      accum -= randpasses;
    }
  /* assert (top == num); */
}

