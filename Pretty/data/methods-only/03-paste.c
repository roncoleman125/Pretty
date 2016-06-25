/* Perform column paste on the NFILES files named in FNAMPTR.
   Return true if successful, false if one or more files could not be
   opened or read. */

static bool
paste_parallel (size_t nfiles, char **fnamptr)
{
  bool ok = true;
  /* If all files are just ready to be closed, or will be on this
     round, the string of delimiters must be preserved.
     delbuf[0] through delbuf[nfiles]
     store the delimiters for closed files. */
  char *delbuf = xmalloc (nfiles + 2);

  /* Streams open to the files to process; NULL if the corresponding
     stream is closed.  */
  FILE **fileptr = xnmalloc (nfiles + 1, sizeof *fileptr);

  /* Number of files still open to process.  */
  size_t files_open;

  /* True if any fopen got fd == STDIN_FILENO.  */
  bool opened_stdin = false;

  /* Attempt to open all files.  This could be expanded to an infinite
     number of files, but at the (considerable) expense of remembering
     each file and its current offset, then opening/reading/closing.  */

  for (files_open = 0; files_open < nfiles; ++files_open)
    {
      if (STREQ (fnamptr[files_open], "-"))
        {
          have_read_stdin = true;
          fileptr[files_open] = stdin;
        }
      else
        {
          fileptr[files_open] = fopen (fnamptr[files_open], "r");
          if (fileptr[files_open] == NULL)
            error (EXIT_FAILURE, errno, "%s", fnamptr[files_open]);
          else if (fileno (fileptr[files_open]) == STDIN_FILENO)
            opened_stdin = true;
          fadvise (fileptr[files_open], FADVISE_SEQUENTIAL);
        }
    }

  if (opened_stdin && have_read_stdin)
    error (EXIT_FAILURE, 0, _("standard input is closed"));

  /* Read a line from each file and output it to stdout separated by a
     delimiter, until we go through the loop without successfully
     reading from any of the files. */

  while (files_open)
    {
      /* Set up for the next line. */
      bool somedone = false;
      char const *delimptr = delims;
      size_t delims_saved = 0;	/* Number of delims saved in `delbuf'. */
      size_t i;

      for (i = 0; i < nfiles && files_open; i++)
        {
          int chr IF_LINT ( = 0);	/* Input character. */
          int err IF_LINT ( = 0);	/* Input errno value.  */
          size_t line_length = 0;	/* Number of chars in line. */

          if (fileptr[i])
            {
              chr = getc (fileptr[i]);
              err = errno;
              if (chr != EOF && delims_saved)
                {
                  if (fwrite (delbuf, 1, delims_saved, stdout) != delims_saved)
                    write_error ();
                  delims_saved = 0;
                }

              while (chr != EOF)
                {
                  line_length++;
                  if (chr == '\n')
                    break;
                  xputchar (chr);
                  chr = getc (fileptr[i]);
                  err = errno;
                }
            }

          if (line_length == 0)
            {
              /* EOF, read error, or closed file.
                 If an EOF or error, close the file.  */
              if (fileptr[i])
                {
                  if (ferror (fileptr[i]))
                    {
                      error (0, err, "%s", fnamptr[i]);
                      ok = false;
                    }
                  if (fileptr[i] == stdin)
                    clearerr (fileptr[i]); /* Also clear EOF. */
                  else if (fclose (fileptr[i]) == EOF)
                    {
                      error (0, errno, "%s", fnamptr[i]);
                      ok = false;
                    }

                  fileptr[i] = NULL;
                  files_open--;
                }

              if (i + 1 == nfiles)
                {
                  /* End of this output line.
                     Is this the end of the whole thing? */
                  if (somedone)
                    {
                      /* No.  Some files were not closed for this line. */
                      if (delims_saved)
                        {
                          if (fwrite (delbuf, 1, delims_saved, stdout)
                              != delims_saved)
                            write_error ();
                          delims_saved = 0;
                        }
                      xputchar ('\n');
                    }
                  continue;	/* Next read of files, or exit. */
                }
              else
                {
                  /* Closed file; add delimiter to `delbuf'. */
                  if (*delimptr != EMPTY_DELIM)
                    delbuf[delims_saved++] = *delimptr;
                  if (++delimptr == delim_end)
                    delimptr = delims;
                }
            }
          else
            {
              /* Some data read. */
              somedone = true;

              /* Except for last file, replace last newline with delim. */
              if (i + 1 != nfiles)
                {
                  if (chr != '\n' && chr != EOF)
                    xputchar (chr);
                  if (*delimptr != EMPTY_DELIM)
                    xputchar (*delimptr);
                  if (++delimptr == delim_end)
                    delimptr = delims;
                }
              else
                {
                  /* If the last line of the last file lacks a newline,
                     print one anyhow.  POSIX requires this.  */
                  char c = (chr == EOF ? '\n' : chr);
                  xputchar (c);
                }
            }
        }
    }
  free (fileptr);
  free (delbuf);
  return ok;
}

