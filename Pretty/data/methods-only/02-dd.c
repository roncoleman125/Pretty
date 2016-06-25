static void
translate_charset (char const *new_trans)
{
  int i;

  for (i = 0; i < 256; i++)
    trans_table[i] = new_trans[trans_table[i]];
  translation_needed = true;
}

