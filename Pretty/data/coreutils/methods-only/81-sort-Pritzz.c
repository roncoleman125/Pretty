/* Merge the input FILES.  NTEMPS is the number of files at the
   start of FILES that are temporary; it is zero at the top level.
   NFILES is the total number of files.  Put the output in
   OUTPUT_FILE; a null OUTPUT_FILE stands for standard output.  */

static void
merge (struct sortfile *files, size_t ntemps, size_t nfiles,
       char const *output_file)
{
  while (nmerge < nfiles)
    {
      /* Number of input files processed so far.  */
      size_t in;

      /* Number of output files generated so far.  */
      size_t out;

      /* nfiles % NMERGE; this counts input files that are left over
         after all full-sized merges have been done.  */
      size_t remainder;

      /* Number of easily-available slots at the next loop iteration.  */
      size_t cheap_slots;

      /* Do as many NMERGE-size merges as possible. In the case that
         nmerge is bogus, increment by the maximum number of file
         descriptors allowed.  */
      for (out = in = 0; nmerge <= nfiles - in; out++)
        {
          FILE *tfp;
          struct tempnode *temp = create_temp (&tfp);
          size_t num_merged = mergefiles (&files[in], MIN (ntemps, nmerge),
                                          nmerge, tfp, temp->name);
          ntemps -= MIN (ntemps, num_merged);
          files[out].name = temp->name;
          files[out].temp = temp;
          in += num_merged;
        }

      remainder = nfiles - in;
      cheap_slots = nmerge - out % nmerge;

      if (cheap_slots < remainder)
        {
          /* So many files remain that they can't all be put into the last
             NMERGE-sized output window.  Do one more merge.  Merge as few
             files as possible, to avoid needless I/O.  */
          size_t nshortmerge = remainder - cheap_slots + 1;
          FILE *tfp;
          struct tempnode *temp = create_temp (&tfp);
          size_t num_merged = mergefiles (&files[in], MIN (ntemps, nshortmerge),
                                          nshortmerge, tfp, temp->name);
          ntemps -= MIN (ntemps, num_merged);
          files[out].name = temp->name;
          files[out++].temp = temp;
          in += num_merged;
        }

      /* Put the remaining input files into the last NMERGE-sized output
         window, so they will be merged in the next pass.  */
      memmove (&files[out], &files[in], (nfiles - in) * sizeof *files);
      ntemps += out;
      nfiles -= in - out;
    }

  avoid_trashing_input (files, ntemps, nfiles, output_file);

  /* We aren't guaranteed that this final mergefiles will work, therefore we
     try to merge into the output, and then merge as much as we can into a
     temp file if we can't. Repeat.  */

  while (true)
    {
      /* Merge directly into the output file if possible.  */
      FILE **fps;
      size_t nopened = open_input_files (files, nfiles, &fps);

      if (nopened == nfiles)
        {
          FILE *ofp = stream_open (output_file, "w");
          if (ofp)
            {
              mergefps (files, ntemps, nfiles, ofp, output_file, fps);
              break;
            }
          if (errno != EMFILE || nopened <= 2)
            die (_("open failed"), output_file);
        }
      else if (nopened <= 2)
        die (_("open failed"), files[nopened].name);

      /* We ran out of file descriptors.  Close one of the input
         files, to gain a file descriptor.  Then create a temporary
         file with our spare file descriptor.  Retry if that failed
         (e.g., some other process could open a file between the time
         we closed and tried to create).  */
      FILE *tfp;
      struct tempnode *temp;
      do
        {
          nopened--;
          xfclose (fps[nopened], files[nopened].name);
          temp = maybe_create_temp (&tfp, ! (nopened <= 2));
        }
      while (!temp);

      /* Merge into the newly allocated temporary.  */
      mergefps (&files[0], MIN (ntemps, nopened), nopened, tfp, temp->name,
                fps);
      ntemps -= MIN (ntemps, nopened);
      files[0].name = temp->name;
      files[0].temp = temp;

      memmove (&files[1], &files[nopened], (nfiles - nopened) * sizeof *files);
      ntemps++;
      nfiles -= nopened - 1;
    }
}

