static bool
find_bol (const Buf *x,
          const Line_ptr *last_bol, Line_ptr *new_bol, char eol_byte)
{
  size_t i;
  Line_ptr tmp;
  char *last_bol_ptr;

  if (last_bol->ptr == x->p[0].start)
    return false;

  tmp = line_ptr_decrement (x, last_bol);
  last_bol_ptr = tmp.ptr;
  i = tmp.i;
  while (1)
    {
      char *nl = memrchr (x->p[i].start, last_bol_ptr, eol_byte);
      if (nl)
        {
          Line_ptr nl_pos;
          nl_pos.i = i;
          nl_pos.ptr = nl;
          *new_bol = line_ptr_increment (x, &nl_pos);
          return true;
        }

      if (i == 0)
        break;

      --i;
      last_bol_ptr = ONE_PAST_END (x, i);
    }

  /* If last_bol->ptr didn't point at the first byte of X, then reaching
     this point means that we're about to return the line that is at the
     beginning of X.  */
  if (last_bol->ptr != x->p[0].start)
    {
      new_bol->i = 0;
      new_bol->ptr = x->p[0].start;
      return true;
    }

  return false;
}

