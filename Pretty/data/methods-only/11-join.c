/* Read a line from FP and add it to SEQ.  Return true if successful.  */

static bool
getseq (FILE *fp, struct seq *seq, int whichfile)
{
  if (seq->count == seq->alloc)
    {
      size_t i;
      seq->lines = X2NREALLOC (seq->lines, &seq->alloc);
      for (i = seq->count; i < seq->alloc; i++)
        seq->lines[i] = NULL;
    }

  if (get_line (fp, &seq->lines[seq->count], whichfile))
    {
      ++seq->count;
      return true;
    }
  return false;
}

