/* Do the : operator.
   SV is the VALUE for the lhs (the string),
   PV is the VALUE for the rhs (the pattern).  */

static VALUE *
docolon (VALUE *sv, VALUE *pv)
{
  VALUE *v IF_LINT ( = NULL);
  const char *errmsg;
  struct re_pattern_buffer re_buffer;
  char fastmap[UCHAR_MAX + 1];
  struct re_registers re_regs;
  regoff_t matchlen;

  tostring (sv);
  tostring (pv);

  re_regs.num_regs = 0;
  re_regs.start = NULL;
  re_regs.end = NULL;

  re_buffer.buffer = NULL;
  re_buffer.allocated = 0;
  re_buffer.fastmap = fastmap;
  re_buffer.translate = NULL;
  re_syntax_options =
    RE_SYNTAX_POSIX_BASIC & ~RE_CONTEXT_INVALID_DUP & ~RE_NO_EMPTY_RANGES;
  errmsg = re_compile_pattern (pv->u.s, strlen (pv->u.s), &re_buffer);
  if (errmsg)
    error (EXPR_INVALID, 0, "%s", errmsg);
  re_buffer.newline_anchor = 0;

  matchlen = re_match (&re_buffer, sv->u.s, strlen (sv->u.s), 0, &re_regs);
  if (0 <= matchlen)
    {
      /* Were \(...\) used? */
      if (re_buffer.re_nsub > 0)
        {
          sv->u.s[re_regs.end[1]] = '\0';
          v = str_value (sv->u.s + re_regs.start[1]);
        }
      else
        v = int_value (matchlen);
    }
  else if (matchlen == -1)
    {
      /* Match failed -- return the right kind of null.  */
      if (re_buffer.re_nsub > 0)
        v = str_value ("");
      else
        v = int_value (0);
    }
  else
    error (EXPR_FAILURE,
           (matchlen == -2 ? errno : EOVERFLOW),
           _("error in regular expression matcher"));

  if (0 < re_regs.num_regs)
    {
      free (re_regs.start);
      free (re_regs.end);
    }
  re_buffer.fastmap = NULL;
  regfree (&re_buffer);
  return v;
}

