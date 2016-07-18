/* Add a field spec for field FIELD of file FILE to `outlist'.  */

static void
add_field (int file, size_t field)
{
  struct outlist *o;

  assert (file == 0 || file == 1 || file == 2);
  assert (file != 0 || field == 0);

  o = xmalloc (sizeof *o);
  o->file = file;
  o->field = field;
  o->next = NULL;

  /* Add to the end of the list so the fields are in the right order.  */
  outlist_end->next = o;
  outlist_end = o;
}

