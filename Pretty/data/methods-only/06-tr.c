/* If CLASS_STR is a valid character class string, return its index
   in the global char_class_name array.  Otherwise, return CC_NO_CLASS.  */

static enum Char_class
look_up_char_class (char const *class_str, size_t len)
{
  enum Char_class i;

  for (i = 0; i < ARRAY_CARDINALITY (char_class_name); i++)
    if (strncmp (class_str, char_class_name[i], len) == 0
        && strlen (char_class_name[i]) == len)
      return i;
  return CC_NO_CLASS;
}

