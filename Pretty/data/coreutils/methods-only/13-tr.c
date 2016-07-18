/* Given a string, EQUIV_CLASS_STR, from a [=str=] context and
   the length of that string, LEN, if LEN is exactly one, append
   a newly allocated structure representing the specified
   equivalence class to the specification list, LIST and return true.
   If LEN is not 1, return false.  */

static bool
append_equiv_class (struct Spec_list *list,
                    char const *equiv_class_str, size_t len)
{
  struct List_element *new;

  if (len != 1)
    return false;
  new = xmalloc (sizeof *new);
  new->next = NULL;
  new->type = RE_EQUIV_CLASS;
  new->u.equiv_code = *equiv_class_str;
  assert (list->tail);
  list->tail->next = new;
  list->tail = new;
  return true;
}

