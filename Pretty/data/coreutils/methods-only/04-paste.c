/* Perform serial paste on the NFILES files named in FNAMPTR.
   Return true if no errors, false if one or more files could not be
   opened or read. */

static bool
paste_serial (size_t nfiles, char **fnamptr)
{
  bool ok = true;	/* false if open or read errors occur. */
  int charnew, charold; /* Current and previous char read. */
  char const *delimptr;	/* Current delimiter char. */
  FILE *fileptr;	/* Open for reading current file. */

  for (; nfiles; nfiles--, fnamptr++)
    {
      int saved_errno;
      bool is_stdin = STREQ (*fnamptr, "-");
      if (is_stdin)
        {
          have_read_stdin = true;
          fileptr = stdin;
        }
      else
        {
          fileptr = fopen (*fnamptr, "r");
          if (fileptr == NULL)
            {
              error (0, errno, "%s", *fnamptr);
              ok = false;
              continue;
            }
          fadvise (fileptr, FADVISE_SEQUENTIAL);
        }

      delimptr = delims;	/* Set up for delimiter string. */

      charold = getc (fileptr);
      saved_errno = errno;
      if (charold != EOF)
        {
          /* `charold' is set up.  Hit it!
             Keep reading characters, stashing them in `charnew';
             output `charold', converting to the appropriate delimiter
             character if needed.  After the EOF, output `charold'
             if it's a newline; otherwise, output it and then a newline. */

          while ((charnew = getc (fileptr)) != EOF)
            {
              /* Process the old character. */
              if (charold == '\n')
                {
                  if (*delimptr != EMPTY_DELIM)
                    xputchar (*delimptr);

                  if (++delimptr == delim_end)
                    delimptr = delims;
                }
              else
                xputchar (charold);

              charold = charnew;
            }
          saved_errno = errno;

          /* Hit EOF.  Process that last character. */
          xputchar (charold);
        }

      if (charold != '\n')
        xputchar ('\n');

      if (ferror (fileptr))
        {
          error (0, saved_errno, "%s", *fnamptr);
          ok = false;
        }
      if (is_stdin)
        clearerr (fileptr);	/* Also clear EOF. */
      else if (fclose (fileptr) == EOF)
        {
          error (0, errno, "%s", *fnamptr);
          ok = false;
        }
    }
  return ok;
}

