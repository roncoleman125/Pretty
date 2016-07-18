/* Scan through FILES[NTEMPS .. NFILES-1] looking for files that are
   the same as OUTFILE.  If found, replace each with the same
   temporary copy that can be merged into OUTFILE without destroying
   OUTFILE before it is completely read.  This temporary copy does not
   count as a merge temp, so don't worry about incrementing NTEMPS in
   the caller; final cleanup will remove it, not zaptemp.

   This test ensures that an otherwise-erroneous use like
   "sort -m -o FILE ... FILE ..." copies FILE before writing to it.
   It's not clear that POSIX requires this nicety.
   Detect common error cases, but don't try to catch obscure cases like
   "cat ... FILE ... | sort -m -o FILE"
   where traditional "sort" doesn't copy the input and where
   people should know that they're getting into trouble anyway.
   Catching these obscure cases would slow down performance in
   common cases.  */

static void
avoid_trashing_input (struct sortfile *files, size_t ntemps,
                      size_t nfiles, char const *outfile)
{
  size_t i;
  bool got_outstat = false;
  struct stat outstat;
  struct tempnode *tempcopy = NULL;

  for (i = ntemps; i < nfiles; i++)
    {
      bool is_stdin = STREQ (files[i].name, "-");
      bool same;
      struct stat instat;

      if (outfile && STREQ (outfile, files[i].name) && !is_stdin)
        same = true;
      else
        {
          if (! got_outstat)
            {
              if ((outfile
                   ? stat (outfile, &outstat)
                   : fstat (STDOUT_FILENO, &outstat))
                  != 0)
                break;
              got_outstat = true;
            }

          same = (((is_stdin
                    ? fstat (STDIN_FILENO, &instat)
                    : stat (files[i].name, &instat))
                   == 0)
                  && SAME_INODE (instat, outstat));
        }

      if (same)
        {
          if (! tempcopy)
            {
              FILE *tftp;
              tempcopy = create_temp (&tftp);
              mergefiles (&files[i], 0, 1, tftp, tempcopy->name);
            }

          files[i].name = tempcopy->name;
          files[i].temp = tempcopy;
        }
    }
}

