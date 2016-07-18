/* Return the file status for the NFILES files addressed by FILE.
   Optimize the case where only one number is printed, for just one
   file; in that case we can use a print width of 1, so we don't need
   to stat the file.  Handle the case of (nfiles == 0) in the same way;
   that happens when we don't know how long the list of file names will be.  */

static struct fstatus *
get_input_fstatus (int nfiles, char *const *file)
{
  struct fstatus *fstatus = xnmalloc (nfiles ? nfiles : 1, sizeof *fstatus);

  if (nfiles == 0
      || (nfiles == 1
          && ((print_lines + print_words + print_chars
               + print_bytes + print_linelength)
              == 1)))
    fstatus[0].failed = 1;
  else
    {
      int i;

      for (i = 0; i < nfiles; i++)
        fstatus[i].failed = (! file[i] || STREQ (file[i], "-")
                             ? fstat (STDIN_FILENO, &fstatus[i].st)
                             : stat (file[i], &fstatus[i].st));
    }

  return fstatus;
}

