/* Parse the bracketed repeat-char syntax.  If the P_LEN characters
   beginning with P[ START_IDX ] comprise a valid [c*n] construct,
   then set *CHAR_TO_REPEAT, *REPEAT_COUNT, and *CLOSING_BRACKET_IDX
   and return zero. If the second character following
   the opening bracket is not `*' or if no closing bracket can be
   found, return -1.  If a closing bracket is found and the
   second char is `*', but the string between the `*' and `]' isn't
   empty, an octal number, or a decimal number, print an error message
   and return -2.  */

static int
find_bracketed_repeat (const struct E_string *es, size_t start_idx,
                       unsigned char *char_to_repeat, count *repeat_count,
                       size_t *closing_bracket_idx)
{
  size_t i;

  assert (start_idx + 1 < es->len);
  if (!es_match (es, start_idx + 1, '*'))
    return -1;

  for (i = start_idx + 2; i < es->len && !es->escaped[i]; i++)
    {
      if (es->s[i] == ']')
        {
          size_t digit_str_len = i - start_idx - 2;

          *char_to_repeat = es->s[start_idx];
          if (digit_str_len == 0)
            {
              /* We've matched [c*] -- no explicit repeat count.  */
              *repeat_count = 0;
            }
          else
            {
              /* Here, we have found [c*s] where s should be a string
                 of octal (if it starts with `0') or decimal digits.  */
              char const *digit_str = &es->s[start_idx + 2];
              char *d_end;
              if ((xstrtoumax (digit_str, &d_end, *digit_str == '0' ? 8 : 10,
                               repeat_count, NULL)
                   != LONGINT_OK)
                  || REPEAT_COUNT_MAXIMUM < *repeat_count
                  || digit_str + digit_str_len != d_end)
                {
                  char *tmp = make_printable_str (digit_str, digit_str_len);
                  error (0, 0,
                         _("invalid repeat count %s in [c*n] construct"),
                         quote (tmp));
                  free (tmp);
                  return -2;
                }
            }
          *closing_bracket_idx = i;
          return 0;
        }
    }
  return -1;			/* No bracket found.  */
}

