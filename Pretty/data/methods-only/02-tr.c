/* Return nonzero if the character C is a member of the
   equivalence class containing the character EQUIV_CLASS.  */

static inline bool
is_equiv_class_member (unsigned char equiv_class, unsigned char c)
{
  return (equiv_class == c);
}

