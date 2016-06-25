/* Initialize a boolean membership set, IN_SET, with the character
   values obtained by traversing the linked list of constructs S
   using the function `get_next'.  IN_SET is expected to have been
   initialized to all zeros by the caller.  If COMPLEMENT_THIS_SET
   is true the resulting set is complemented.  */

static void
set_initialize (struct Spec_list *s, bool complement_this_set, bool *in_set)
{
  int c;
  size_t i;

  s->state = BEGIN_STATE;
  while ((c = get_next (s, NULL)) != -1)
    in_set[c] = true;
  if (complement_this_set)
    for (i = 0; i < N_CHARS; i++)
      in_set[i] = (!in_set[i]);
}

