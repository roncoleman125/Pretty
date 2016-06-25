/* Read a line from FP and add it to SEQ, as the first item if FIRST is
   true, else as the next.  */
static bool
advance_seq (FILE *fp, struct seq *seq, bool first, int whichfile)
{
  if (first)
    seq->count = 0;

  return getseq (fp, seq, whichfile);
}

