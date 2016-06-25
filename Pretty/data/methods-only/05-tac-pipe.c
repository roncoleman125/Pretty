static void
print_line (FILE *out_stream, const Buf *x,
            const Line_ptr *bol, const Line_ptr *bol_next)
{
  size_t i;
  for (i = bol->i; i <= bol_next->i; i++)
    {
      char *a = (i == bol->i ? bol->ptr : x->p[i].start);
      char *b = (i == bol_next->i ? bol_next->ptr : ONE_PAST_END (x, i));
      fwrite (a, 1, b - a, out_stream);
    }
}

