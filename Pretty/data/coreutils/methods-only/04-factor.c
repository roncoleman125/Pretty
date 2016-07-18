static bool
do_stdin (void)
{
  bool ok = true;
  token_buffer tokenbuffer;

  init_tokenbuffer (&tokenbuffer);

  while (true)
    {
      size_t token_length = readtoken (stdin, DELIM, sizeof (DELIM) - 1,
                                       &tokenbuffer);
      if (token_length == (size_t) -1)
        break;
      ok &= print_factors (tokenbuffer.buffer);
    }
  free (tokenbuffer.buffer);

  return ok;
}

