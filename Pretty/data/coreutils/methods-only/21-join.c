/* Add the comma or blank separated field spec(s) in STR to `outlist'.  */

static void
add_field_list (char *str)
{
  char *p = str;

  do
    {
      int file_index;
      size_t field_index;
      char const *spec_item = p;

      p = strpbrk (p, ", \t");
      if (p)
        *p++ = '\0';
      decode_field_spec (spec_item, &file_index, &field_index);
      add_field (file_index, field_index);
    }
  while (p);
}

