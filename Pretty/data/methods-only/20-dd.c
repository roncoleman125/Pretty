/* Fix up translation table. */

static void
apply_translations (void)
{
  int i;

  if (conversions_mask & C_ASCII)
    translate_charset (ebcdic_to_ascii);

  if (conversions_mask & C_UCASE)
    {
      for (i = 0; i < 256; i++)
        trans_table[i] = toupper (trans_table[i]);
      translation_needed = true;
    }
  else if (conversions_mask & C_LCASE)
    {
      for (i = 0; i < 256; i++)
        trans_table[i] = tolower (trans_table[i]);
      translation_needed = true;
    }

  if (conversions_mask & C_EBCDIC)
    {
      translate_charset (ascii_to_ebcdic);
      newline_character = ascii_to_ebcdic['\n'];
      space_character = ascii_to_ebcdic[' '];
    }
  else if (conversions_mask & C_IBM)
    {
      translate_charset (ascii_to_ibm);
      newline_character = ascii_to_ibm['\n'];
      space_character = ascii_to_ibm[' '];
    }
}

