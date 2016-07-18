/* Print a \ escape sequence starting at ESCSTART.
   Return the number of characters in the escape sequence
   besides the backslash.
   If OCTAL_0 is nonzero, octal escapes are of the form \0ooo, where o
   is an octal digit; otherwise they are of the form \ooo.  */

static int
print_esc (const char *escstart, bool octal_0)
{
  const char *p = escstart + 1;
  int esc_value = 0;		/* Value of \nnn escape. */
  int esc_length;		/* Length of \nnn escape. */

  if (*p == 'x')
    {
      /* A hexadecimal \xhh escape sequence must have 1 or 2 hex. digits.  */
      for (esc_length = 0, ++p;
           esc_length < 2 && isxdigit (to_uchar (*p));
           ++esc_length, ++p)
        esc_value = esc_value * 16 + hextobin (*p);
      if (esc_length == 0)
        error (EXIT_FAILURE, 0, _("missing hexadecimal number in escape"));
      putchar (esc_value);
    }
  else if (isodigit (*p))
    {
      /* Parse \0ooo (if octal_0 && *p == '0') or \ooo (otherwise).
         Allow \ooo if octal_0 && *p != '0'; this is an undocumented
         extension to POSIX that is compatible with Bash 2.05b.  */
      for (esc_length = 0, p += octal_0 && *p == '0';
           esc_length < 3 && isodigit (*p);
           ++esc_length, ++p)
        esc_value = esc_value * 8 + octtobin (*p);
      putchar (esc_value);
    }
  else if (*p && strchr ("\"\\abcefnrtv", *p))
    print_esc_char (*p++);
  else if (*p == 'u' || *p == 'U')
    {
      char esc_char = *p;
      unsigned int uni_value;

      uni_value = 0;
      for (esc_length = (esc_char == 'u' ? 4 : 8), ++p;
           esc_length > 0;
           --esc_length, ++p)
        {
          if (! isxdigit (to_uchar (*p)))
            error (EXIT_FAILURE, 0, _("missing hexadecimal number in escape"));
          uni_value = uni_value * 16 + hextobin (*p);
        }

      /* A universal character name shall not specify a character short
         identifier in the range 00000000 through 00000020, 0000007F through
         0000009F, or 0000D800 through 0000DFFF inclusive. A universal
         character name shall not designate a character in the required
         character set.  */
      if ((uni_value <= 0x9f
           && uni_value != 0x24 && uni_value != 0x40 && uni_value != 0x60)
          || (uni_value >= 0xd800 && uni_value <= 0xdfff))
        error (EXIT_FAILURE, 0, _("invalid universal character name \\%c%0*x"),
               esc_char, (esc_char == 'u' ? 4 : 8), uni_value);

      print_unicode_char (stdout, uni_value, 0);
    }
  else
    {
      putchar ('\\');
      if (*p)
        {
          putchar (*p);
          p++;
        }
    }
  return p - escstart - 1;
}

