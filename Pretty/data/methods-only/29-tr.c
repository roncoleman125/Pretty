/* Die with an error message if S1 and S2 describe strings that
   are not valid with the given command line switches.
   A side effect of this function is that if a valid [c*] or
   [c*0] construct appears in string2, it is converted to [c*n]
   with a value for n that makes s2->length == s1->length.  By
   the same token, if the --truncate-set1 option is not
   given, S2 may be extended.  */

static void
validate (struct Spec_list *s1, struct Spec_list *s2)
{
  get_s1_spec_stats (s1);
  if (s1->n_indefinite_repeats > 0)
    {
      error (EXIT_FAILURE, 0,
             _("the [c*] repeat construct may not appear in string1"));
    }

  if (s2)
    {
      get_s2_spec_stats (s2, s1->length);

      if (s2->n_indefinite_repeats > 1)
        {
          error (EXIT_FAILURE, 0,
                 _("only one [c*] repeat construct may appear in string2"));
        }

      if (translating)
        {
          if (s2->has_equiv_class)
            {
              error (EXIT_FAILURE, 0,
                     _("[=c=] expressions may not appear in string2 \
when translating"));
            }

          if (s2->has_restricted_char_class)
            {
              error (EXIT_FAILURE, 0,
                     _("when translating, the only character classes that may \
appear in\nstring2 are `upper' and `lower'"));
            }

          validate_case_classes (s1, s2);

          if (s1->length > s2->length)
            {
              if (!truncate_set1)
                {
                  /* string2 must be non-empty unless --truncate-set1 is
                     given or string1 is empty.  */

                  if (s2->length == 0)
                    error (EXIT_FAILURE, 0,
                     _("when not truncating set1, string2 must be non-empty"));
                  string2_extend (s1, s2);
                }
            }

          if (complement && s1->has_char_class
              && ! (s2->length == s1->length && homogeneous_spec_list (s2)))
            {
              error (EXIT_FAILURE, 0,
                     _("when translating with complemented character classes,\
\nstring2 must map all characters in the domain to one"));
            }
        }
      else
        /* Not translating.  */
        {
          if (s2->n_indefinite_repeats > 0)
            error (EXIT_FAILURE, 0,
                   _("the [c*] construct may appear in string2 only \
when translating"));
        }
    }
}

