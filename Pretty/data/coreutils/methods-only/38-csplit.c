/* Check that the printf format conversion specifier *FORMAT is valid
   and compatible with FLAGS.  Change it to 'u' if it is 'd' or 'i',
   since the format will be used with an unsigned value.  */
static void
check_format_conv_type (char *format, int flags)
{
  unsigned char ch = *format;
  int compatible_flags = FLAG_THOUSANDS;

  switch (ch)
    {
    case 'd':
    case 'i':
      *format = 'u';
      break;

    case 'u':
      break;

    case 'o':
    case 'x':
    case 'X':
      compatible_flags = FLAG_ALTERNATIVE;
      break;

    case 0:
      error (EXIT_FAILURE, 0, _("missing conversion specifier in suffix"));
      break;

    default:
      if (isprint (ch))
        error (EXIT_FAILURE, 0,
               _("invalid conversion specifier in suffix: %c"), ch);
      else
        error (EXIT_FAILURE, 0,
               _("invalid conversion specifier in suffix: \\%.3o"), ch);
    }

  if (flags & ~ compatible_flags)
    error (EXIT_FAILURE, 0,
           _("invalid flags in conversion specification: %%%c%c"),
           (flags & ~ compatible_flags & FLAG_ALTERNATIVE ? '#' : '\''), ch);
}

