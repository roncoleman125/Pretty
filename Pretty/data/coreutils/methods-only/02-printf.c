/* Output a single-character \ escape.  */

static void
print_esc_char (char c)
{
  switch (c)
    {
    case 'a':			/* Alert. */
      putchar ('\a');
      break;
    case 'b':			/* Backspace. */
      putchar ('\b');
      break;
    case 'c':			/* Cancel the rest of the output. */
      exit (EXIT_SUCCESS);
      break;
    case 'e':			/* Escape. */
      putchar ('\x1B');
      break;
    case 'f':			/* Form feed. */
      putchar ('\f');
      break;
    case 'n':			/* New line. */
      putchar ('\n');
      break;
    case 'r':			/* Carriage return. */
      putchar ('\r');
      break;
    case 't':			/* Horizontal tab. */
      putchar ('\t');
      break;
    case 'v':			/* Vertical tab. */
      putchar ('\v');
      break;
    default:
      putchar (c);
      break;
    }
}

