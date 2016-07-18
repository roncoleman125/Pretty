/* Extract the regular expression from STR and check for a numeric offset.
   STR should start with the regexp delimiter character.
   Return a new control record for the regular expression.
   ARGNUM is the ARGV index of STR.
   Unless IGNORE is true, mark these lines for output. */

static struct control *
extract_regexp (int argnum, bool ignore, char const *str)
{
  size_t len;			/* Number of bytes in this regexp. */
  char delim = *str;
  char const *closing_delim;
  struct control *p;
  const char *err;

  closing_delim = strrchr (str + 1, delim);
  if (closing_delim == NULL)
    error (EXIT_FAILURE, 0,
           _("%s: closing delimiter `%c' missing"), str, delim);

  len = closing_delim - str - 1;
  p = new_control_record ();
  p->argnum = argnum;
  p->ignore = ignore;

  p->regexpr = true;
  p->re_compiled.buffer = NULL;
  p->re_compiled.allocated = 0;
  p->re_compiled.fastmap = xmalloc (UCHAR_MAX + 1);
  p->re_compiled.translate = NULL;
  re_syntax_options =
    RE_SYNTAX_POSIX_BASIC & ~RE_CONTEXT_INVALID_DUP & ~RE_NO_EMPTY_RANGES;
  err = re_compile_pattern (str + 1, len, &p->re_compiled);
  if (err)
    {
      error (0, 0, _("%s: invalid regular expression: %s"), str, err);
      cleanup_fatal ();
    }

  if (closing_delim[1])
    check_for_offset (p, str, closing_delim + 1);

  return p;
}

