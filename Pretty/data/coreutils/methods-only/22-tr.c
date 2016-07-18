/* Gather statistics about the spec-list S in preparation for the tests
   in validate that determine the consistency of the specs.  This function
   is called at most twice; once for string1, and again for any string2.
   LEN_S1 < 0 indicates that this is the first call and that S represents
   string1.  When LEN_S1 >= 0, it is the length of the expansion of the
   constructs in string1, and we can use its value to resolve any
   indefinite repeat construct in S (which represents string2).  Hence,
   this function has the side-effect that it converts a valid [c*]
   construct in string2 to [c*n] where n is large enough (or 0) to give
   string2 the same length as string1.  For example, with the command
   tr a-z 'A[\n*]Z' on the second call to get_spec_stats, LEN_S1 would
   be 26 and S (representing string2) would be converted to 'A[\n*24]Z'.  */

static void
get_spec_stats (struct Spec_list *s)
{
  struct List_element *p;
  count length = 0;

  s->n_indefinite_repeats = 0;
  s->has_equiv_class = false;
  s->has_restricted_char_class = false;
  s->has_char_class = false;
  for (p = s->head->next; p; p = p->next)
    {
      int i;
      count len = 0;
      count new_length;

      switch (p->type)
        {
        case RE_NORMAL_CHAR:
          len = 1;
          break;

        case RE_RANGE:
          assert (p->u.range.last_char >= p->u.range.first_char);
          len = p->u.range.last_char - p->u.range.first_char + 1;
          break;

        case RE_CHAR_CLASS:
          s->has_char_class = true;
          for (i = 0; i < N_CHARS; i++)
            if (is_char_class_member (p->u.char_class, i))
              ++len;
          switch (p->u.char_class)
            {
            case CC_UPPER:
            case CC_LOWER:
              break;
            default:
              s->has_restricted_char_class = true;
              break;
            }
          break;

        case RE_EQUIV_CLASS:
          for (i = 0; i < N_CHARS; i++)
            if (is_equiv_class_member (p->u.equiv_code, i))
              ++len;
          s->has_equiv_class = true;
          break;

        case RE_REPEATED_CHAR:
          if (p->u.repeated_char.repeat_count > 0)
            len = p->u.repeated_char.repeat_count;
          else
            {
              s->indefinite_repeat_element = p;
              ++(s->n_indefinite_repeats);
            }
          break;

        default:
          abort ();
          break;
        }

      /* Check for arithmetic overflow in computing length.  Also, reject
         any length greater than the maximum repeat count, in case the
         length is later used to compute the repeat count for an
         indefinite element.  */
      new_length = length + len;
      if (! (length <= new_length && new_length <= REPEAT_COUNT_MAXIMUM))
        error (EXIT_FAILURE, 0, _("too many characters in set"));
      length = new_length;
    }

  s->length = length;
}

