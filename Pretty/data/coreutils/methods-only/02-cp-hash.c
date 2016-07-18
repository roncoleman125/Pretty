static void
src_to_dest_free (void *x)
{
  struct Src_to_dest *a = x;
  free (a->name);
  free (x);
}

