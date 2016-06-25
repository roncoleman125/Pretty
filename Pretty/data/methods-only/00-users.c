static int
userid_compare (const void *v_a, const void *v_b)
{
  char **a = (char **) v_a;
  char **b = (char **) v_b;
  return strcmp (*a, *b);
}

