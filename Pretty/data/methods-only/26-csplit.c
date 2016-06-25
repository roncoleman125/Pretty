/* Split the input file according to the control records we have built. */

static void
split_file (void)
{
  size_t i;

  for (i = 0; i < control_used; i++)
    {
      uintmax_t j;
      if (controls[i].regexpr)
        {
          for (j = 0; (controls[i].repeat_forever
                       || j <= controls[i].repeat); j++)
            process_regexp (&controls[i], j);
        }
      else
        {
          for (j = 0; (controls[i].repeat_forever
                       || j <= controls[i].repeat); j++)
            process_line_count (&controls[i], j);
        }
    }

  create_output_file ();
  dump_rest_of_file ();
  close_output_file ();
}

