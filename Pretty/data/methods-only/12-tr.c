/* Append a newly allocated structure representing a [c*n]
   repeated character construct to the specification list LIST.
   THE_CHAR is the single character to be repeated, and REPEAT_COUNT
   is a non-negative repeat count.  */

static void
append_repeated_char (struct Spec_list *list, unsigned char the_char,
                      count repeat_count)
{
  struct List_element *new;

  new = xmalloc (sizeof *new);
  new->next = NULL;
  new->type = RE_REPEATED_CHAR;
  new->u.repeated_char.the_repeated_char = the_char;
  new->u.repeated_char.repeat_count = repeat_count;
  assert (list->tail);
  list->tail->next = new;
  list->tail = new;
}

