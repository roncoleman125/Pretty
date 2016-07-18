/* Search forward starting at START_IDX for the 2-char sequence
   (PRE_BRACKET_CHAR,']') in the string P of length P_LEN.  If such
   a sequence is found, set *RESULT_IDX to the index of the first
   character and return true.  Otherwise return false.  P may contain
   zero bytes.  */

static bool
find_closing_delim (const struct E_string *es, size_t start_idx,
                    char pre_bracket_char, size_t *result_idx)
{
  size_t i;

  for (i = start_idx; i < es->len - 1; i++)
    if (es->s[i] == pre_bracket_char && es->s[i + 1] == ']'
        && !es->escaped[i] && !es->escaped[i + 1])
      {
        *result_idx = i;
        return true;
      }
  return false;
}

