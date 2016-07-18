/* If CHAR_CLASS_STR is a valid character class string, append a
   newly allocated structure representing that character class to the end
   of the specification list LIST and return true.  If CHAR_CLASS_STR is not
   a valid string return false.  */

static bool
append_char_class (struct Spec_list *list,
                   char const *char_class_str, size_t len)
{
  enum Char_class char_class;
  struct List_element *new;

  char_class = look_up_char_class (char_class_str, len);
  if (char_class == CC_NO_CLASS)
    return false;
  new = xmalloc (sizeof *new);
  new->next = NULL;
  new->type = RE_CHAR_CLASS;
  new->u.char_class = char_class;
  assert (list->tail);
  list->tail->next = new;
  list->tail = new;
  return true;
}

