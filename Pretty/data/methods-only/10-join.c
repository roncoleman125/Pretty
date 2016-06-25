static void
initseq (struct seq *seq)
{
  seq->count = 0;
  seq->alloc = 0;
  seq->lines = NULL;
}

