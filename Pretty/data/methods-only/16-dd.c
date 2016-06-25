/* Interpret one "conv=..." or similar operand STR according to the
   symbols in TABLE, returning the flags specified.  If the operand
   cannot be parsed, use ERROR_MSGID to generate a diagnostic.  */

static int
parse_symbols (char const *str, struct symbol_value const *table,
               char const *error_msgid)
{
  int value = 0;

  while (true)
    {
      char const *strcomma = strchr (str, ',');
      struct symbol_value const *entry;

      for (entry = table;
           ! (operand_matches (str, entry->symbol, ',') && entry->value);
           entry++)
        {
          if (! entry->symbol[0])
            {
              size_t slen = strcomma ? strcomma - str : strlen (str);
              error (0, 0, "%s: %s", _(error_msgid),
                     quotearg_n_style_mem (0, locale_quoting_style, str, slen));
              usage (EXIT_FAILURE);
            }
        }

      value |= entry->value;
      if (!strcomma)
        break;
      str = strcomma + 1;
    }

  return value;
}

