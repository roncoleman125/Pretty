/* Print the join of the files in FP1 and FP2.  */

static void
join (FILE *fp1, FILE *fp2)
{
  struct seq seq1, seq2;
  int diff;
  bool eof1, eof2;

  fadvise (fp1, FADVISE_SEQUENTIAL);
  fadvise (fp2, FADVISE_SEQUENTIAL);

  /* Read the first line of each file.  */
  initseq (&seq1);
  getseq (fp1, &seq1, 1);
  initseq (&seq2);
  getseq (fp2, &seq2, 2);

  if (autoformat)
    {
      autocount_1 = seq1.count ? seq1.lines[0]->nfields : 0;
      autocount_2 = seq2.count ? seq2.lines[0]->nfields : 0;
    }

  if (join_header_lines && (seq1.count || seq2.count))
    {
      struct line const *hline1 = seq1.count ? seq1.lines[0] : &uni_blank;
      struct line const *hline2 = seq2.count ? seq2.lines[0] : &uni_blank;
      prjoin (hline1, hline2);
      prevline[0] = NULL;
      prevline[1] = NULL;
      if (seq1.count)
        advance_seq (fp1, &seq1, true, 1);
      if (seq2.count)
        advance_seq (fp2, &seq2, true, 2);
    }

  while (seq1.count && seq2.count)
    {
      size_t i;
      diff = keycmp (seq1.lines[0], seq2.lines[0],
                     join_field_1, join_field_2);
      if (diff < 0)
        {
          if (print_unpairables_1)
            prjoin (seq1.lines[0], &uni_blank);
          advance_seq (fp1, &seq1, true, 1);
          seen_unpairable = true;
          continue;
        }
      if (diff > 0)
        {
          if (print_unpairables_2)
            prjoin (&uni_blank, seq2.lines[0]);
          advance_seq (fp2, &seq2, true, 2);
          seen_unpairable = true;
          continue;
        }

      /* Keep reading lines from file1 as long as they continue to
         match the current line from file2.  */
      eof1 = false;
      do
        if (!advance_seq (fp1, &seq1, false, 1))
          {
            eof1 = true;
            ++seq1.count;
            break;
          }
      while (!keycmp (seq1.lines[seq1.count - 1], seq2.lines[0],
                      join_field_1, join_field_2));

      /* Keep reading lines from file2 as long as they continue to
         match the current line from file1.  */
      eof2 = false;
      do
        if (!advance_seq (fp2, &seq2, false, 2))
          {
            eof2 = true;
            ++seq2.count;
            break;
          }
      while (!keycmp (seq1.lines[0], seq2.lines[seq2.count - 1],
                      join_field_1, join_field_2));

      if (print_pairables)
        {
          for (i = 0; i < seq1.count - 1; ++i)
            {
              size_t j;
              for (j = 0; j < seq2.count - 1; ++j)
                prjoin (seq1.lines[i], seq2.lines[j]);
            }
        }

      if (!eof1)
        {
          SWAPLINES (seq1.lines[0], seq1.lines[seq1.count - 1]);
          seq1.count = 1;
        }
      else
        seq1.count = 0;

      if (!eof2)
        {
          SWAPLINES (seq2.lines[0], seq2.lines[seq2.count - 1]);
          seq2.count = 1;
        }
      else
        seq2.count = 0;
    }

  /* If the user did not specify --nocheck-order, then we read the
     tail ends of both inputs to verify that they are in order.  We
     skip the rest of the tail once we have issued a warning for that
     file, unless we actually need to print the unpairable lines.  */
  struct line *line = NULL;
  bool checktail = false;

  if (check_input_order != CHECK_ORDER_DISABLED
      && !(issued_disorder_warning[0] && issued_disorder_warning[1]))
    checktail = true;

  if ((print_unpairables_1 || checktail) && seq1.count)
    {
      if (print_unpairables_1)
        prjoin (seq1.lines[0], &uni_blank);
      if (seq2.count)
        seen_unpairable = true;
      while (get_line (fp1, &line, 1))
        {
          if (print_unpairables_1)
            prjoin (line, &uni_blank);
          if (issued_disorder_warning[0] && !print_unpairables_1)
            break;
        }
    }

  if ((print_unpairables_2 || checktail) && seq2.count)
    {
      if (print_unpairables_2)
        prjoin (&uni_blank, seq2.lines[0]);
      if (seq1.count)
        seen_unpairable = true;
      while (get_line (fp2, &line, 2))
        {
          if (print_unpairables_2)
            prjoin (&uni_blank, line);
          if (issued_disorder_warning[1] && !print_unpairables_2)
            break;
        }
    }

  freeline (line);
  free (line);

  delseq (&seq1);
  delseq (&seq2);
}

