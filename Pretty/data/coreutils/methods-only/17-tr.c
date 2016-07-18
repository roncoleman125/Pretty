/* Convert string UNESCAPED_STRING (which has been preprocessed to
   convert backslash-escape sequences) of length LEN characters into
   a linked list of the following 5 types of constructs:
      - [:str:] Character class where `str' is one of the 12 valid strings.
      - [=c=] Equivalence class where `c' is any single character.
      - [c*n] Repeat the single character `c' `n' times. n may be omitted.
          However, if `n' is present, it must be a non-negative octal or
          decimal integer.
      - r-s Range of characters from `r' to `s'.  The second endpoint must
          not precede the first in the current collating sequence.
      - c Any other character is interpreted as itself.  */

static bool
build_spec_list (const struct E_string *es, struct Spec_list *result)
{
  char const *p;
  size_t i;

  p = es->s;

  /* The main for-loop below recognizes the 4 multi-character constructs.
     A character that matches (in its context) none of the multi-character
     constructs is classified as `normal'.  Since all multi-character
     constructs have at least 3 characters, any strings of length 2 or
     less are composed solely of normal characters.  Hence, the index of
     the outer for-loop runs only as far as LEN-2.  */

  for (i = 0; i + 2 < es->len; /* empty */)
    {
      if (es_match (es, i, '['))
        {
          bool matched_multi_char_construct;
          size_t closing_bracket_idx;
          unsigned char char_to_repeat;
          count repeat_count;
          int err;

          matched_multi_char_construct = true;
          if (es_match (es, i + 1, ':') || es_match (es, i + 1, '='))
            {
              size_t closing_delim_idx;

              if (find_closing_delim (es, i + 2, p[i + 1], &closing_delim_idx))
                {
                  size_t opnd_str_len = closing_delim_idx - 1 - (i + 2) + 1;
                  char const *opnd_str = p + i + 2;

                  if (opnd_str_len == 0)
                    {
                      if (p[i + 1] == ':')
                        error (0, 0, _("missing character class name `[::]'"));
                      else
                        error (0, 0,
                               _("missing equivalence class character `[==]'"));
                      return false;
                    }

                  if (p[i + 1] == ':')
                    {
                      /* FIXME: big comment.  */
                      if (!append_char_class (result, opnd_str, opnd_str_len))
                        {
                          if (star_digits_closebracket (es, i + 2))
                            goto try_bracketed_repeat;
                          else
                            {
                              char *tmp = make_printable_str (opnd_str,
                                                              opnd_str_len);
                              error (0, 0, _("invalid character class %s"),
                                     quote (tmp));
                              free (tmp);
                              return false;
                            }
                        }
                    }
                  else
                    {
                      /* FIXME: big comment.  */
                      if (!append_equiv_class (result, opnd_str, opnd_str_len))
                        {
                          if (star_digits_closebracket (es, i + 2))
                            goto try_bracketed_repeat;
                          else
                            {
                              char *tmp = make_printable_str (opnd_str,
                                                              opnd_str_len);
                              error (0, 0,
               _("%s: equivalence class operand must be a single character"),
                                     tmp);
                              free (tmp);
                              return false;
                            }
                        }
                    }

                  i = closing_delim_idx + 2;
                  continue;
                }
              /* Else fall through.  This could be [:*] or [=*].  */
            }

        try_bracketed_repeat:

          /* Determine whether this is a bracketed repeat range
             matching the RE \[.\*(dec_or_oct_number)?\].  */
          err = find_bracketed_repeat (es, i + 1, &char_to_repeat,
                                       &repeat_count,
                                       &closing_bracket_idx);
          if (err == 0)
            {
              append_repeated_char (result, char_to_repeat, repeat_count);
              i = closing_bracket_idx + 1;
            }
          else if (err == -1)
            {
              matched_multi_char_construct = false;
            }
          else
            {
              /* Found a string that looked like [c*n] but the
                 numeric part was invalid.  */
              return false;
            }

          if (matched_multi_char_construct)
            continue;

          /* We reach this point if P does not match [:str:], [=c=],
             [c*n], or [c*].  Now, see if P looks like a range `[-c'
             (from `[' to `c').  */
        }

      /* Look ahead one char for ranges like a-z.  */
      if (es_match (es, i + 1, '-'))
        {
          if (!append_range (result, p[i], p[i + 2]))
            return false;
          i += 3;
        }
      else
        {
          append_normal_char (result, p[i]);
          ++i;
        }
    }

  /* Now handle the (2 or fewer) remaining characters p[i]..p[es->len - 1].  */
  for (; i < es->len; i++)
    append_normal_char (result, p[i]);

  return true;
}

