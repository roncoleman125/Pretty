static bool
print_type_indicator (bool stat_ok, mode_t mode, enum filetype type)
{
  char c = get_type_indicator (stat_ok, mode, type);
  if (c)
    DIRED_PUTCHAR (c);
  return !!c;
}

