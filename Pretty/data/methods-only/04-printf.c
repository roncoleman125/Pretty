/* Print string STR, evaluating \ escapes. */

static void
print_esc_string (const char *str)
{
  for (; *str; str++)
    if (*str == '\\')
      str += print_esc (str, true);
    else
      putchar (*str);
}

