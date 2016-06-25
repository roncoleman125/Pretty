/* Print ELEMENT, preceded by a space if something has already been
   printed.  */

static void
print_element (char const *element)
{
  static bool printed;
  if (printed)
    putchar (' ');
  printed = true;
  fputs (element, stdout);
}


