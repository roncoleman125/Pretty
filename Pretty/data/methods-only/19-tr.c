/* Given a Spec_list S (with its saved state implicit in the values
   of its members `tail' and `state'), return the next single character
   in the expansion of S's constructs.  If the last character of S was
   returned on the previous call or if S was empty, this function
   returns -1.  For example, successive calls to get_next where S
   represents the spec-string 'a-d[y*3]' will return the sequence
   of values a, b, c, d, y, y, y, -1.  Finally, if the construct from
   which the returned character comes is [:upper:] or [:lower:], the
   parameter CLASS is given a value to indicate which it was.  Otherwise
   CLASS is set to UL_NONE.  This value is used only when constructing
   the translation table to verify that any occurrences of upper and
   lower class constructs in the spec-strings appear in the same relative
   positions.  */

static int
get_next (struct Spec_list *s, enum Upper_Lower_class *class)
{
  struct List_element *p;
  int return_val;
  int i;

  if (class)
    *class = UL_NONE;

  if (s->state == BEGIN_STATE)
    {
      s->tail = s->head->next;
      s->state = NEW_ELEMENT;
    }

  p = s->tail;
  if (p == NULL)
    return -1;

  switch (p->type)
    {
    case RE_NORMAL_CHAR:
      return_val = p->u.normal_char;
      s->state = NEW_ELEMENT;
      s->tail = p->next;
      break;

    case RE_RANGE:
      if (s->state == NEW_ELEMENT)
        s->state = p->u.range.first_char;
      else
        ++(s->state);
      return_val = s->state;
      if (s->state == p->u.range.last_char)
        {
          s->tail = p->next;
          s->state = NEW_ELEMENT;
        }
      break;

    case RE_CHAR_CLASS:
      if (class)
        {
          switch (p->u.char_class)
            {
            case CC_LOWER:
              *class = UL_LOWER;
              break;
            case CC_UPPER:
              *class = UL_UPPER;
              break;
            default:
              break;
            }
        }

      if (s->state == NEW_ELEMENT)
        {
          for (i = 0; i < N_CHARS; i++)
            if (is_char_class_member (p->u.char_class, i))
              break;
          assert (i < N_CHARS);
          s->state = i;
        }
      assert (is_char_class_member (p->u.char_class, s->state));
      return_val = s->state;
      for (i = s->state + 1; i < N_CHARS; i++)
        if (is_char_class_member (p->u.char_class, i))
          break;
      if (i < N_CHARS)
        s->state = i;
      else
        {
          s->tail = p->next;
          s->state = NEW_ELEMENT;
        }
      break;

    case RE_EQUIV_CLASS:
      /* FIXME: this assumes that each character is alone in its own
         equivalence class (which appears to be correct for my
         LC_COLLATE.  But I don't know of any function that allows
         one to determine a character's equivalence class.  */

      return_val = p->u.equiv_code;
      s->state = NEW_ELEMENT;
      s->tail = p->next;
      break;

    case RE_REPEATED_CHAR:
      /* Here, a repeat count of n == 0 means don't repeat at all.  */
      if (p->u.repeated_char.repeat_count == 0)
        {
          s->tail = p->next;
          s->state = NEW_ELEMENT;
          return_val = get_next (s, class);
        }
      else
        {
          if (s->state == NEW_ELEMENT)
            {
              s->state = 0;
            }
          ++(s->state);
          return_val = p->u.repeated_char.the_repeated_char;
          if (s->state == p->u.repeated_char.repeat_count)
            {
              s->tail = p->next;
              s->state = NEW_ELEMENT;
            }
        }
      break;

    default:
      abort ();
      break;
    }

  return return_val;
}

