static void
buf_free (Buf *x)
{
  size_t i;
  for (i = 0; i < x->n_bufs; i++)
    free (x->p[i].start);
  obstack_free (OBS, NULL);
}

