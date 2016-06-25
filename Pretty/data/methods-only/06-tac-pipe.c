static bool
tac_mem ()
{
  Buf x;
  Line_ptr bol;
  char eol_byte = '\n';

  if (! buf_init_from_stdin (&x, eol_byte))
    {
      buf_free (&x);
      return false;
    }

  /* Special case the empty file.  */
  if (EMPTY (&x))
    return true;

  /* Initially, point at one past the last byte of the file.  */
  bol.i = x.n_bufs - 1;
  bol.ptr = ONE_PAST_END (&x, bol.i);

  while (1)
    {
      Line_ptr new_bol;
      if (! find_bol (&x, &bol, &new_bol, eol_byte))
        break;
      print_line (stdout, &x, &new_bol, &bol);
      bol = new_bol;
    }
  return true;
}
