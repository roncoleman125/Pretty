static void
set_normal_color (void)
{
  if (print_with_color && is_colored (C_NORM))
    {
      put_indicator (&color_indicator[C_LEFT]);
      put_indicator (&color_indicator[C_NORM]);
      put_indicator (&color_indicator[C_RIGHT]);
    }
}

