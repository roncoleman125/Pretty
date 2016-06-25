static void
print_ascii (size_t fields, size_t blank, void const *block,
             const char *unused_fmt_string ATTRIBUTE_UNUSED, int width,
             int pad)
{
  unsigned char const *p = block;
  size_t i;
  int pad_remaining = pad;
  for (i = fields; blank < i; i--)
    {
      int next_pad = pad * (i - 1) / fields;
      unsigned char c = *p++;
      const char *s;
      char buf[4];

      switch (c)
        {
        case '\0':
          s = "\\0";
          break;

        case '\a':
          s = "\\a";
          break;

        case '\b':
          s = "\\b";
          break;

        case '\f':
          s = "\\f";
          break;

        case '\n':
          s = "\\n";
          break;

        case '\r':
          s = "\\r";
          break;

        case '\t':
          s = "\\t";
          break;

        case '\v':
          s = "\\v";
          break;

        default:
          sprintf (buf, (isprint (c) ? "%c" : "%03o"), c);
          s = buf;
        }

      xprintf ("%*s", pad_remaining - next_pad + width, s);
      pad_remaining = next_pad;
    }
}

