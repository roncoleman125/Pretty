/* Append a newly allocated structure representing the range
   of characters from FIRST to LAST to the specification list LIST.
   Return false if LAST precedes FIRST in the collating sequence,
   true otherwise.  This means that '[c-c]' is acceptable.  */

static bool
append_range (struct Spec_list *list, unsigned char first, unsigned char last)
{
  struct List_element *new;

  if (last < first)
    {
      char *tmp1 = make_printable_char (first);
      char *tmp2 = make_printable_char (last);

      error (0, 0,
       _("range-endpoints of `%s-%s' are in reverse collating sequence order"),
             tmp1, tmp2);
      free (tmp1);
      free (tmp2);
      return false;
    }
  new = xmalloc (sizeof *new);
  new->next = NULL;
  new->type = RE_RANGE;
  new->u.range.first_char = first;
  new->u.range.last_char = last;
  assert (list->tail);
  list->tail->next = new;
  list->tail = new;
  return true;
}

