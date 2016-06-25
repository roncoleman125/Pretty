static void
get_s2_spec_stats (struct Spec_list *s2, count len_s1)
{
  get_spec_stats (s2);
  if (len_s1 >= s2->length && s2->n_indefinite_repeats == 1)
    {
      s2->indefinite_repeat_element->u.repeated_char.repeat_count =
        len_s1 - s2->length;
      s2->length = len_s1;
    }
}

