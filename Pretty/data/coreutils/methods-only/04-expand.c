/* Close the old stream pointer FP if it is non-NULL,
   and return a new one opened to read the next input file.
   Open a filename of `-' as the standard input.
   Return NULL if there are no more input files.  */

static FILE *
next_file (FILE *fp)
{
  static char *prev_file;
  char *file;

  if (fp)
    {
      if (ferror (fp))
        {
          error (0, errno, "%s", prev_file);
          exit_status = EXIT_FAILURE;
        }
      if (STREQ (prev_file, "-"))
        clearerr (fp);		/* Also clear EOF.  */
      else if (fclose (fp) != 0)
        {
          error (0, errno, "%s", prev_file);
          exit_status = EXIT_FAILURE;
        }
    }

  while ((file = *file_list++) != NULL)
    {
      if (STREQ (file, "-"))
        {
          have_read_stdin = true;
          fp = stdin;
        }
      else
        fp = fopen (file, "r");
      if (fp)
        {
          prev_file = file;
          fadvise (fp, FADVISE_SEQUENTIAL);
          return fp;
        }
      error (0, errno, "%s", file);
      exit_status = EXIT_FAILURE;
    }
  return NULL;
}

