/* Formatting and actual output - service routines.  */

/*-----------------------------------------.
| Prints some NUMBER of spaces on stdout.  |
`-----------------------------------------*/

static void
print_spaces (int number)
{
  int counter;

  for (counter = number; counter > 0; counter--)
    putchar (' ');
}

