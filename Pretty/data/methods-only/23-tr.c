static void
get_s1_spec_stats (struct Spec_list *s1)
{
  get_spec_stats (s1);
  if (complement)
    s1->length = card_of_complement (s1);
}

