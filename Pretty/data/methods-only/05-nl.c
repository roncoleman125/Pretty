/* Switch to a footer section. */

static void
proc_footer (void)
{
  current_type = footer_type;
  current_regex = &footer_regex;
  putchar ('\n');
}

