static void
format_address_std (uintmax_t address, char c)
{
  char buf[MAX_ADDRESS_LENGTH + 2];
  char *p = buf + sizeof buf;
  char const *pbound;

  *--p = '\0';
  *--p = c;
  pbound = p - address_pad_len;

  /* Use a special case of the code for each base.  This is measurably
     faster than generic code.  */
  switch (address_base)
    {
    case 8:
      do
        *--p = '0' + (address & 7);
      while ((address >>= 3) != 0);
      break;

    case 10:
      do
        *--p = '0' + (address % 10);
      while ((address /= 10) != 0);
      break;

    case 16:
      do
        *--p = "0123456789abcdef"[address & 15];
      while ((address >>= 4) != 0);
      break;
    }

  while (pbound < p)
    *--p = '0';

  fputs (p, stdout);
}

