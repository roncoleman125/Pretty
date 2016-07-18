/* If we're tabifying output,

   When print_char encounters white space it keeps track
   of our desired horizontal position and delays printing
   until this function is called. */

static void
print_white_space (void)
{
  int h_new;
  int h_old = output_position;
  int goal = h_old + spaces_not_printed;

  while (goal - h_old > 1
         && (h_new = POS_AFTER_TAB (chars_per_output_tab, h_old)) <= goal)
    {
      putchar (output_tab_char);
      h_old = h_new;
    }
  while (++h_old <= goal)
    putchar (' ');

  output_position = goal;
  spaces_not_printed = 0;
}

