/* Check if there is a numeric offset after a regular expression.
   STR is the entire command line argument.
   P is the control record for this regular expression.
   NUM is the numeric part of STR. */

static void
check_for_offset (struct control *p, const char *str, const char *num)
{
  if (xstrtoimax (num, NULL, 10, &p->offset, "") != LONGINT_OK)
    error (EXIT_FAILURE, 0, _("%s: integer expected after delimiter"), str);
}

