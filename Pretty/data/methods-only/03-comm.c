/* Compare INFILES[0] and INFILES[1].
   If either is "-", use the standard input for that file.
   Assume that each input file is sorted;
   merge them and output the result.  */

static void
compare_files (char **infiles)
{
  /* For each file, we have four linebuffers in lba. */
  struct linebuffer lba[2][4];

  /* thisline[i] points to the linebuffer holding the next available line
     in file i, or is NULL if there are no lines left in that file.  */
  struct linebuffer *thisline[2];

  /* all_line[i][alt[i][0]] also points to the linebuffer holding the
     current line in file i. We keep two buffers of history around so we
     can look two lines back when we get to the end of a file. */
  struct linebuffer *all_line[2][4];

  /* This is used to rotate through the buffers for each input file. */
  int alt[2][3];

  /* streams[i] holds the input stream for file i.  */
  FILE *streams[2];

  int i, j;

  /* Initialize the storage. */
  for (i = 0; i < 2; i++)
    {
      for (j = 0; j < 4; j++)
        {
          initbuffer (&lba[i][j]);
          all_line[i][j] = &lba[i][j];
        }
      alt[i][0] = 0;
      alt[i][1] = 0;
      alt[i][2] = 0;
      streams[i] = (STREQ (infiles[i], "-") ? stdin : fopen (infiles[i], "r"));
      if (!streams[i])
        error (EXIT_FAILURE, errno, "%s", infiles[i]);

      fadvise (streams[i], FADVISE_SEQUENTIAL);

      thisline[i] = readlinebuffer (all_line[i][alt[i][0]], streams[i]);
      if (ferror (streams[i]))
        error (EXIT_FAILURE, errno, "%s", infiles[i]);
    }

  while (thisline[0] || thisline[1])
    {
      int order;
      bool fill_up[2] = { false, false };

      /* Compare the next available lines of the two files.  */

      if (!thisline[0])
        order = 1;
      else if (!thisline[1])
        order = -1;
      else
        {
          if (hard_LC_COLLATE)
            order = xmemcoll (thisline[0]->buffer, thisline[0]->length - 1,
                              thisline[1]->buffer, thisline[1]->length - 1);
          else
            {
              size_t len = min (thisline[0]->length, thisline[1]->length) - 1;
              order = memcmp (thisline[0]->buffer, thisline[1]->buffer, len);
              if (order == 0)
                order = (thisline[0]->length < thisline[1]->length
                         ? -1
                         : thisline[0]->length != thisline[1]->length);
            }
        }

      /* Output the line that is lesser. */
      if (order == 0)
        writeline (thisline[1], stdout, 3);
      else
        {
          seen_unpairable = true;
          if (order <= 0)
            writeline (thisline[0], stdout, 1);
          else
            writeline (thisline[1], stdout, 2);
        }

      /* Step the file the line came from.
         If the files match, step both files.  */
      if (0 <= order)
        fill_up[1] = true;
      if (order <= 0)
        fill_up[0] = true;

      for (i = 0; i < 2; i++)
        if (fill_up[i])
          {
            /* Rotate the buffers for this file. */
            alt[i][2] = alt[i][1];
            alt[i][1] = alt[i][0];
            alt[i][0] = (alt[i][0] + 1) & 0x03;

            thisline[i] = readlinebuffer (all_line[i][alt[i][0]], streams[i]);

            if (thisline[i])
              check_order (all_line[i][alt[i][1]], thisline[i], i + 1);

            /* If this is the end of the file we may need to re-check
               the order of the previous two lines, since we might have
               discovered an unpairable match since we checked before. */
            else if (all_line[i][alt[i][2]]->buffer)
              check_order (all_line[i][alt[i][2]],
                           all_line[i][alt[i][1]], i + 1);

            if (ferror (streams[i]))
              error (EXIT_FAILURE, errno, "%s", infiles[i]);

            fill_up[i] = false;
          }
    }

  for (i = 0; i < 2; i++)
    if (fclose (streams[i]) != 0)
      error (EXIT_FAILURE, errno, "%s", infiles[i]);
}

