static void
print_named_ascii (size_t fields, size_t blank, void const *block,
                   const char *unused_fmt_string ATTRIBUTE_UNUSED,
                   int width, int pad)
{
  unsigned char const *p = block;
  size_t i;
  int pad_remaining = pad;
  for (i = fields; blank < i; i--)
    {
      int next_pad = pad * (i - 1) / fields;
      int masked_c = *p++ & 0x7f;
      const char *s;
      char buf[2];

      if (masked_c == 127)
        s = "del";
      else if (masked_c <= 040)
        s = charname[masked_c];
      else
        {
          buf[0] = masked_c;
          buf[1] = 0;
          s = buf;
        }

      xprintf ("%*s", pad_remaining - next_pad + width, s);
      pad_remaining = next_pad;
    }
}

