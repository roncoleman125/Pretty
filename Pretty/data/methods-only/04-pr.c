/* Parse options of the form -scNNN.

   Example: -nck, where 'n' is the option, c is the optional number
   separator, and k is the optional width of the field used when printing
   a number. */

static void
getoptarg (char *arg, char switch_char, char *character, int *number)
{
  if (!ISDIGIT (*arg))
    *character = *arg++;
  if (*arg)
    {
      long int tmp_long;
      if (xstrtol (arg, NULL, 10, &tmp_long, "") != LONGINT_OK
          || tmp_long <= 0 || tmp_long > INT_MAX)
        {
          error (0, 0,
             _("`-%c' extra characters or invalid number in the argument: %s"),
                 switch_char, quote (arg));
          usage (EXIT_FAILURE);
        }
      *number = tmp_long;
    }
}

