/* Discard the lengths associated with a case conversion,
   as using the actual number of upper or lower case characters
   is problematic when they don't match in some locales.
   Also ensure the case conversion classes in string2 are
   aligned correctly with those in string1.
   Note POSIX says the behavior of `tr "[:upper:]" "[:upper:]"'
   is undefined.  Therefore we allow it (unlike Solaris)
   and treat it as a no-op.  */

static void
validate_case_classes (struct Spec_list *s1, struct Spec_list *s2)
{
  size_t n_upper = 0;
  size_t n_lower = 0;
  unsigned int i;
  int c1 = 0;
  int c2 = 0;
  count old_s1_len = s1->length;
  count old_s2_len = s2->length;
  struct List_element *s1_tail = s1->tail;
  struct List_element *s2_tail = s2->tail;
  bool s1_new_element = true;
  bool s2_new_element = true;

  if (!s2->has_char_class)
    return;

  for (i = 0; i < N_CHARS; i++)
    {
      if (isupper (i))
        n_upper++;
      if (islower (i))
        n_lower++;
    }

  s1->state = BEGIN_STATE;
  s2->state = BEGIN_STATE;

  while (c1 != -1 && c2 != -1)
    {
      enum Upper_Lower_class class_s1, class_s2;

      c1 = get_next (s1, &class_s1);
      c2 = get_next (s2, &class_s2);

      /* If c2 transitions to a new case class, then
         c1 must also transition at the same time.  */
      if (s2_new_element && class_s2 != UL_NONE
          && !(s1_new_element && class_s1 != UL_NONE))
        error (EXIT_FAILURE, 0,
               _("misaligned [:upper:] and/or [:lower:] construct"));

      /* If case converting, quickly skip over the elements.  */
      if (class_s2 != UL_NONE)
        {
          skip_construct (s1);
          skip_construct (s2);
          /* Discount insignificant/problematic lengths.  */
          s1->length -= (class_s1 == UL_UPPER ? n_upper : n_lower) - 1;
          s2->length -= (class_s2 == UL_UPPER ? n_upper : n_lower) - 1;
        }

      s1_new_element = s1->state == NEW_ELEMENT; /* Next element is new.  */
      s2_new_element = s2->state == NEW_ELEMENT; /* Next element is new.  */
    }

  assert (old_s1_len >= s1->length && old_s2_len >= s2->length);

  s1->tail = s1_tail;
  s2->tail = s2_tail;
}

