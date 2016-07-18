/* Append a newly allocated structure representing a
   character C to the specification list LIST.  */

static void
append_normal_char (struct Spec_list *list, unsigned char c)
{
  struct List_element *new;

  new = xmalloc (sizeof *new);
  new->next = NULL;
  new->type = RE_NORMAL_CHAR;
  new->u.normal_char = c;
  assert (list->tail);
  list->tail->next = new;
  list->tail = new;
}

