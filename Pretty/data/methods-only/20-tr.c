/* This is a minor kludge.  This function is called from
   get_spec_stats to determine the cardinality of a set derived
   from a complemented string.  It's a kludge in that some of the
   same operations are (duplicated) performed in set_initialize.  */

static int
card_of_complement (struct Spec_list *s)
{
  int c;
  int cardinality = N_CHARS;
  bool in_set[N_CHARS] = { 0, };

  s->state = BEGIN_STATE;
  while ((c = get_next (s, NULL)) != -1)
    {
      cardinality -= (!in_set[c]);
      in_set[c] = true;
    }
  return cardinality;
}

