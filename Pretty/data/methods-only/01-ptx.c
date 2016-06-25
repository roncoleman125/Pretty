/*--------------------------------------------------------------------------.
| Compile the regex represented by REGEX, diagnose and abort if any error.  |
`--------------------------------------------------------------------------*/

static void
compile_regex (struct regex_data *regex)
{
  struct re_pattern_buffer *pattern = &regex->pattern;
  char const *string = regex->string;
  char const *message;

  pattern->buffer = NULL;
  pattern->allocated = 0;
  pattern->fastmap = regex->fastmap;
  pattern->translate = ignore_case ? folded_chars : NULL;

  message = re_compile_pattern (string, strlen (string), pattern);
  if (message)
    error (EXIT_FAILURE, 0, _("%s (for regexp %s)"), message, quote (string));

  /* The fastmap should be compiled before `re_match'.  The following
     call is not mandatory, because `re_search' is always called sooner,
     and it compiles the fastmap if this has not been done yet.  */

  re_compile_fastmap (pattern);
}

