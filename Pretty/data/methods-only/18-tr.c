/* Advance past the current construct.
   S->tail must be non-NULL.  */
static void
skip_construct (struct Spec_list *s)
{
  s->tail = s->tail->next;
  s->state = NEW_ELEMENT;
}

