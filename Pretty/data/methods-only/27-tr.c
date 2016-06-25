/* Given two specification lists, S1 and S2, and assuming that
   S1->length > S2->length, append a single [c*n] element to S2 where c
   is the last character in the expansion of S2 and n is the difference
   between the two lengths.
   Upon successful completion, S2->length is set to S1->length.  The only
   way this function can fail to make S2 as long as S1 is when S2 has
   zero-length, since in that case, there is no last character to repeat.
   So S2->length is required to be at least 1.  */


static void
string2_extend (const struct Spec_list *s1, struct Spec_list *s2)
{
  struct List_element *p;
  unsigned char char_to_repeat;

  assert (translating);
  assert (s1->length > s2->length);
  assert (s2->length > 0);

  p = s2->tail;
  switch (p->type)
    {
    case RE_NORMAL_CHAR:
      char_to_repeat = p->u.normal_char;
      break;
    case RE_RANGE:
      char_to_repeat = p->u.range.last_char;
      break;
    case RE_CHAR_CLASS:
      /* Note BSD allows extending of classes in string2.  For example:
           tr '[:upper:]0-9' '[:lower:]'
         That's not portable however, contradicts POSIX and is dependent
         on your collating sequence.  */
      error (EXIT_FAILURE, 0,
             _("when translating with string1 longer than string2,\n\
the latter string must not end with a character class"));
      abort (); /* inform gcc that the above use of error never returns. */
      break;

    case RE_REPEATED_CHAR:
      char_to_repeat = p->u.repeated_char.the_repeated_char;
      break;

    case RE_EQUIV_CLASS:
      /* This shouldn't happen, because validate exits with an error
         if it finds an equiv class in string2 when translating.  */
      abort ();
      break;

    default:
      abort ();
      break;
    }

  append_repeated_char (s2, char_to_repeat, s1->length - s2->length);
  s2->length = s1->length;
}

