static void
spec_init (struct Spec_list *spec_list)
{
  struct List_element *new = xmalloc (sizeof *new);
  spec_list->head = spec_list->tail = new;
  spec_list->head->next = NULL;
}

