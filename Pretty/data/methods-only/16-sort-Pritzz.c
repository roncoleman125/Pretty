static FILE *
stream_open (char const *file, char const *how)
{
  if (!file)
    return stdout;
  if (*how == 'r')
    {
      FILE *fp;
      if (STREQ (file, "-"))
        {
          have_read_stdin = true;
          fp = stdin;
        }
      else
        fp = fopen (file, how);
      fadvise (fp, FADVISE_SEQUENTIAL);
      return fp;
    }
  return fopen (file, how);
}

