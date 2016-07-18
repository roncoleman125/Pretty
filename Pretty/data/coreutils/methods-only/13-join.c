static void
delseq (struct seq *seq)
{
  size_t i;
  for (i = 0; i < seq->alloc; i++)
    {
      freeline (seq->lines[i]);
      free (seq->lines[i]);
    }
  free (seq->lines);
}


