Line_ptr
line_ptr_increment (const Buf *x, const Line_ptr *lp)
{
  Line_ptr lp_new;

  assert (lp->ptr <= ONE_PAST_END (x, lp->i) - 1);
  if (lp->ptr < ONE_PAST_END (x, lp->i) - 1)
    {
      lp_new.i = lp->i;
      lp_new.ptr = lp->ptr + 1;
    }
  else
    {
      assert (lp->i < x->n_bufs - 1);
      lp_new.i = lp->i + 1;
      lp_new.ptr = x->p[lp->i + 1].start;
    }
  return lp_new;
}

