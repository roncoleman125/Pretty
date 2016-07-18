static void
dump_hexl_mode_trailer (size_t n_bytes, const char *block)
{
  size_t i;
  fputs ("  >", stdout);
  for (i = n_bytes; i > 0; i--)
    {
      unsigned char c = *block++;
      unsigned char c2 = (isprint (c) ? c : '.');
      putchar (c2);
    }
  putchar ('<');
}

