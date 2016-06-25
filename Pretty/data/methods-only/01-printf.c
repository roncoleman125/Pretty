static void
verify_numeric (const char *s, const char *end)
{
  if (errno)
    {
      error (0, errno, "%s", s);
      exit_status = EXIT_FAILURE;
    }
  else if (*end)
    {
      if (s == end)
        error (0, 0, _("%s: expected a numeric value"), s);
      else
        error (0, 0, _("%s: value not completely converted"), s);
      exit_status = EXIT_FAILURE;
    }
}

static TYPE								 \
FUNC_NAME (char const *s)						 \
{									 \
  char *end;								 \
  TYPE val;								 \
                                                                         \
  if (*s == '\"' || *s == '\'')						 \
    {									 \
      unsigned char ch = *++s;						 \
      val = ch;								 \
      /* If POSIXLY_CORRECT is not set, then give a warning that there	 \
         are characters following the character constant and that GNU	 \
         printf is ignoring those characters.  If POSIXLY_CORRECT *is*	 \
         set, then don't give the warning.  */				 \
      if (*++s != 0 && !posixly_correct)				 \
        error (0, 0, _(cfcc_msg), s);					 \
    }									 \
  else									 \
    {									 \
      errno = 0;							 \
      val = (LIB_FUNC_EXPR);						 \
      verify_numeric (s, end);						 \
    }									 \
  return val;								 \
}									 \

STRTOX (intmax_t,    vstrtoimax, strtoimax (s, &end, 0))
STRTOX (uintmax_t,   vstrtoumax, strtoumax (s, &end, 0))
STRTOX (long double, vstrtold,   c_strtold (s, &end))

