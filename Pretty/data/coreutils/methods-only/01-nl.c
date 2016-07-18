/* Set the command line flag TYPEP and possibly the regex pointer REGEXP,
   according to `optarg'.  */

static bool
build_type_arg (char const **typep,
                struct re_pattern_buffer *regexp, char *fastmap)
{
  char const *errmsg;
  bool rval = true;

  switch (*optarg)
    {
    case 'a':
    case 't':
    case 'n':
      *typep = optarg;
      break;
    case 'p':
      *typep = optarg++;
      regexp->buffer = NULL;
      regexp->allocated = 0;
      regexp->fastmap = fastmap;
      regexp->translate = NULL;
      re_syntax_options =
        RE_SYNTAX_POSIX_BASIC & ~RE_CONTEXT_INVALID_DUP & ~RE_NO_EMPTY_RANGES;
      errmsg = re_compile_pattern (optarg, strlen (optarg), regexp);
      if (errmsg)
        error (EXIT_FAILURE, 0, "%s", errmsg);
      break;
    default:
      rval = false;
      break;
    }
  return rval;
}

