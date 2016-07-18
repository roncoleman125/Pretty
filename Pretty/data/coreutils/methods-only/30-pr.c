/* Print a header.

   Formfeeds are assumed to use up two lines at the beginning of
   the page. */

static void
print_header (void)
{
  char page_text[256 + INT_STRLEN_BOUND (page_number)];
  int available_width;
  int lhs_spaces;
  int rhs_spaces;

  output_position = 0;
  pad_across_to (chars_per_margin);
  print_white_space ();

  if (page_number == 0)
    error (EXIT_FAILURE, 0, _("page number overflow"));

  /* The translator must ensure that formatting the translation of
     "Page %"PRIuMAX does not generate more than (sizeof page_text - 1)
     bytes.  */
  sprintf (page_text, _("Page %"PRIuMAX), page_number++);
  available_width = header_width_available - mbswidth (page_text, 0);
  available_width = MAX (0, available_width);
  lhs_spaces = available_width >> 1;
  rhs_spaces = available_width - lhs_spaces;

  printf ("\n\n%*s%s%*s%s%*s%s\n\n\n",
          chars_per_margin, "",
          date_text, lhs_spaces, " ",
          file_text, rhs_spaces, " ", page_text);

  print_a_header = false;
  output_position = 0;
}

