Line_ptr
line_ptr_decrement (const Buf *x, const Line_ptr *lp)
{
  Line_ptr lp_new;

  if (lp->ptr > x->p[lp->i].start)
    {
      lp_new.i = lp->i;
      lp_new.ptr = lp->ptr - 1;
    }
  else
    {
      assert (lp->i > 0);
      lp_new.i = lp->i - 1;
      lp_new.ptr = ONE_PAST_END (x, lp->i - 1) - 1;
    }
  return lp_new;
}

