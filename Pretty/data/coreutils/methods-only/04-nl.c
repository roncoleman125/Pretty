/* Switch to a body section. */

static void
proc_body (void)
{
  current_type = body_type;
  current_regex = &body_regex;
  putchar ('\n');
}

