static void
format_address_paren (uintmax_t address, char c)
{
  putchar ('(');
  format_address_std (address, ')');
  if (c)
    putchar (c);
}

