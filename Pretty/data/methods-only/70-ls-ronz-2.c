static void
prep_non_filename_text (void)
{
  if (color_indicator[C_END].string != NULL)
    put_indicator (&color_indicator[C_END]);
  else
    {
      put_indicator (&color_indicator[C_LEFT]);
      put_indicator (&color_indicator[C_RESET]);
      put_indicator (&color_indicator[C_RIGHT]);
    }
}

